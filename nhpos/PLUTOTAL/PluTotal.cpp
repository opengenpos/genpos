// PluTotal.cpp : Defines the initialization routines for the DLL.
//

#include	"stdafx.h"

//  files included through other file includes
//#include	"PluTtlD.h"
//#include	"CnAdoCe.h"
//#include	"PluTtlDB.h"
//#include	"PluTtlM.h"
//#include	"PluTtlAp.h"

#include	"PluTotal.h"

#include	"NDbgTrc.h"	// TRACER object

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CPluTotalApp

BEGIN_MESSAGE_MAP(CPluTotalApp, CWinApp)
	//{{AFX_MSG_MAP(CPluTotalApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPluTotalApp construction

CPluTotalApp::CPluTotalApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPluTotalApp object

CPluTotalApp		theApp;
CnPluTotalAPI		plutotal;	// PluTotal Object

CnDbgTrace			dbgtrc(TEXT("\\FlashDisk\\PluTtlLog.txt"));
CnDbgTrace			dbgtrc2(TEXT("\\FlashDisk\\PluTtlLog2.txt"));

#ifndef	_PLUTOTAL_NOP

PLUTOTAL_EXTERN_C	 __declspec( dllexport ) ULONG PLUTTLAPI	PluTotalOpen(PLUTOTAL_DB_HANDLE * phdDB, SHORT fPTD, SHORT fTmp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ULONG  ulStatus = plutotal.Open(phdDB, fPTD, fTmp);
	if (ulStatus != PLUTOTAL_SUCCESS) {
		char   xBuff[128];
		sprintf (xBuff, "**WARNING: PluTotalOpen():  fPTD =  %d  ulStatus = %d", fPTD, ulStatus);
		NHPOS_ASSERT_TEXT((ulStatus == PLUTOTAL_SUCCESS), xBuff);
	}
	return ulStatus;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalClose(PLUTOTAL_DB_HANDLE hdDB){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ULONG  ulStatus = plutotal.Close(hdDB);
	if (ulStatus != PLUTOTAL_SUCCESS) {
		char   xBuff[128];
		sprintf (xBuff, "**WARNING: PluTotalClose():  hdDB = %d  ulStatus = %d", hdDB, ulStatus);
		NHPOS_ASSERT_TEXT((ulStatus == PLUTOTAL_SUCCESS), xBuff);
	}
	return ulStatus;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalGetRecCnt(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,ULONG *ulRecCnt){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ULONG ulStatus = plutotal.GetRecordCnt(hdDB,nTblID,ulRecCnt);
	return	ulStatus;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalCreateTbl(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const ULONG ulRec){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ULONG  ulStatus = plutotal.CreateTable(hdDB,nTblID,ulRec);

	if (ulStatus != PLUTOTAL_SUCCESS) {
		char   xBuff[128];
		sprintf (xBuff, "==NOTE: PluTotalCreateTbl():  hdDB = %d  nTblID %d  ulStatus = %d", hdDB, nTblID, ulStatus);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
	return	ulStatus;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalDropTbl(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ULONG  ulStatus = plutotal.DropTable(hdDB,nTblID);
	if (ulStatus != PLUTOTAL_SUCCESS) {
		char   xBuff[128];
		sprintf (xBuff, "==NOTE: PluTotalDropTbl():  hdDB = %d  nTblID %d  ulStatus = %d", hdDB, nTblID, ulStatus);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
	return	ulStatus;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalMoveTbl(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nFromTblID,const SHORT nToTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ULONG  ulStatus = plutotal.MoveTable(hdDB, nFromTblID, nToTblID);
	if (ulStatus != PLUTOTAL_SUCCESS) {
		char   xBuff[128];
		sprintf (xBuff, "==NOTE: PluTotalMoveTbl():  hdDB = %d  nFromTblID %d  nToTblID %d  ulStatus = %d", hdDB, nFromTblID, nToTblID, ulStatus);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
	return	ulStatus;
}

PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalExportTblToFile(const PLUTOTAL_DB_HANDLE hdDB, const SHORT nFromTblID, const TCHAR *pFileName){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return	plutotal.ExportTableToFile(hdDB, nFromTblID, pFileName);
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalCheckTbl(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.CheckTable(hdDB,nTblID);
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalInsert(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const PLUTOTAL_REC RecData){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ULONG	ulStatus = plutotal.Insert(hdDB,nTblID,RecData);
	if (ulStatus != PLUTOTAL_SUCCESS) {
		char   xBuff[128];
		sprintf (xBuff, "**WARNING: PluTotalInsert():  hdDB = %d  nTblID %d  ulStatus = %d", hdDB, nTblID, ulStatus);
		NHPOS_ASSERT_TEXT((ulStatus == PLUTOTAL_SUCCESS), xBuff);
#if defined(DCURRENCY_LONGLONG)
		sprintf(xBuff, "           PLU %-14.14S  Count %d  lTotal %lld", RecData.auchPluNumber, RecData.lCounter, RecData.lTotal);
#else
		sprintf (xBuff, "           PLU %-14.14S  Count %d  lTotal %d", RecData.auchPluNumber, RecData.lCounter, RecData.lTotal);
#endif
		NHPOS_ASSERT_TEXT((ulStatus == PLUTOTAL_SUCCESS), xBuff);
	}
	return	ulStatus;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalDelete(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const TCHAR pPluNo[],const BYTE byAdjIdx){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ULONG	ulStatus = plutotal.Delete(hdDB,nTblID,pPluNo,byAdjIdx);
	if (ulStatus != PLUTOTAL_SUCCESS) {
		char   xBuff[128];
		sprintf (xBuff, "**WARNING: PluTotalDelete():  hdDB = %d  nTblID %d  ulStatus = %d", hdDB, nTblID, ulStatus);
		NHPOS_ASSERT_TEXT((ulStatus == PLUTOTAL_SUCCESS), xBuff);
		sprintf (xBuff, "           PLU %-14.14S", pPluNo);
		NHPOS_ASSERT_TEXT((ulStatus == PLUTOTAL_SUCCESS), xBuff);
	}
	return	ulStatus;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalUpdate(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const PLUTOTAL_REC RecData){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ULONG	ulStatus = plutotal.Update(hdDB,nTblID,RecData);
	if (ulStatus != PLUTOTAL_SUCCESS) {
		char   xBuff[128];
		sprintf (xBuff, "**WARNING: PluTotalUpdate():  hdDB = %d  nTblID %d  ulStatus = %d", hdDB, nTblID, ulStatus);
		NHPOS_ASSERT_TEXT((ulStatus == PLUTOTAL_SUCCESS), xBuff);
#if defined(DCURRENCY_LONGLONG)
		sprintf (xBuff, "           PLU %-14.14S  Count %d  lTotal %lld", RecData.auchPluNumber, RecData.lCounter, RecData.lTotal);
#else
		sprintf(xBuff, "           PLU %-14.14S  Count %d  lTotal %d", RecData.auchPluNumber, RecData.lCounter, RecData.lTotal);
#endif
		NHPOS_ASSERT_TEXT((ulStatus == PLUTOTAL_SUCCESS), xBuff);
	}
	return	ulStatus;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalFind(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const TCHAR pPluNo[],const BYTE byAdjIdx,PLUTOTAL_REC *pRecBuf){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ULONG	ulStatus = plutotal.Find(hdDB,nTblID,pPluNo,byAdjIdx,pRecBuf);
	return	ulStatus;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalExecSQL_Dbg(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const CHAR * szSqlCode){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.dbg_ExecSQL(hdDB,nTblID,szSqlCode);
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalSelectRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const ULONG SearchCond){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return	plutotal.SelectRec(hdDB,nTblID,SearchCond);
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalFirstRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.FirstRec(hdDB,nTblID);
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalLastRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.LastRec(hdDB,nTblID);
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalNextRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.NextRec(hdDB,nTblID);
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalPrevRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.PrevRec(hdDB,nTblID);
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalGetRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,PLUTOTAL_REC *pRecBuf){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.GetRec(hdDB,nTblID,pRecBuf);
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalGetNumRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,ULONG * pulRecNum){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.GetNumRec(hdDB,nTblID,pulRecNum);
}


PLUTOTAL_EXTERN_C	__declspec( dllexport ) ULONG   PLUTTLAPI  PluTotalReleaseRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.ReleaseRec(hdDB,nTblID);
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalGetStsInfo(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,PLUTOTAL_STATUS * pStatusRec){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ULONG ulStatus = plutotal.GetStsInfo(hdDB,nTblID,pStatusRec);
	if (ulStatus != PLUTOTAL_SUCCESS) {
		char   xBuff[128];
		sprintf (xBuff, "**WARNING: PluTotalGetStsInfo():  hdDB = %d  nTblID %d  ulStatus = %d", hdDB, nTblID, ulStatus);
		NHPOS_ASSERT_TEXT((ulStatus == PLUTOTAL_SUCCESS), xBuff);
	}
	return	ulStatus;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalSetStsInfo(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const PLUTOTAL_STATUS StatusRec){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ULONG ulStatus = plutotal.SetStsInfo(hdDB,nTblID,StatusRec);
	if (ulStatus != PLUTOTAL_SUCCESS) {
		char   xBuff[128];
		sprintf (xBuff, "**WARNING: PluTotalSetStsInfo():  hdDB = %d  nTblID %d  ulStatus = %d", hdDB, nTblID, ulStatus);
		NHPOS_ASSERT_TEXT((ulStatus == PLUTOTAL_SUCCESS), xBuff);
	}
	return	ulStatus;
}

PLUTOTAL_EXTERN_C	__declspec( dllexport ) ULONG   PLUTTLAPI  PluTotalRestoreDB(const PLUTOTAL_DB_HANDLE hdDB, const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.RestoreDB(hdDB,nTblID);
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG   PLUTTLAPI  PluTotalOpenFile(PLUTOTAL_FILE_HANDLE * phdDBFile,const SHORT nTblID,const ULONG ulOption){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.OpenDBFile(phdDBFile,nTblID,ulOption);
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalCloseFile(const PLUTOTAL_FILE_HANDLE hdDBFile){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.CloseDBFile(hdDBFile);
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalReadFile(const PLUTOTAL_FILE_HANDLE hdDBFile,const ULONG uloffset,const LONG lLen,UCHAR * pucBuf,LONG * plRedLen){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.ReadDbFile(hdDBFile, uloffset, (ULONG)lLen,(VOID *)pucBuf,(ULONG *)plRedLen);
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalWriteFile(const PLUTOTAL_FILE_HANDLE hdDBFile,const ULONG uloffset,const LONG lLen,const UCHAR * pucBuf){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.WriteDbFile(hdDBFile, uloffset, (ULONG)lLen,(VOID *)pucBuf);
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalDelFile(const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.DeleteDbFile(nTblID);
}

PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalMoveFile(const SHORT nFromTblID, const SHORT nToTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.MoveDbFile(nFromTblID, nToTblID);
}

PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalCopyFile(const SHORT nFromTblID, const SHORT nToTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.CopyDbFile(nFromTblID, nToTblID);
}

// ### ADD Saratoga (053000)
PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalStartBackup(void){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.StartBackup();
}

PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalStartRestore(void){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.StartRestore();
}

PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalGetBackupStatus(void){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.GetBackupStatus();
}


// *** DEBUG & TESTS **********************************************************
PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalMakeTestData_Dbg(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,
	const LONG lFrom,const LONG lTo,const LONG lStep){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	plutotal.__DbgMakeTestData(hdDB,nTblID,lFrom,lTo,lStep);
}


//PLUTOTAL_EXTERN_C	__declspec( dllexport )	VOID 	PLUTTLAPI  PluTotalPutLog_Dbg(char * szLabel1,int len1,char * szLabel2,int len2,char * szLabel3,int len3)
PLUTOTAL_EXTERN_C	__declspec( dllexport )	VOID 	PLUTTLAPI  PluTotalPutLog_Dbg(LPCTSTR tMsg1,LPCTSTR tMsg2,LPCTSTR tMsg3){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
/*
	TCHAR tMsg1[255],tMsg2[255],tMsg3[255];
	memset(tMsg1,0,sizeof(TCHAR)*255);
	memset(tMsg2,0,sizeof(TCHAR)*255);
	memset(tMsg3,0,sizeof(TCHAR)*255);
	mbstowcs(tMsg1,szLabel1,len1);
	mbstowcs(tMsg2,szLabel2,len2);
	mbstowcs(tMsg3,szLabel3,len3);
*/
#ifdef	_DEBUG
	dbgtrc2.PutLog(tMsg1,tMsg2,tMsg3);
#endif

}


#else
#pragma	message("### WARNING (PluTotal.cpp)###  This is NOP version!!! ")

// NO OPERATION VERION
PLUTOTAL_EXTERN_C	 __declspec( dllexport ) ULONG PLUTTLAPI	PluTotalOpen(PLUTOTAL_DB_HANDLE * phdDB, SHORT fPTD, SHORT fTmp){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*phdDB = 1; /* DUMMY handle*/
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalClose(PLUTOTAL_DB_HANDLE hdDB){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalGetRecCnt(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,ULONG *ulRecCnt){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*ulRecCnt = 0L; /* DUMMY */
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalCreateTbl(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const ULONG ulRec){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalDropTbl(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalMoveTbl(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nFromTblID,const SHORT nToTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalInsert(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const PLUTOTAL_REC RecData){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalDelete(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const TCHAR pPluNo[],const BYTE byAdjIdx){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalUpdate(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const PLUTOTAL_REC RecData){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalFind(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const TCHAR pPluNo[],const UCHAR byAdjIdx,PLUTOTAL_REC *pRecBuf){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalExecSQL_Dbg(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const CHAR * szSqlCode){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalSelectRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const ULONG SearchCond){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalFirstRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalLastRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalNextRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalPrevRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalGetRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,PLUTOTAL_REC *pRecBuf){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalGetNumRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,ULONG * pulRecNum){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport ) ULONG   PLUTTLAPI  PluTotalReleaseRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalGetStsInfo(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,PLUTOTAL_STATUS * pStatusRec){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalSetStsInfo(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const PLUTOTAL_STATUS StatusRec){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}

PLUTOTAL_EXTERN_C	__declspec( dllexport ) ULONG   PLUTTLAPI  PluTotalRestoreDB(const PLUTOTAL_DB_HANDLE hdDB, const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG   PLUTTLAPI  PluTotalOpenFile(PLUTOTAL_FILE_HANDLE * phdDBFile,const SHORT nTblID,const ULONG ulOption){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalCloseFile(const PLUTOTAL_FILE_HANDLE hdDBFile){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalReadFile(const PLUTOTAL_FILE_HANDLE hdDBFile,const ULONG uloffset,const LONG lLen,UCHAR * pucBuf,LONG * plRedLen){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}


PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalWriteFile(const PLUTOTAL_FILE_HANDLE hdDBFile,const ULONG uloffset,const LONG lLen,const UCHAR * pucBuf){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}

// ### ADD Saratoga (053000)
PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalStartBackup(void){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}

PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalStartRestore(void){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}

PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalGetBackupStatus(void){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}

PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	PLUTTLAPI	PluTotalCheckTbl(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}

PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalDelFile(const SHORT nTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}

PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalMoveFile(const SHORT nFromTblID, const SHORT nToTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}

PLUTOTAL_EXTERN_C	__declspec( dllexport )	ULONG	  PLUTTLAPI  PluTotalCopyFile(const SHORT nFromTblID, const SHORT nToTblID){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return	PLUTOTAL_SUCCESS;
}

PLUTOTAL_EXTERN_C	__declspec( dllexport )	VOID 	PLUTTLAPI  PluTotalPutLog_Dbg(LPCTSTR tMsg1,LPCTSTR tMsg2,LPCTSTR tMsg3){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
/*
	TCHAR tMsg1[255],tMsg2[255],tMsg3[255];
	memset(tMsg1,0,sizeof(TCHAR)*255);
	memset(tMsg2,0,sizeof(TCHAR)*255);
	memset(tMsg3,0,sizeof(TCHAR)*255);
	mbstowcs(tMsg1,szLabel1,len1);
	mbstowcs(tMsg2,szLabel2,len2);
	mbstowcs(tMsg3,szLabel3,len3);
*/
#ifdef	_DEBUG
	dbgtrc2.PutLog(tMsg1,tMsg2,tMsg3);
#endif

}

#endif
