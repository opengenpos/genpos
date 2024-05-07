// PluTtlM.cpp: implementation of the CnPluTotal class.
//
//////////////////////////////////////////////////////////////////////
/*
 * change log
 *  date      | revision | name     | note
 * -----------+----------+----------+----------------------------------
 *  Aug-09-00 | V1.0.0.4 | m.teraki | Solve memory leak probrem
 *            |          |
 *            |          |
 */

// #define _CNPLUTOTAL_TRACE

#include "stdafx.h"
#include "PluTtlM.h"
#include "PluTtlD.h"
#include "NStr.h"

#if defined(_CNPLUTOTAL_TRACE) || defined(_CNPLUTOTAL_DBFILE_TRACE)
    #include    "NDbgTrc.h"
    extern      CnDbgTrace          dbgtrc;
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int    CnPluTotal::s_nObjCnt;              // object counter

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CnPluTotal::CnPluTotal(){
    CreateObject();
}

CnPluTotal::~CnPluTotal(){
    DestroyObject();
}

VOID CnPluTotal::SetPluTotalStatusVal (ULONG ulResetSts,DATE_TIME *dtFrom,DATE_TIME *dtTo,
	DCURRENCY lAllTotal, DCURRENCY lAmount, PLUTOTAL_STATUS *ppsRec) {

	ppsRec->usSize = sizeof(PLUTOTAL_STATUS);
	ppsRec->ulResetStatus = ulResetSts;

	/* from date */
	if(dtFrom != NULL){
		ppsRec->dateFrom.usYear	= dtFrom->usYear;
		ppsRec->dateFrom.usMonth= dtFrom->usMonth;
		ppsRec->dateFrom.usDay	= dtFrom->usMDay;
		ppsRec->dateFrom.usHour	= dtFrom->usHour;
		ppsRec->dateFrom.usMin	= dtFrom->usMin;
		ppsRec->dateFrom.usSec	= dtFrom->usSec;
	}

	/* to date */
	if(dtTo != NULL){
		ppsRec->dateTo.usYear	= dtTo->usYear;
		ppsRec->dateTo.usMonth	= dtTo->usMonth;
		ppsRec->dateTo.usDay	= dtTo->usMDay;
		ppsRec->dateTo.usHour	= dtTo->usHour;
		ppsRec->dateTo.usMin	= dtTo->usMin;
		ppsRec->dateTo.usSec	= dtTo->usSec;
	}

	/* All Total & Amount */
	ppsRec->lAllTotal = lAllTotal;
	ppsRec->lAmount = lAmount;	/* ### ADD 2172 (Saratoga)(041800) */

	/* version */
	ppsRec->lVersion = PLUTOTAL_DB_VERSION;	/* fixed!!! */
}

void    CnPluTotal::CreateObject(){
    CString strDbPathWrk(_T(""));
    // ### DEL m_strDbPath.Empty();
    m_Regs.GetTotalPath(NULL,TRUE);
    m_Regs.GetBackupPath(NULL,TRUE);

    // initialize backup object (TBC)
    m_BackUp.Initialize();
    m_BackUp.SetBkupPath(m_Regs.m_strBackupPath);
    m_BackUp.SetWorkPath(m_Regs.m_strTotalPath);
    m_BackUp.SetFileName(PLUTOTAL_DBNM_DAILY_CUR);
    m_BackUp.SetFileName(PLUTOTAL_DBNM_PTD_CUR);
//    m_BackUp.SetFileName(TEXT("PLUTTLDC.cdb"));
//    m_BackUp.SetFileName(TEXT("PLUTTLPC.cdb"));

    if(s_nObjCnt == 0){
        s_lstDb[0].nId      = PLUTOTAL_ID_DAILY_CUR ;
        s_lstDb[0].pDb      = NULL;
        s_lstDb[0].strName  = PLUTOTAL_DBNM_DAILY_CUR;
        s_lstDb[0].strPath  = m_Regs.m_strTotalPath;
        s_lstDb[0].lstTbl[0].nId        = PLUTOTAL_ID_DAILY_CUR;
        s_lstDb[0].lstTbl[1].nId        = PLUTOTAL_ID_DAILY_CUR_S;
        s_lstDb[0].lstTbl[2].nId        = -1;

		s_lstDb[1].nId      = PLUTOTAL_ID_DAILY_SAV ;
        s_lstDb[1].pDb      = NULL;
        s_lstDb[1].strName  = PLUTOTAL_DBNM_DAILY_SAV;
        s_lstDb[1].strPath  = m_Regs.m_strBackupPath;
        s_lstDb[1].lstTbl[0].nId        = PLUTOTAL_ID_DAILY_SAV;
        s_lstDb[1].lstTbl[1].nId        = PLUTOTAL_ID_DAILY_SAV_S;
        s_lstDb[1].lstTbl[2].nId        = -1;

		s_lstDb[2].nId      = PLUTOTAL_ID_PTD_CUR   ;
        s_lstDb[2].pDb      = NULL;
        s_lstDb[2].strName  = PLUTOTAL_DBNM_PTD_CUR;
        s_lstDb[2].strPath  = m_Regs.m_strTotalPath;
        s_lstDb[2].lstTbl[0].nId        = PLUTOTAL_ID_PTD_CUR;
        s_lstDb[2].lstTbl[1].nId        = PLUTOTAL_ID_PTD_CUR_S;
        s_lstDb[2].lstTbl[2].nId        = -1;

		s_lstDb[3].nId      = PLUTOTAL_ID_PTD_SAV   ;
        s_lstDb[3].pDb      = NULL;
        s_lstDb[3].strName  = PLUTOTAL_DBNM_PTD_SAV;
        s_lstDb[3].strPath  = m_Regs.m_strBackupPath;
        s_lstDb[3].lstTbl[0].nId        = PLUTOTAL_ID_PTD_SAV;
        s_lstDb[3].lstTbl[1].nId        = PLUTOTAL_ID_PTD_SAV_S;
        s_lstDb[3].lstTbl[2].nId        = -1;

		s_lstDb[4].nId      = PLUTOTAL_ID_TMP_CUR   ;				/* */
        s_lstDb[4].pDb      = NULL;
        s_lstDb[4].strName  = PLUTOTAL_DBNM_TMP_CUR;			/* */
        s_lstDb[4].strPath  = m_Regs.m_strTotalPath;	/* */
        s_lstDb[4].lstTbl[0].nId        = PLUTOTAL_ID_TMP_CUR;		/* */
        s_lstDb[4].lstTbl[1].nId        = PLUTOTAL_ID_TMP_CUR_S;	/* */
        s_lstDb[4].lstTbl[2].nId        = -1;					/* */

		s_lstDb[5].nId      = -1;
        s_lstDb[5].pDb      = NULL;
        s_lstDb[5].strName.Empty();
        s_lstDb[5].strPath.Empty();

        // *** INDEX definition
        // index names
        s_lstDb[0].lstTbl[0].strIdxName = _T( "idx1" );
        s_lstDb[0].lstTbl[1].strIdxName = _T( "idx2" );
        s_lstDb[0].lstTbl[2].strIdxName.Empty();
        s_lstDb[1].lstTbl[0].strIdxName = _T( "idx3" );
        s_lstDb[1].lstTbl[1].strIdxName = _T( "idx4" );
        s_lstDb[1].lstTbl[2].strIdxName.Empty();
        s_lstDb[2].lstTbl[0].strIdxName = _T( "idx5" );
        s_lstDb[2].lstTbl[1].strIdxName = _T( "idx6" );
        s_lstDb[2].lstTbl[2].strIdxName.Empty();
        s_lstDb[3].lstTbl[0].strIdxName = _T( "idx7" );
        s_lstDb[3].lstTbl[1].strIdxName = _T( "idx8" );
        s_lstDb[3].lstTbl[2].strIdxName.Empty();
        s_lstDb[4].lstTbl[0].strIdxName = _T( "idx9" );		/* */
        s_lstDb[4].lstTbl[1].strIdxName = _T( "idx10" );	/* */
        s_lstDb[4].lstTbl[2].strIdxName.Empty();			/* */

		for (int iIndex = 0; iIndex < STSTBL_NUM; iIndex++) {
			s_lstDb[iIndex].lstTbl[0].strName    = PLUTPTAL_TBNM_TOTAL;
			s_lstDb[iIndex].lstTbl[1].strName    = PLUTPTAL_TBNM_STATUS;
			s_lstDb[iIndex].lstTbl[2].strName.Empty();
			s_lstDb[iIndex].lstTbl[0].strFldFormat= PLUTOTAL_TOTAL_TBL_FRM;
			s_lstDb[iIndex].lstTbl[1].strFldFormat= PLUTOTAL_STATUS_TBL_FRM;
			s_lstDb[iIndex].lstTbl[2].strFldFormat.Empty();

			s_lstDb[iIndex].lstTbl[0].strIdxFld[0]   = SQLVAR_PLUNUMBER;     // TOTAL table
			s_lstDb[iIndex].lstTbl[0].strIdxFld[1]   = SQLVAR_ADJINDEX;
			s_lstDb[iIndex].lstTbl[0].strIdxFld[2].Empty();
			s_lstDb[iIndex].lstTbl[1].strIdxFld[0]   = SQLVAR_VERSION;    // STATUS table
			s_lstDb[iIndex].lstTbl[1].strIdxFld[1].Empty();
			s_lstDb[iIndex].lstTbl[2].strIdxFld[0].Empty();                              // DELIMITER
		}

    }

    // create PluTotal Database objects
//    OpenDatabase(FALSE, TRUE, TRUE);		//causes hang on Windows 7 during dll init... is it even necessary?

    m_nCurrentDbFile = -1;
    s_nObjCnt++;

    // ### ADD (032400) init status buffer
    DATE_TIME       dtWrk;
	PifGetDateTime(&dtWrk);              /* get current date */
	PLUTOTAL_STATUS StatusZero = {0};
    SetPluTotalStatusVal (0L, &dtWrk, &dtWrk, 0L, 0L, &StatusZero);
    for(int cnt = 0;cnt < STSTBL_NUM;cnt++) {
        m_StatusBuf[cnt].nStsTblIdx = -1;   // CreateObject() initialize the cache table to begin
        m_StatusBuf[cnt].nStsUpdate = -1;	// 08/30/01 CreateObject() initialize the cache table to begin
		m_StatusBuf[cnt].Status = StatusZero;
    }
}


/*
*============================================================================
**Synopsis:     void    CnPluTotal::DestroyObject();
*
*    Input:     Nothing
*
*   Output:     Nothing
*
*    InOut:     Nothing
*
** Return:      Nothing
*               
** Description  delete DB(ADOCE) objects. (Unload ADOCE ActiveX control)
*
*============================================================================
*/
void    CnPluTotal::DestroyObject(){
    int idx = 0;
    while(!(s_lstDb[idx].strName.IsEmpty())){
        delete  s_lstDb[idx].pDb;
		s_lstDb[idx].pDb = NULL;
        idx++;
    }
    s_nObjCnt--;
}


BOOL    CnPluTotal::GetTblFormat(){

    // getting table format from ressources

    return TRUE;
}

#define  MAKEPARMS  1
BOOL CnPluTotal::MakeParmsPluTotals(COleSafeArray& saFields) {
    long    lIdx[1] = { 0 };    // bound

    saFields.CreateOneDim(VT_VARIANT, PLUTOTAL_REC_FLD_NUM, NULL, 0);
    lIdx[0] = 0;
    saFields.PutElement(lIdx, COleVariant(SQLVAR_PLUNUMBER, VT_BSTR));

    lIdx[0]++;
    saFields.PutElement(lIdx, COleVariant(SQLVAR_ADJINDEX, VT_BSTR));

    lIdx[0]++;
    saFields.PutElement(lIdx, COleVariant(SQLVAR_TOTAL, VT_BSTR));

    lIdx[0]++;
    saFields.PutElement(lIdx, COleVariant(SQLVAR_COUNTER, VT_BSTR));

#if defined(DCURRENCY_LONGLONG)
    lIdx[0]++;
    saFields.PutElement(lIdx, COleVariant(SQLVAR_TOTALVOID, VT_BSTR));

    lIdx[0]++;
    saFields.PutElement(lIdx, COleVariant(SQLVAR_COUNTERVOID, VT_BSTR));
#endif

    return TRUE;
}

BOOL CnPluTotal::MakeParmsPluStatus(COleSafeArray& saFields) {
    long    lIdx[1] = { 0 };    // bound

    saFields.CreateOneDim(VT_VARIANT, PLUTOTAL_STSREC_FLD_NUM, NULL, 0);

    lIdx[0] = 0;
    saFields.PutElement(lIdx, COleVariant(SQLVAR_RESETSTATUS, VT_BSTR));

    lIdx[0]++;
    saFields.PutElement(lIdx, COleVariant(SQLVAR_FROMDATE, VT_BSTR));

    lIdx[0]++;
    saFields.PutElement(lIdx, COleVariant(SQLVAR_TODATE, VT_BSTR));

    lIdx[0]++;
    saFields.PutElement(lIdx, COleVariant(SQLVAR_PLUALLTOTAL, VT_BSTR));

    lIdx[0]++;
    saFields.PutElement(lIdx, COleVariant(SQLVAR_PLUAMOUNT, VT_BSTR));

    lIdx[0]++;
    saFields.PutElement(lIdx, COleVariant(SQLVAR_VERSION, VT_BSTR));

    return TRUE;
}

BOOL    CnPluTotal::MakeParams(const SHORT nTblID,COleSafeArray &saFields){
//    COleSafeArray   vFldNames;      // field names

    COleVariant vWrk;
    long    lIdx[1];    // bound

    switch(nTblID){
        case PLUTOTAL_ID_DAILY_CUR  :
        case PLUTOTAL_ID_DAILY_SAV  :
        case PLUTOTAL_ID_PTD_CUR    :
        case PLUTOTAL_ID_PTD_SAV    :
        case PLUTOTAL_ID_TMP_CUR:       /* */
#if defined(MAKEPARMS)
            MakeParmsPluTotals(saFields);
#else
            // *** Total table
			saFields.CreateOneDim(VT_VARIANT, PLUTOTAL_REC_FLD_NUM, NULL,0);
            lIdx[0] = 0; vWrk.Clear(); vWrk.SetString(SQLVAR_PLUNUMBER,VT_BSTR);
            saFields.PutElement(lIdx,&vWrk);

            lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_ADJINDEX,VT_BSTR);
            saFields.PutElement(lIdx,&vWrk);

            lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_TOTAL,VT_BSTR);
            saFields.PutElement(lIdx,&vWrk);

            lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_COUNTER,VT_BSTR);
            saFields.PutElement(lIdx,&vWrk);

#if defined(DCURRENCY_LONGLONG)
            lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_TOTALVOID,VT_BSTR);
            saFields.PutElement(lIdx,&vWrk);

            lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_COUNTERVOID,VT_BSTR);
            saFields.PutElement(lIdx,&vWrk);
#endif
            vWrk.Clear();                               // V1.0.0.4
#endif
            break;
        case PLUTOTAL_ID_DAILY_CUR_S:
        case PLUTOTAL_ID_DAILY_SAV_S:
        case PLUTOTAL_ID_PTD_CUR_S  :
        case PLUTOTAL_ID_PTD_SAV_S  :
        case PLUTOTAL_ID_TMP_CUR_S:							/* */
#if defined(MAKEPARMS)
            MakeParmsPluStatus(saFields);
#else
            saFields.CreateOneDim(VT_VARIANT, 5,NULL,0); // ### ADD (03272000)

            // *** Status table
            lIdx[0] = 0; vWrk.Clear();
            vWrk.SetString(SQLVAR_RESETSTATUS,VT_BSTR);
            saFields.PutElement(lIdx,&vWrk);

            lIdx[0]++; vWrk.Clear();
            vWrk.SetString(SQLVAR_FROMDATE,VT_BSTR);
            saFields.PutElement(lIdx,&vWrk);

            lIdx[0]++; vWrk.Clear();
            vWrk.SetString(SQLVAR_TODATE,VT_BSTR);
            saFields.PutElement(lIdx,&vWrk);

            lIdx[0]++; vWrk.Clear();
            vWrk.SetString(SQLVAR_PLUALLTOTAL,VT_BSTR);
            saFields.PutElement(lIdx,&vWrk);

            lIdx[0]++; vWrk.Clear();
            vWrk.SetString(SQLVAR_VERSION,VT_BSTR);
            saFields.PutElement(lIdx,&vWrk);
            vWrk.Clear();                               // V1.0.0.4
#endif
            break;
        default:
            saFields.Clear();
            return FALSE;
    }
    // copy
    //vFields = (VARIANT)vFldNames;
    return  TRUE;
}


/*
*============================================================================
**Synopsis: BOOL    CnPluTotal::Var2PluTtlRec(const COleVariant vRecData,PLUTOTAL_REC * pRecData)
*           BOOL    CnPluTotal::Var2PluTtlRec(const COleVariant vRecData,PLUTOTAL_STATUS * pStsData){
*
*    Input:     vRecData            - record data (ADOCE)
*
*   Output:     pRecData            - 
*               pStsData            - 
*
*    InOut:     Nothing
*
** Return:      TRUE
*
** Description  COleVariant to struct PLUTOTAL_REC | PLUTOTAL_STATUS
*
*============================================================================
*/
BOOL    CnPluTotal::Var2PluTtlRec(const COleVariant vRecData,PLUTOTAL_REC * pRecData){

    if(vRecData.vt == VT_EMPTY)
        return  FALSE;

    COleSafeArray   saRec;
    saRec = vRecData;

    COleVariant vWrk;
    long    lIdx[2];    // bound
    lIdx[0] = 0; lIdx[1] = 0;

    // PluNumber
    vWrk.Clear();                               // V1.0.0.4
    CString strBuf;
    saRec.GetElement(lIdx,&vWrk);
    strBuf = vWrk;
    _tcsncpy(pRecData->auchPluNumber,strBuf,PLUTOTAL_PLUNO_LEN);

    // size
    pRecData->usSize = sizeof(PLUTOTAL_REC);

    // AdjectiveIndex
    vWrk.Clear();                               // V1.0.0.4
    lIdx[0]++;
    saRec.GetElement(lIdx,&vWrk);
    pRecData->sAdjIdx = V_I2((LPCVARIANT)vWrk); // short

    // Total
    vWrk.Clear();                               // V1.0.0.4
    lIdx[0]++; saRec.GetElement(lIdx,&vWrk);
    pRecData->lTotal = V_DCURRENCY((LPCVARIANT)vWrk);  // DCURRENCY
//    pRecData->lTotal = V_I4((LPCVARIANT)vWrk);  // integer

    // Counter
    vWrk.Clear();                               // V1.0.0.4
    lIdx[0]++; saRec.GetElement(lIdx,&vWrk);
    pRecData->lCounter = V_I4((LPCVARIANT)vWrk);    // integer

#if defined(DCURRENCY_LONGLONG)
    // Total
    vWrk.Clear();                               // V1.0.0.4
    lIdx[0]++; saRec.GetElement(lIdx,&vWrk);
    pRecData->lTotalVoid = V_DCURRENCY((LPCVARIANT)vWrk);  // DCURRENCY

    // Counter
    vWrk.Clear();                               // V1.0.0.4
    lIdx[0]++; saRec.GetElement(lIdx,&vWrk);
    pRecData->lCounterVoid = V_I4((LPCVARIANT)vWrk);    // integer
#endif

    vWrk.Clear();                               // V1.0.0.4
    saRec.Clear();                            // V1.0.0.4
//    saRec.Destroy();                            // V1.0.0.4

    return  TRUE;
}


BOOL    CnPluTotal::Var2PluTtlRec(const COleVariant vRecData,PLUTOTAL_STATUS * pStsData){
    if(vRecData.vt == VT_EMPTY)
        return  FALSE;

    COleSafeArray   saRec;
    saRec = vRecData;

    COleVariant vWrk;
    long    lIdx[] = {0,0,0};   // bound

    // structure size
    pStsData->usSize = sizeof(PLUTOTAL_STATUS);

    // ResetStatus
    vWrk.Clear();                               // V1.0.0.4
    saRec.GetElement(lIdx,&vWrk);
    pStsData->ulResetStatus = V_UI4((LPCVARIANT)(vWrk));

    // From date
    vWrk.Clear();                               // V1.0.0.4
    lIdx[0]++;
    saRec.GetElement(lIdx,&vWrk);
    Var2PluTtlDate(vWrk,&pStsData->dateFrom);

    // To date
    vWrk.Clear();                               // V1.0.0.4
    lIdx[0]++;
    saRec.GetElement(lIdx,&vWrk);
    Var2PluTtlDate(vWrk,&pStsData->dateTo);

    // Counter
    vWrk.Clear();                               // V1.0.0.4
    lIdx[0]++;
    saRec.GetElement(lIdx,&vWrk);
    pStsData->lAllTotal = V_DCURRENCY((LPCVARIANT)vWrk);    // DCURRENCY

    // Amount // ### ADD (041800)
    vWrk.Clear();                               // V1.0.0.4
    lIdx[0]++;
    saRec.GetElement(lIdx,&vWrk);
    pStsData->lAmount = V_DCURRENCY((LPCVARIANT)vWrk);      // DCURRENCY

    // Version
    vWrk.Clear();                               // V1.0.0.4
    lIdx[0]++;
    saRec.GetElement(lIdx,&vWrk);
    pStsData->lVersion = V_UI4((LPCVARIANT)vWrk);
    vWrk.Clear();                               // V1.0.0.4

	/***/
    saRec.Clear();                            // V1.0.0.4
//    saRec.Destroy();                            // V1.0.0.4

    return  TRUE;
}

/*
*============================================================================
**Synopsis: BOOL    CnPluTotal::Var2PluTtlDate(const COleVariant &vDate,PLUTOTAL_DATE *pDate);
*           BOOL    CnPluTotal::PluTtlDate2Var(const PLUTOTAL_DATE &pdDate,COleVariant  &vDate);
*
*    Input:     vDate               - variant date
*               pdDate              - Plutotal date
*
*   Output:     pDate
*               vDate
*
*    InOut:     Nothing
*
** Return:      TRUE
*
** Description  COleVariant <---> PLUTOTAL_DATE
*
*============================================================================
*/
BOOL    CnPluTotal::Var2PluTtlDate(const COleVariant &vDate,PLUTOTAL_DATE *pDate){
    if(vDate.vt == VT_EMPTY)
        return  FALSE;

    COleDateTime    dtDate(vDate);
    pDate->usMonth = (USHORT)dtDate.GetMonth(); // month
    pDate->usDay   = (USHORT)dtDate.GetDay();   // day
    pDate->usYear  = (USHORT)dtDate.GetYear();  // year

    pDate->usHour  = (USHORT)dtDate.GetHour();  // hour
    pDate->usMin   = (USHORT)dtDate.GetMinute();// minute
    pDate->usSec   = (USHORT)dtDate.GetSecond();// second

    return  TRUE;
}


BOOL    CnPluTotal::PluTtlDate2Var(const PLUTOTAL_DATE &pdDate,COleVariant  &vDate){
    COleDateTime    dtDate((int)pdDate.usYear,(int)pdDate.usMonth,(int)pdDate.usDay,
                        pdDate.usHour,pdDate.usMin,pdDate.usSec);
    // copy
    vDate = dtDate;
    return  TRUE;
}


/*
*============================================================================
**Synopsis: ULONG   CnPluTotal::OpenDatabase();
*           ULONG   CnPluTotal::CloseDatabase();
*
*    Input:     Nothing
*
*   Output:     Nothing
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS    : normal
*               PLUTOTAL_E_FAILURE  : 
*
** Description  Create/Delete database objects.
*
*============================================================================
*/
ULONG   CnPluTotal::OpenDatabase(const BOOL bCreateTbl/* = TRUE*/, SHORT fPTD, SHORT fTmp){
    ULONG   ulSts = PLUTOTAL_SUCCESS;

    // create database
    CString strDbPath;
    int idx = 0;
    while(!(s_lstDb[idx].strName.IsEmpty())){
        if(s_lstDb[idx].pDb == NULL){
            strDbPath = s_lstDb[idx].strPath;
            strDbPath += s_lstDb[idx].strName;
            s_lstDb[idx].pDb = new CnPluTotalDb(strDbPath);
        }
        if(bCreateTbl)
        {
			// do not create ptd file by option, 08/30/01
			if (fPTD == FALSE)
			{
		        if ((s_lstDb[idx].nId == PLUTOTAL_ID_PTD_CUR) || (s_lstDb[idx].nId == PLUTOTAL_ID_PTD_SAV))
		    	{
					idx++;
					continue;
				}
			}
			if (fTmp == FALSE)
			{
		        if (s_lstDb[idx].nId == PLUTOTAL_ID_TMP_CUR)
		    	{
					idx++;
					continue;
				}
			}
            ulSts = s_lstDb[idx].pDb->CreateDbFile();
        }
        //if(ulSts != ....
		if(ulSts == PLUTOTAL_DB_ERROR_MSDEDOWN)
		{
			return ulSts;
		}
        idx++;
    }
    s_lstDb[idx].pDb = NULL;

    return  ulSts;

}


ULONG   CnPluTotal::CloseDatabase(){
    ULONG   ulSts = PLUTOTAL_SUCCESS;

	// save status table data at database close, 08/18/01
	int nBufIdx;
	for (nBufIdx=0; nBufIdx<STSTBL_NUM; nBufIdx++)
	{
        if (m_StatusBuf[nBufIdx].nStsTblIdx != -1) {
	        if (m_StatusBuf[nBufIdx].nStsUpdate != -1) {	// just save when actual update has done, 08/30/01
			    ulSts = SaveStatus(m_StatusBuf[nBufIdx].nStsTblIdx, m_StatusBuf[nBufIdx].Status);
			}
		}
        m_StatusBuf[nBufIdx].nStsUpdate = -1;	// 08/30/01  CloseDatabase() indicate data has been saved.
	}

    // delete database objects
    int idx = 0;
    while(!(s_lstDb[idx].strName.IsEmpty())){
        if(s_lstDb[idx].pDb != NULL){
            delete  s_lstDb[idx].pDb;
            s_lstDb[idx].pDb = NULL;
        }
        idx++;
    }
    return  ulSts;
}


/*
*============================================================================
**Synopsis: ULONG   CnPluTotal::GetRecordCnt(const SHORT nTblID,LONG * plRecNum);
*
*    Input:     nTblID              - table index No.
*
*   Output:     pRecNum             - number of records
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS    : normal
*               PLUTOTAL_E_ILLEAGAL : illeagal argument(TableID) value
*
** Description  
*
*============================================================================
*/
ULONG   CnPluTotal::GetRecordCnt(const SHORT nTblID,LONG * plRecNum){
    *plRecNum = 0L;

    int     nDbCnt,nTblCnt;
    nDbCnt = SearchTbl(nTblID,&nTblCnt);
    if(nDbCnt >= 0 && s_lstDb[nDbCnt].pDb != NULL){
        return  s_lstDb[nDbCnt].pDb->get_RecordCnt(s_lstDb[nDbCnt].lstTbl[nTblCnt].strName,plRecNum);
    }
    return PLUTOTAL_E_ILLEAGAL;
}


/*
*============================================================================
**Synopsis: ULONG   CnPluTotal::CreateTable(const SHORT nTblID,const ULONG ulRecNum);
*           ULONG   CnPluTotal::DropTable(const SHORT nTblID);
*           ULONG   CnPluTotal::MoveTableN(const SHORT nFromTblID,const SHORT nToTblID);
*
*    Input:     nTblID              - table index No. 
*               ulRecNum            - max record num (reserved)
*
*   Output:     pRecNum             - number of records
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS    : normal
*               PLUTOTAL_E_ILLEAGAL : illeagal argument(TableID) value
*
** Description  CREATE/DROP/MOVE table.
*
*============================================================================
*/
ULONG   CnPluTotal::CreateTable(const SHORT nTblID,const ULONG ulRecNum){
    // get database objects
    SHORT   nStsTblID = GetStatusTblID(nTblID);
    CnPluTotalDb*   pDB    = _DataBaseObject(nTblID);
    CnPluTotalDb*   pStsDB = _DataBaseObject(nStsTblID);
    if(pDB == NULL || pStsDB == NULL)
        return PLUTOTAL_E_ILLEAGAL;

    // create TOTAL table
    ULONG   ulSts = pDB->CreateTable(_TableInfo(nTblID));
	{
		char  xBuff[128];
		sprintf(xBuff, "** ERROR: Create Total table nTblID %d ulSts %d result 0x%x", nTblID, ulSts, pDB->m_hr);
		NHPOS_ASSERT_TEXT((ulSts == PLUTOTAL_SUCCESS), xBuff);
	}
	m_pDB_hr = pDB->m_hr;
    if(ulSts != PLUTOTAL_SUCCESS)
        return  ulSts;

    // create Status Table
    ulSts = pStsDB->CreateTable(_TableInfo(nStsTblID));
	{
		char  xBuff[128];
		sprintf(xBuff, "** ERROR: Create Status table nTblID %d ulSts %d result 0x%x", nTblID, ulSts, pStsDB->m_hr);
		NHPOS_ASSERT_TEXT((ulSts == PLUTOTAL_SUCCESS), xBuff);
	}
	m_pStsDB_hr = pStsDB->m_hr;
    if(ulSts != PLUTOTAL_SUCCESS){
        pDB->DropTable(_TableName(nTblID));
        pStsDB->DropTable(_TableName(nStsTblID));
        return  ulSts;
    }
    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotal::DropTable(const SHORT nTblID){
    // get database objects
    SHORT   nStsTblID = GetStatusTblID(nTblID);
    CnPluTotalDb*   pDB    = _DataBaseObject(nTblID);
    CnPluTotalDb*   pStsDB = _DataBaseObject(nStsTblID);
    if(pDB == NULL || pStsDB == NULL)
        return PLUTOTAL_E_ILLEAGAL;

    // drop TOTAL & Status tables
    ULONG   ulSts = pDB->DropTable(_TableName(nTblID));
	m_pDB_hr = pDB->m_hr;

    pStsDB->DropTable(_TableName(nStsTblID));
	m_pStsDB_hr = pStsDB->m_hr;

    DelStatusBuf(nStsTblID);    // ### ADD (032400)

    return  ulSts;
}

ULONG   CnPluTotal::CreateIndex(const SHORT nTblID)
{
    // get database objects
    SHORT   nStsTblID = GetStatusTblID(nTblID);
    CnPluTotalDb*   pDB    = _DataBaseObject(nTblID);
    CnPluTotalDb*   pStsDB = _DataBaseObject(nStsTblID);
    if(pDB == NULL || pStsDB == NULL)
        return PLUTOTAL_E_ILLEAGAL;

    // drop TOTAL & Status tables
    ULONG   ulSts = pDB->CreateIndexEx(_TableInfo(nTblID));
    pStsDB->CreateIndexEx(_TableInfo(nStsTblID));

    return  ulSts;
}

ULONG   CnPluTotal::DropIndex(const SHORT nTblID)
{
    // get database objects
    SHORT   nStsTblID = GetStatusTblID(nTblID);
    CnPluTotalDb*   pDB    = _DataBaseObject(nTblID);
    CnPluTotalDb*   pStsDB = _DataBaseObject(nStsTblID);
    if(pDB == NULL || pStsDB == NULL)
        return PLUTOTAL_E_ILLEAGAL;

    // drop TOTAL & Status tables
    ULONG   ulSts = pDB->DropIndexEx(_TableInfo(nTblID));
    pStsDB->DropIndexEx(_TableInfo(nStsTblID));

    return  ulSts;
}

ULONG   CnPluTotal::MoveTableN(const SHORT nFromTblID,const SHORT nToTblID){
    CnPluTotalDb*   pFromDB = _DataBaseObject(nFromTblID);
    CnPluTotalDb*   pToDB   = _DataBaseObject(nToTblID);
    if(pFromDB == NULL || pToDB == NULL || (nFromTblID == nToTblID))
        return  PLUTOTAL_E_ILLEAGAL;

    ULONG   ulSts = PLUTOTAL_SUCCESS, ulSts2  = PLUTOTAL_SUCCESS;

	ulSts = ClearTable(nToTblID);
	if (ulSts != PLUTOTAL_SUCCESS) {
		return ulSts;
	}

/***
    // *** copy status
    PLUTOTAL_STATUS psSts;
    GetStsInfo(nFromTblID,&psSts);
    SetStsInfo(nToTblID,psSts, TRUE);
****/
    // select all records
    LONG    lRecCnt = 0L,lIdx[] = {0,0,0};
    CnPluTotal::SelectRec(nFromTblID,PLUTOTAL_SORT_PLUNO);
    pFromDB->get_RecordCnt(&lRecCnt);
    if(lRecCnt > 0){
        COleSafeArray   saFields;
        COleVariant     vWrk;
#if defined(MAKEPARMS)
        MakeParmsPluTotals(saFields);
#else
        saFields.CreateOneDim(VT_VARIANT, PLUTOTAL_REC_FLD_NUM, NULL, 0);
        lIdx[0] = 0; vWrk.Clear(); vWrk.SetString(SQLVAR_PLUNUMBER,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_ADJINDEX,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_TOTAL,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_COUNTER,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);

#if defined(DCURRENCY_LONGLONG)
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_TOTALVOID,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_COUNTERVOID,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
#endif
#endif
        TCHAR   strPluNo[PLUTOTAL_PLUNO_LEN+1];
        COleVariant     vValues;
        COleSafeArray   saValues;
        PLUTOTAL_REC    prRec;
        int nCount = 0;

        saValues.CreateOneDim(VT_VARIANT, PLUTOTAL_REC_FLD_NUM, NULL, 0);
        pToDB->OpenRec(_TableName(nToTblID));
        for(int nIdx = 0; nIdx < lRecCnt; nIdx++){
            vValues.Clear();
            ulSts = pFromDB->GetRow(saFields,&vValues); // read row

            Var2PluTtlRec(vValues,&prRec);
            //mbstowcs(strPluNo,(const char *)prRec.auchPluNumber,PLUTOTAL_PLUNO_LEN);
            _tcsncpy(strPluNo, prRec.auchPluNumber, PLUTOTAL_PLUNO_LEN);
			strPluNo[PLUTOTAL_PLUNO_LEN] = 0;
            lIdx[0] = 0 ; vWrk.Clear(); vWrk.SetString(strPluNo,VT_BSTR);
            saValues.PutElement(lIdx,&vWrk);
            lIdx[0]++ ; vWrk.Clear(); vWrk = prRec.sAdjIdx ;
            saValues.PutElement(lIdx,&vWrk);
            lIdx[0]++ ; vWrk.Clear(); vWrk = prRec.lTotal ;
            saValues.PutElement(lIdx,&vWrk);
            lIdx[0]++; vWrk.Clear(); vWrk = prRec.lCounter ;
            saValues.PutElement(lIdx,&vWrk);

#if defined(DCURRENCY_LONGLONG)
            lIdx[0]++ ; vWrk.Clear(); vWrk = prRec.lTotalVoid ;
            saValues.PutElement(lIdx,&vWrk);
            lIdx[0]++; vWrk.Clear(); vWrk = prRec.lCounterVoid ;
            saValues.PutElement(lIdx,&vWrk);
#endif

        	vWrk.Clear();                               // V1.0.0.4

            ulSts2 = pToDB->AddRec(saFields,saValues);  // write row
            if(ulSts == PLUTOTAL_EOF && ulSts2 == PLUTOTAL_SUCCESS){
                ulSts = PLUTOTAL_SUCCESS;
                break;
            }
            if(ulSts != PLUTOTAL_SUCCESS)
                break;
			/* avoid the error of AddRec at the continuous add record at CE3.0 */
            nCount++;
            if (nCount > 1000) {
        		pToDB->CloseRec();
        		pToDB->OpenRec(_TableName(nToTblID));
        		nCount = 0;
			}
            if(ulSts2 != PLUTOTAL_SUCCESS){
                ulSts = ulSts2;
                break;
            }
        }
        pToDB->CloseRec();

        vValues.Clear();
        saFields.Clear();                         // V1.0.0.4
        saValues.Clear();                         // V1.0.0.4
//        saFields.Destroy();                         // V1.0.0.4
//        saValues.Destroy();                         // V1.0.0.4
    }
    pFromDB->CloseRecoredset();

    // *** copy status, 08/30/01
    PLUTOTAL_STATUS psSts;

	// get the current date and initialize the status information for those
	// tables that are unused.  This prevents asserts from bad dates during debugging
	// processing unused tables such as period to date.
    DATE_TIME       dtWrk;
	PifGetDateTime(&dtWrk);
    SetPluTotalStatusVal (0L, &dtWrk, &dtWrk, 0L, 0L, &psSts);

    GetStsInfo(nFromTblID,&psSts);
    SetStsInfo(nToTblID,psSts, TRUE);

    if(ulSts == PLUTOTAL_SUCCESS && ulSts2 == PLUTOTAL_SUCCESS){
        ulSts = ClearTable(nFromTblID);
    }
    return  ulSts;
}


ULONG   CnPluTotal::ExportTableToFile (const SHORT nFromTblID, const TCHAR *pFileName)
{
    CnPluTotalDb*   pFromDB = _DataBaseObject(nFromTblID);
    if(pFromDB == NULL)
        return  PLUTOTAL_E_ILLEAGAL;
/***
    // *** copy status
    PLUTOTAL_STATUS psSts;
    GetStsInfo(nFromTblID,&psSts);
    SetStsInfo(nToTblID,psSts, TRUE);
****/
    // select all records
    ULONG   ulSts = PLUTOTAL_SUCCESS, ulSts2 = PLUTOTAL_SUCCESS;
    LONG    lRecCnt = 0L,lIdx[] = {0,0,0};

    CnPluTotal::SelectRec(nFromTblID, PLUTOTAL_SORT_PLUNO);
    pFromDB->get_RecordCnt (&lRecCnt);

	SHORT  hsPluTtlExportHandle;

	hsPluTtlExportHandle = PifOpenFile (pFileName, "owrf");
	if (hsPluTtlExportHandle < 0) {
		hsPluTtlExportHandle = PifOpenFile (pFileName, "nwrf");
	}

	NHPOS_ASSERT (hsPluTtlExportHandle >= 0);

    if(lRecCnt > 0 && hsPluTtlExportHandle >= 0) {
        COleSafeArray   saFields;
        COleVariant     vWrk;
#if defined(MAKEPARMS)
        MakeParmsPluTotals(saFields);
#else
        saFields.CreateOneDim(VT_VARIANT, PLUTOTAL_REC_FLD_NUM, NULL, 0);
        lIdx[0] = 0; vWrk.Clear(); vWrk.SetString(SQLVAR_PLUNUMBER,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_ADJINDEX,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_TOTAL,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_COUNTER,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);

#if defined(DCURRENCY_LONGLONG)
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_TOTALVOID,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_COUNTERVOID,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
#endif
#endif

        COleVariant     vValues;
        COleSafeArray   saValues;
        PLUTOTAL_REC    prRec;
        int             nCount = 0;
		int             nIdx;

		// the second argument, dwElements, must be the same number as the actual
		// number of items inserted into the arry with calls to PutElement().
		// otherwise run the risk of function GetRow() return an error code of 0x800A0CC1.
        saValues.CreateOneDim(VT_VARIANT, PLUTOTAL_REC_FLD_NUM, NULL, 0);

        for (nIdx = 0; nIdx < lRecCnt; nIdx++){
            vValues.Clear();
            ulSts = pFromDB->GetRow(saFields,&vValues); // read row

            Var2PluTtlRec(vValues,&prRec);

            if(ulSts != PLUTOTAL_SUCCESS && ulSts != PLUTOTAL_EOF) {
				NHPOS_ASSERT_TEXT(0, "CnPluTotal::ExportTableToFile(): (ulSts != PLUTOTAL_SUCCESS)");
                break;
			}

			PifWriteFile (hsPluTtlExportHandle, &prRec, sizeof(prRec));
			
			ulSts = PLUTOTAL_SUCCESS;
        }

        vValues.Clear();
        saFields.Clear();                         // V1.0.0.4
        saValues.Clear();                         // V1.0.0.4

		PifCloseFile (hsPluTtlExportHandle);
    }

    pFromDB->CloseRecoredset();

    // *** copy status, 08/30/01
    PLUTOTAL_STATUS psSts;
    GetStsInfo(nFromTblID,&psSts);

    return  ulSts;
}



ULONG   CnPluTotal::ClearTable(const SHORT nTblID)
{
    ULONG   ulSts = CnPluTotal::DropTable(nTblID);
	if (ulSts != PLUTOTAL_SUCCESS) {
		char  xBuff[128];
		sprintf (xBuff, "==WARNING: CnPluTotal::DropTable() ulSts %d, m_pDB_hr 0x%x, m_pStsDB_hr 0x%x", ulSts, m_pDB_hr, m_pStsDB_hr);
		NHPOS_ASSERT_TEXT(ulSts == PLUTOTAL_SUCCESS, xBuff);
	}
    ulSts = CnPluTotal::CreateTable(nTblID,10000L);
	if (ulSts != PLUTOTAL_SUCCESS) {
		char  xBuff[128];
		sprintf (xBuff, "==WARNING: CnPluTotal::CreateTable() ulSts %d, m_pDB_hr 0x%x, m_pStsDB_hr 0x%x", ulSts, m_pDB_hr, m_pStsDB_hr);
		NHPOS_ASSERT_TEXT(ulSts == PLUTOTAL_SUCCESS, xBuff);
	}

	return ulSts;
}


ULONG   CnPluTotal::CheckTable(const SHORT nTblID){
    CnPluTotalDb*   pDB = _DataBaseObject(nTblID);
    if(pDB == NULL)
        return PLUTOTAL_E_ILLEAGAL;

    return  pDB->CheckTable(_TableName(nTblID));
}

ULONG   CnPluTotal::dbg_ExecSQL(const SHORT nTblId,LPCTSTR szSqlCode){
    int     nDbCnt,nTblCnt;

    nDbCnt = SearchTbl(nTblId,&nTblCnt);
    if(nDbCnt >= 0 && s_lstDb[nDbCnt].pDb != NULL){
        return  s_lstDb[nDbCnt].pDb->ExecSQL(szSqlCode);
    }
    return PLUTOTAL_E_ILLEAGAL;
}


// ******* New Functions ***********
#ifdef  ___SQL_VERSION
ULONG   CnPluTotal::InsertN(const SHORT nTblID,const PLUTOTAL_REC &RecData){
    // *** SQL code
    CString     strSqlCode;
    TCHAR       szPluNo[PLUTOTAL_PLUNO_LEN+1];
    //mbstowcs(szPluNo,(const char *)RecData.auchPluNumber,PLUTOTAL_PLUNO_LEN);
    szPluNo[PLUTOTAL_PLUNO_LEN] = _T('\0');
    strSqlCode.Format(TEXT("INSERT INTO %s VALUES(%s,%d,%ld,%ld)"),_TableName(nTblID),
        szPluNo,RecData.sAdjIdx,RecData.lTotal,RecData.lCounter);

    CnPluTotalDb*   pDB = _DataBaseObject(nTblID);
    if(pDB == NULL)
        return  PLUTOTAL_E_ILLEAGAL;
    LONG    lRecCnt;
    ULONG   ulSts = pDB->OpenRec((LPCTSTR)strSqlCode,TRUE,&lRecCnt);
    return  ulSts;
}
#endif


ULONG   CnPluTotal::InsertN(const SHORT nTblID,const PLUTOTAL_REC &RecData){
    CnPluTotalDb*   pDB = _DataBaseObject(nTblID);
    if(pDB == NULL)
        return  PLUTOTAL_E_ILLEAGAL;
    ULONG ulSts = pDB->OpenRec(_TableName(nTblID));
    if(ulSts == PLUTOTAL_SUCCESS){
        COleVariant     vWrk;
        long    lIdx[] = {0,0,0};

        // field names
        COleSafeArray   saFields;
#if defined(MAKEPARMS)
        MakeParmsPluTotals(saFields);
#else
        saFields.CreateOneDim(VT_VARIANT, PLUTOTAL_REC_FLD_NUM, NULL, 0);

        lIdx[0] = 0; vWrk.Clear(); vWrk.SetString(SQLVAR_PLUNUMBER,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_ADJINDEX,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_TOTAL,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_COUNTER,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);

#if defined(DCURRENCY_LONGLONG)
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_TOTALVOID,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_COUNTERVOID,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
#endif
#endif

        // field values
        COleSafeArray   saValues;
        saValues.CreateOneDim(VT_VARIANT, PLUTOTAL_REC_FLD_NUM, NULL, 0);

        TCHAR   strPluNo[PLUTOTAL_PLUNO_LEN+1];
        //mbstowcs(strPluNo,(const char *)RecData.auchPluNumber,PLUTOTAL_PLUNO_LEN);
        _tcsncpy(strPluNo, RecData.auchPluNumber, PLUTOTAL_PLUNO_LEN);
		strPluNo[PLUTOTAL_PLUNO_LEN] = _T('\0');

        lIdx[0] = 0 ; vWrk.Clear(); vWrk.SetString((LPCTSTR)strPluNo,VT_BSTR);
        saValues.PutElement(lIdx,&vWrk);
        lIdx[0]++ ; vWrk.Clear(); vWrk = RecData.sAdjIdx ;
        saValues.PutElement(lIdx,&vWrk);
        lIdx[0]++ ; vWrk.Clear(); vWrk = RecData.lTotal ;
        saValues.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk = RecData.lCounter ;
        saValues.PutElement(lIdx,&vWrk);

#if defined(DCURRENCY_LONGLONG)
        lIdx[0]++ ; vWrk.Clear(); vWrk = RecData.lTotalVoid ;
        saValues.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk = RecData.lCounterVoid ;
        saValues.PutElement(lIdx,&vWrk);
#endif

        vWrk.Clear();                               // V1.0.0.4

        ulSts = pDB->AddRec(saFields,saValues);
        pDB->CloseRec();
        saFields.Clear();                         // V1.0.0.4
        saValues.Clear();                         // V1.0.0.4
//        saFields.Destroy();                         // V1.0.0.4
//        saValues.Destroy();                         // V1.0.0.4
    }
    PutLog(TEXT("### InsertN   "),nTblID,RecData,ulSts);
    return  ulSts;
}


ULONG   CnPluTotal::UpdateN(const SHORT nTblID,const PLUTOTAL_REC &RecData){
    CnPluTotalDb*   pDB = _DataBaseObject(nTblID);
    if(pDB == NULL)
        return  PLUTOTAL_E_ILLEAGAL;

    // *** SQL code
    LONG    lRecCnt;
    ULONG ulSts = CnPluTotal::Find(nTblID,RecData.auchPluNumber,RecData.sAdjIdx,FALSE,&lRecCnt);
    if(ulSts == PLUTOTAL_SUCCESS && lRecCnt == 0){
        // Insert???
        // ### MAKING InsertN();
        return  PLUTOTAL_E_ILLEAGAL;
    }
    else if(ulSts == PLUTOTAL_SUCCESS && 0 < lRecCnt){
        COleVariant     vWrk;
        long    lIdx[] = {0,0,0};

        COleSafeArray   saFields;
        saFields.CreateOneDim(VT_VARIANT, PLUTOTAL_REC_FLD_NUM - 2, NULL, 0);    
    //  lIdx[0] = 0; vWrk.Clear(); vWrk.SetString(SQLVAR_PLUNUMBER,VT_BSTR);
    //  saFields.PutElement(lIdx,&vWrk);

    //  lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_ADJINDEX,VT_BSTR);
    //  saFields.PutElement(lIdx,&vWrk);

        // field names
        lIdx[0] = 0; vWrk.Clear(); vWrk.SetString(SQLVAR_TOTAL,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_COUNTER,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);

#if defined(DCURRENCY_LONGLONG)
		lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_TOTALVOID,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_COUNTERVOID,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
#endif

        // field values
        COleSafeArray   saValues;
        saValues.CreateOneDim(VT_VARIANT, PLUTOTAL_REC_FLD_NUM - 2, NULL, 0);
        lIdx[0] = 0; vWrk.Clear(); vWrk = RecData.lTotal ;
        saValues.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk = RecData.lCounter ;
        saValues.PutElement(lIdx,&vWrk);

#if defined(DCURRENCY_LONGLONG)
        lIdx[0]++; vWrk.Clear(); vWrk = RecData.lTotalVoid ;
        saValues.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk = RecData.lCounterVoid ;
        saValues.PutElement(lIdx,&vWrk);
#endif
        vWrk.Clear();                               // V1.0.0.4

        ulSts = pDB->PutRec(saFields,saValues);

        saFields.Clear();                         // V1.0.0.4
		/****/
        saValues.Clear();
//        saFields.Destroy();                         // V1.0.0.4
		/****/
//        saValues.Destroy();
    }
    pDB->CloseRec();
    PutLog(TEXT("### UpdateN   "),nTblID,RecData,ulSts);
    return  ulSts;
}

// Status table
ULONG   CnPluTotal::Find(const SHORT nTblID,const ULONG ulFindVal){
    // *** SQL code
    CString     strSqlCode;
    strSqlCode.Format(TEXT("SELECT * FROM %s WHERE Version = %ld "),_TableName(nTblID),ulFindVal);

    CnPluTotalDb*   pDB = _DataBaseObject(nTblID);
    if(pDB == NULL)
        return  PLUTOTAL_E_ILLEAGAL;

    long    lRecNum;
    return  pDB->OpenRec(strSqlCode,FALSE,&lRecNum);
}

// total
ULONG   CnPluTotal::Find(const SHORT nTblID,const TCHAR pPluNo[],const SHORT sAdjIdx,const BOOL bClose,long * plRecNum){
    // *** SQL code
    CString     strSqlCode;
    TCHAR       szPluNo[PLUTOTAL_PLUNO_LEN+1];
    //mbstowcs(szPluNo,(const char *)pPluNo,PLUTOTAL_PLUNO_LEN);
	_tcsncpy(szPluNo, pPluNo, PLUTOTAL_PLUNO_LEN);
    szPluNo[PLUTOTAL_PLUNO_LEN] = _T('\0');
//### MOD (042500) BUG  strSqlCode.Format(TEXT("SELECT * FROM %s WHERE PluNumber = %s "),_TableName(nTblID),szPluNo);
    strSqlCode.Format(TEXT("SELECT * FROM %s WHERE PluNumber = '%s' and AdjectiveIndex = %d "),_TableName(nTblID),szPluNo,sAdjIdx);

    CnPluTotalDb*   pDB = _DataBaseObject(nTblID);
    if(pDB == NULL)
        return  PLUTOTAL_E_ILLEAGAL;
    return  pDB->OpenRec(strSqlCode,bClose,plRecNum);
}

ULONG   CnPluTotal::FindN(const SHORT nTblID,const TCHAR pPluNo[],const BYTE byAdjIdx,PLUTOTAL_REC * pRecData){
    CnPluTotalDb*   pDB = _DataBaseObject(nTblID);
    if(pDB == NULL)
        return  PLUTOTAL_E_ILLEAGAL;

    // open record
    COleVariant     vValues;
    LONG    lRecCnt;

    ULONG   ulSts = CnPluTotal::Find(nTblID,pPluNo,byAdjIdx,FALSE,&lRecCnt);

    if (ulSts == PLUTOTAL_SUCCESS && lRecCnt == 0) {
        ulSts = PLUTOTAL_NOT_FOUND;
    }
    else if (ulSts == PLUTOTAL_SUCCESS && 0 < lRecCnt) {
        // *** GetRow
        COleVariant     vWrk;
        long    lIdx[] = {0,0,0};

        COleSafeArray   saFields;
#if defined(MAKEPARMS)
        MakeParmsPluTotals(saFields);
#else
        saFields.CreateOneDim(VT_VARIANT, PLUTOTAL_REC_FLD_NUM, NULL, 0);

        lIdx[0] = 0; vWrk.Clear(); vWrk.SetString(SQLVAR_PLUNUMBER,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_ADJINDEX,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_TOTAL,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_COUNTER,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);

#if defined(DCURRENCY_LONGLONG)
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_TOTALVOID,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_COUNTERVOID,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
#endif
#endif

        vWrk.Clear();                               // V1.0.0.4

        ulSts = pDB->GetRec(saFields,(LPVARIANT)vValues);
        saFields.Clear();                         // V1.0.0.4
//        saFields.Destroy();                         // V1.0.0.4
    }
    pDB->CloseRec();
    if(ulSts != PLUTOTAL_SUCCESS) {
        return  ulSts;
    }

    // *** variant -> PLUTOTAL_REC
    if(Var2PluTtlRec(vValues,pRecData)){
        vValues.Clear();                            // V1.0.0.4
        return  PLUTOTAL_SUCCESS;
    }
    else {
        vValues.Clear();                            // V1.0.0.4
        return  PLUTOTAL_E_FAILURE;
    }
}


ULONG   CnPluTotal::DeleteN(const SHORT nTblID,const TCHAR pPluNo[],const BYTE byAdjIdx){
    CnPluTotalDb*   pDB = _DataBaseObject(nTblID);
    if(pDB == NULL)
        return  PLUTOTAL_E_ILLEAGAL;

    // open record
    // COleVariant     vValues;                 // removed V1.0.0.4
    LONG    lRecCnt;
    ULONG ulSts = CnPluTotal::Find(nTblID,pPluNo,byAdjIdx,FALSE,&lRecCnt);
    if(ulSts == PLUTOTAL_SUCCESS && lRecCnt == 0){
        ulSts = PLUTOTAL_NOT_FOUND;
    }
    else if(ulSts == PLUTOTAL_SUCCESS && 0 < lRecCnt){
        ulSts = pDB->DelRec();
    }
    pDB->CloseRec();
    return  ulSts;
}


ULONG   CnPluTotal::SelectRec(const SHORT nTblID,const ULONG SearchCond){
    CString strSqlCode;

    PDBObject   pDB = _DataBaseObject(nTblID);
    if(pDB == NULL)
        return  PLUTOTAL_E_ILLEAGAL;

    switch(SearchCond){
        case PLUTOTAL_SORT_TOTAL:
            strSqlCode.Format(_T("SELECT * FROM %s ORDER BY Total "),_TableName(nTblID));
            break;
        case PLUTOTAL_SORT_PLUNO:
            strSqlCode.Format(_T("SELECT * FROM %s ORDER BY PluNumber, AdjectiveIndex "),_TableName(nTblID));
            break;
        case PLUTOTAL_SEARCH_CNT:
            strSqlCode.Format(_T("SELECT * FROM %s WHERE Counter > 0 ORDER BY PluNumber, AdjectiveIndex "),_TableName(nTblID));
            break;
        default:
            return  PLUTOTAL_E_ILLEAGAL;
    }

    return  pDB->OpenRecoredset(strSqlCode);
}


ULONG   CnPluTotal::FirstRec(const SHORT nTblID){
    PDBObject   pDB = _DataBaseObject(nTblID);

    if(pDB == NULL)
        return PLUTOTAL_E_ILLEAGAL;

    return  pDB->MoveFirst();
}


ULONG   CnPluTotal::LastRec(const SHORT nTblID){
    PDBObject   pDB = _DataBaseObject(nTblID);

    if(pDB == NULL)
        return PLUTOTAL_E_ILLEAGAL;

    return  pDB->MoveLast();
}


ULONG   CnPluTotal::NextRec(const SHORT nTblID){
    PDBObject   pDB = _DataBaseObject(nTblID);

    if(pDB == NULL)
        return PLUTOTAL_E_ILLEAGAL;

    return  pDB->MoveNext();
}


ULONG   CnPluTotal::PrevRec(const SHORT nTblID){
    PDBObject   pDB = _DataBaseObject(nTblID);

    if(pDB == NULL)
        return PLUTOTAL_E_ILLEAGAL;

    return  pDB->MovePrev();
}


ULONG   CnPluTotal::GetRec(const SHORT nTblID,PLUTOTAL_REC * pRecData){
    PDBObject   pDB = _DataBaseObject(nTblID);
    if(pDB == NULL)
        return PLUTOTAL_E_ILLEAGAL;

    ULONG   ulSts;
    COleVariant     vValues;
    COleSafeArray   saFields;
    MakeParams(nTblID,saFields);
    COleVariant     vFields((LPCVARIANT)saFields);

    ulSts = pDB->GetRow(vFields,&vValues);
    if(ulSts == PLUTOTAL_SUCCESS || ulSts == PLUTOTAL_EOF){
        // variant -> PLUTOTAL_REC
        // if(! Var2PluTtlRec(vValues,pRecData))    // V1.0.0.4
        //    return  PLUTOTAL_E_FAILURE;           // V1.0.0.4
        if(! Var2PluTtlRec(vValues,pRecData)) {     // V1.0.0.4
            ulSts = PLUTOTAL_E_FAILURE;             // V1.0.0.4
        }                                           // V1.0.0.4
    }
    vValues.Clear();                                // V1.0.0.4
    saFields.Clear();                             // V1.0.0.4
//    saFields.Destroy();                             // V1.0.0.4
    return  ulSts;
}


ULONG   CnPluTotal::GetNumRec(const SHORT nTblID,ULONG * pulRecNum){
    PDBObject   pDB = _DataBaseObject(nTblID);

    if(pDB == NULL)
        return PLUTOTAL_E_ILLEAGAL;

    return  pDB->get_RecordCnt((LONG *)pulRecNum);
}


ULONG   CnPluTotal::ReleaseRec(const SHORT nTblID){
    PDBObject   pDB = _DataBaseObject(nTblID);

    if(pDB == NULL)
        return PLUTOTAL_E_ILLEAGAL;

    return  pDB->CloseRecoredset();
}

ULONG   CnPluTotal::RestoreDB(const SHORT nTblID){
	SHORT nStsTblID = GetStatusTblID(nTblID);
	CnPluTotalDb*   pDB    = _DataBaseObject(nTblID);
	CString     strSqlCode, strFileName;
	ULONG		ulSts;

	if(nStsTblID < 0)
	{
		NHPOS_ASSERT_TEXT(0,"nStsTblID < 0");
		return PLUTOTAL_E_ILLEAGAL;
	}

	strFileName = _DbFileName(nTblID);

	ulSts = pDB->RestoreDB(strFileName);

	return ulSts;


}

ULONG   CnPluTotal::GetStsInfo(const SHORT nTblID,PLUTOTAL_STATUS * pStatusRec){
    SHORT   nStsTblID = GetStatusTblID(nTblID);
    if(nStsTblID < 0)
        return  PLUTOTAL_E_ILLEAGAL;

    int nBufIdx = _StsBufIdx(nStsTblID);
    if(0 <= nBufIdx){
        *pStatusRec = m_StatusBuf[nBufIdx].Status;  // copy data from cache

		int nBufIdx2;
		ULONG ulSts;
		//SR 807, we now update the status everytime we want to get the status information
		//this allows us to have it saved at another point than just at application shutdown. JHHJ
		for (nBufIdx2=0; nBufIdx2<STSTBL_NUM; nBufIdx2++)
		{
			if (m_StatusBuf[nBufIdx2].nStsTblIdx != -1) {
				if (m_StatusBuf[nBufIdx2].nStsUpdate != -1) {	// just save when actual update has done, 08/30/01
					ulSts = SaveStatus(m_StatusBuf[nBufIdx2].nStsTblIdx, m_StatusBuf[nBufIdx2].Status);
					if (ulSts == PLUTOTAL_SUCCESS) {
						m_StatusBuf[nBufIdx2].nStsUpdate = -1;      // indicate that we have saved the data.
					}
				}
			}
		}
        return  PLUTOTAL_SUCCESS;
    }

    // *** notfound in buffer
    nBufIdx = _StsBufIdx(-1); // search empty buffer
    ULONG   ulSts;
    if(0 <= nBufIdx){
        ulSts = LoadStatus(nStsTblID,&m_StatusBuf[nBufIdx].Status);
        if(ulSts == PLUTOTAL_SUCCESS){
            // store in buffer
            m_StatusBuf[nBufIdx].nStsTblIdx = nStsTblID;
			m_StatusBuf[nBufIdx].nStsUpdate = -1;        // indicate that we have saved the data.
            *pStatusRec = m_StatusBuf[nBufIdx].Status;
        }
    }
    else
        return  PLUTOTAL_E_FAILURE;
    return  ulSts;
}


ULONG   CnPluTotal::LoadStatus(const SHORT nStsTblID,PLUTOTAL_STATUS * pStatusRec){
    PDBObject   pDB = _DataBaseObject(nStsTblID);
    if(pDB == NULL)
        return PLUTOTAL_E_ILLEAGAL;

    // search
    ULONG   ulSts = CnPluTotal::Find(nStsTblID,(ULONG)PLUTOTAL_DB_VERSION);
    if(ulSts != PLUTOTAL_SUCCESS){
        pDB->CloseRec();
        return  PLUTOTAL_E_FAILURE;
    }

    COleSafeArray   saFields;
#if defined(MAKEPARMS)
    MakeParmsPluStatus(saFields);
#else
    COleVariant     vWrk;
    long    lIdx[] = {0,0,0};
    // field names
    saFields.CreateOneDim(VT_VARIANT, PLUTOTAL_STSREC_FLD_NUM,NULL,0);
    lIdx[0] = 0; vWrk.Clear(); vWrk.SetString(SQLVAR_RESETSTATUS,VT_BSTR);
    saFields.PutElement(lIdx,&vWrk);
    lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_FROMDATE,VT_BSTR);
    saFields.PutElement(lIdx,&vWrk);
    lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_TODATE,VT_BSTR);
    saFields.PutElement(lIdx,&vWrk);
    lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_PLUALLTOTAL,VT_BSTR);
    saFields.PutElement(lIdx,&vWrk);
    lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_PLUAMOUNT,VT_BSTR); // ### ADD (041800)
    saFields.PutElement(lIdx,&vWrk);
    lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_VERSION,VT_BSTR);
    saFields.PutElement(lIdx,&vWrk);
    vWrk.Clear();                           // V1.0.0.4
#endif

    // get values
    COleVariant vValues;
    ulSts = pDB->GetRec(saFields,&vValues);             // read row
    saFields.Clear();                     // V1.0.0.4
//    saFields.Destroy();                     // V1.0.0.4
    pDB->CloseRec();
    if(ulSts != PLUTOTAL_SUCCESS)
        return  ulSts;

    // variant -> PLUTOTAL_REC
    if(Var2PluTtlRec(vValues,pStatusRec)) {
        vValues.Clear();                    // V1.0.0.4
        return  PLUTOTAL_SUCCESS;
    }
    else {
        vValues.Clear();                    // V1.0.0.4
        return PLUTOTAL_E_FAILURE;
    }
}


ULONG   CnPluTotal::SetStsInfo(const SHORT nTblID,const PLUTOTAL_STATUS StatusRec, BOOL bSave){
    SHORT   nStsTblID = GetStatusTblID(nTblID);
    if(nStsTblID < 0)
        return PLUTOTAL_E_ILLEAGAL;

    int nBufIdx = _StsBufIdx(nStsTblID);
    if(nBufIdx < 0)
        nBufIdx = _StsBufIdx(-1); // search empty buffer
    if(nBufIdx < 0)
        return PLUTOTAL_E_FAILURE;

    // *** save status
    // save status only move table, 08/18/01
	ULONG   ulSts = PLUTOTAL_SUCCESS;
	if ((nStsTblID == PLUTOTAL_ID_DAILY_SAV_S) || (nStsTblID == PLUTOTAL_ID_PTD_SAV_S) ||
		(bSave == TRUE)) {
	    ULONG   ulSts = SaveStatus (nStsTblID, StatusRec);
        m_StatusBuf[nBufIdx].nStsUpdate = -1;			// 08/30/01  Indicate we have saved the data.
		bSave = TRUE;
	}
    if(ulSts == PLUTOTAL_SUCCESS){
        m_StatusBuf[nBufIdx].nStsTblIdx = nStsTblID;
		if (bSave != TRUE) {
			m_StatusBuf[nBufIdx].nStsUpdate = nStsTblID;	// 08/30/01  Indicate we have non-saved data
		}
        m_StatusBuf[nBufIdx].Status = StatusRec;
    } else
    {
        m_StatusBuf[nBufIdx].nStsTblIdx = -1;
        m_StatusBuf[nBufIdx].nStsUpdate = -1;			// 08/30/01
    }
    return  ulSts;
}


ULONG   CnPluTotal::SaveStatus(const SHORT nStsTblID,const PLUTOTAL_STATUS &StatusRec){
    PDBObject   pDB = _DataBaseObject(nStsTblID);
    if(pDB == NULL)
        return PLUTOTAL_E_ILLEAGAL;

    COleSafeArray   saFields;
    COleVariant     vWrk;
    long    lIdx[] = {0,0,0};
#if defined(MAKEPARMS)
    MakeParmsPluStatus(saFields);
#else
    // field names
    saFields.CreateOneDim(VT_VARIANT, PLUTOTAL_STSREC_FLD_NUM,NULL,0);  
    lIdx[0] = 0; vWrk.Clear(); vWrk.SetString(SQLVAR_RESETSTATUS,VT_BSTR);
    saFields.PutElement(lIdx,&vWrk);
    lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_FROMDATE,VT_BSTR);
    saFields.PutElement(lIdx,&vWrk);
    lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_TODATE,VT_BSTR);
    saFields.PutElement(lIdx,&vWrk);
    lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_PLUALLTOTAL,VT_BSTR);
    saFields.PutElement(lIdx,&vWrk);
    lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_PLUAMOUNT,VT_BSTR);
    saFields.PutElement(lIdx,&vWrk);
    lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_VERSION,VT_BSTR);
    saFields.PutElement(lIdx,&vWrk);
#endif

    // field values
    COleSafeArray   saValues;
    saValues.CreateOneDim(VT_VARIANT, PLUTOTAL_STSREC_FLD_NUM,NULL,0);
    lIdx[0] = 0; vWrk.Clear(); vWrk = (const long)StatusRec.ulResetStatus ;
    saValues.PutElement(lIdx,&vWrk);
    lIdx[0]++; vWrk.Clear(); PluTtlDate2Var(StatusRec.dateFrom,vWrk);
    saValues.PutElement(lIdx,&vWrk);
    lIdx[0]++; vWrk.Clear(); PluTtlDate2Var(StatusRec.dateTo,vWrk);
    saValues.PutElement(lIdx,&vWrk);
    lIdx[0]++; vWrk.Clear(); vWrk = StatusRec.lAllTotal ;
    saValues.PutElement(lIdx,&vWrk);
    lIdx[0]++; vWrk.Clear(); vWrk = StatusRec.lAmount ;
    saValues.PutElement(lIdx,&vWrk);
    lIdx[0]++; vWrk.Clear(); vWrk = (const long)PLUTOTAL_DB_VERSION ;
    saValues.PutElement(lIdx,&vWrk);
    vWrk.Clear();                           // V1.0.0.4

    ULONG   ulSts;
    LONG    lRecCnt;
    pDB->get_RecordCnt(_TableName(nStsTblID),&lRecCnt);
    if(lRecCnt == 0){
        // Insert
        ulSts = pDB->OpenRec(_TableName(nStsTblID));
        if(ulSts == PLUTOTAL_SUCCESS){
            ulSts = pDB->AddRec(saFields,saValues);
        }
    }
    else if (lRecCnt < 0){
		// ignore, 09/07/01
		PutLog(TEXT("@@@ SET STATUS, unknown"),nStsTblID,StatusRec,lRecCnt);
	}
    else{
        // Update
        ulSts = CnPluTotal::Find(nStsTblID,(ULONG)PLUTOTAL_DB_VERSION);
        if(ulSts == PLUTOTAL_SUCCESS){
            ulSts = pDB->PutRec(saFields,saValues);
        }
    }
    saFields.Clear();                     // V1.0.0.4
    saValues.Clear();                     // V1.0.0.4
//    saFields.Destroy();                     // V1.0.0.4
//    saValues.Destroy();                     // V1.0.0.4
    pDB->CloseRec();
    PutLog(TEXT("@@@ SET STATUS"),nStsTblID,StatusRec,ulSts);
    return  ulSts;
}

// This function should only be called when dropping tables or deleting the database
BOOL    CnPluTotal::DelStatusBuf(const SHORT nStsTblID){
    int nBufIdx = _StsBufIdx(nStsTblID);
    if(nBufIdx < 0)
        return  FALSE;

    m_StatusBuf[nBufIdx].nStsTblIdx = -1;   // DelStatusBuf() clear the cache 
    m_StatusBuf[nBufIdx].nStsUpdate = -1;	// 08/30/01  DelStatusBuf() clear the cache 
    return  TRUE;
}


ULONG   CnPluTotal::OpenDBFile(const SHORT nTblID,const ULONG ulOption){
	ULONG ulStatus;
    if(m_nCurrentDbFile != -1)
        return PLUTOTAL_E_FAILURE;

	CnPluTotalDb*   pDB    = _DataBaseObject(nTblID);

    // database file name
/* ### DEL
    LPCTSTR lpFname = _FileName(nTblID);
    if(lpFname == NULL)
        return PLUTOTAL_E_ILLEAGAL;
    CString strDbFileName(m_strDbPath);
    strDbFileName += lpFname;
### */
    CString strDbFileName = _DbPathName(nTblID);
#ifdef XP_PORT
	strDbFileName += _T("_BAK.dat");
#endif

	// 12/10/01
	/* clear status table buffer for AC42 backup copy */
    if((ulOption & PLUTOTAL_DBFILE_CREATEREAD)
		|| (ulOption & PLUTOTAL_DBFILE_CREATEWRITE)){
	    SHORT   nStsTblID = GetStatusTblID(nTblID);
	    //DelStatusBuf(nStsTblID);    // ### ADD (032400)
	}

    // options
    UINT    uMode = 0L;
    if(ulOption & PLUTOTAL_DBFILE_CREATEWRITE){
		ulStatus = pDB->CreateDbBackUpFile();
        uMode |= CFile::modeWrite;
	}
    if(ulOption & PLUTOTAL_DBFILE_CREATEREAD){
#ifdef XP_PORT
		ulStatus = pDB->CreateDbBackUpFile();
#endif
		uMode |= CFile::modeRead;
    }
    uMode |= CFile::typeBinary;
    uMode |= CFile::shareExclusive;

    // open dbfile
	//m_fDbFile.SetFilePath(strDbFileName);
    if(m_fDbFile.Open(strDbFileName,uMode,&m_feErrorInfo)){
        PutLog(TEXT("$$$ CnPluTotalDb::OpenDbFile"),(ULONG)0,(ULONG)PLUTOTAL_SUCCESS,NULL);
        m_nCurrentDbFile = nTblID;
        return  PLUTOTAL_SUCCESS;
    }else{
		DWORD error;

		error = GetLastError();
        PutLog(TEXT("$$$ CnPluTotalDb::OpenDbFile"),(ULONG)0,(ULONG)PLUTOTAL_E_FAILURE,&m_feErrorInfo);
        return  PLUTOTAL_E_FAILURE;
    }
}


ULONG   CnPluTotal::CloseDBFile(){
    if(m_fDbFile.m_hFile != CFile::hFileNull)
        m_fDbFile.Close();
    m_nCurrentDbFile = -1;

    PutLog(TEXT("$$$ CnPluTotalDb::CloseDbFile"),(ULONG)0,(ULONG)PLUTOTAL_SUCCESS,NULL);
    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotal::ReadDbFile(const ULONG ulOffset, const ULONG ulBytes,VOID *pBuf,ULONG *pulReadBytes){

    ULONGLONG dwLength;

	// set seek function for recovery, 02/15/01
    PutLog(TEXT("$$$ CnPluTotalDb::ReadDbFile Seek0"),ulOffset,PLUTOTAL_SUCCESS,NULL);
    TRY{
        dwLength = m_fDbFile.Seek(0,CFile::end);
        PutLog(TEXT("$$$ CnPluTotalDb::ReadDbFile Seek1"),dwLength,PLUTOTAL_SUCCESS,NULL);
    }
    CATCH( CFileException, e ){
        TCHAR   msg[200];
        e->GetErrorMessage(msg,199,NULL);
        ::OutputDebugString(msg);
    }END_CATCH
    if (ulOffset > dwLength) {
        PutLog(TEXT("$$$ CnPluTotalDb::ReadDbFile"),(ULONG)0,(ULONG)PLUTOTAL_EOF);
        return  PLUTOTAL_EOF;
	}
    TRY{
        dwLength = m_fDbFile.Seek(ulOffset,CFile::begin);
        PutLog(TEXT("$$$ CnPluTotalDb::ReadDbFile Seek"),dwLength,(ULONG)PLUTOTAL_SUCCESS,NULL);
    }
    CATCH( CFileException, e ){
        TCHAR   msg[200];
        e->GetErrorMessage(msg,199,NULL);
        ::OutputDebugString(msg);
    }END_CATCH

		
    TRY{
        // read
        *pulReadBytes = m_fDbFile.Read(pBuf,(UINT)ulBytes);
        /* ### DBUEG (20000407)
        { 
            TCHAR   tcBuf[500],tcBuf2[500];
            memset(tcBuf,0,500*sizeof(TCHAR));
            mbstowcs(tcBuf,(const char *)pBuf,*pulReadBytes);
            wsprintf(tcBuf2,TEXT("--->[%s]"),tcBuf);
            ::OutputDebugString(tcBuf2);
        }*/
    }
    CATCH( CFileException, ex ){
        // error
        TCHAR   msg[200];
        ex->GetErrorMessage(msg,199,NULL);
        ::OutputDebugString(msg);
    }END_CATCH

    // EOF?
    if(ulBytes != *pulReadBytes){
        PutLog(TEXT("$$$ CnPluTotalDb::ReadDbFile"),*pulReadBytes,(ULONG)PLUTOTAL_EOF,NULL);
        return  PLUTOTAL_EOF;
    }
    PutLog(TEXT("$$$ CnPluTotalDb::ReadDbFile"),*pulReadBytes,(ULONG)PLUTOTAL_SUCCESS,NULL);
    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotal::WriteDbFile(const ULONG ulOffset, const ULONG ulBytes,const VOID *pBuf){

    ULONGLONG dwLength;

	// set seek function for recovery, 02/15/01
    PutLog(TEXT("$$$ CnPluTotalDb::WriteDbFile Seek0"),ulOffset,(ULONG)PLUTOTAL_SUCCESS,NULL);
    TRY{
        dwLength = m_fDbFile.Seek((UINT)0L,CFile::end);
        PutLog(TEXT("$$$ CnPluTotalDb::WriteDbFile Seek1"),dwLength,(ULONG)PLUTOTAL_SUCCESS,NULL);
    }
    CATCH( CFileException, e ){
        TCHAR   msg[200];
        e->GetErrorMessage(msg,199,NULL);
        ::OutputDebugString(msg);
    }END_CATCH
    if (dwLength > ulOffset) {
		TRY{
			m_fDbFile.SetLength(ulOffset);
		PutLog(TEXT("$$$ CnPluTotalDb::WriteDbFile SetLength"),ulOffset,(ULONG)PLUTOTAL_SUCCESS,NULL);
		}
		CATCH( CFileException, e ){
			TCHAR   msg[200];
			e->GetErrorMessage(msg,199,NULL);
			::OutputDebugString(msg);
		}END_CATCH
	}
    TRY{
        dwLength = m_fDbFile.Seek((UINT)ulOffset,CFile::begin);
    }
    CATCH( CFileException, e ){
        TCHAR   msg[200];
        e->GetErrorMessage(msg,199,NULL);
        ::OutputDebugString(msg);
    }END_CATCH


    TRY{
        // write
        m_fDbFile.Write(pBuf,(UINT)ulBytes);
        /* ### DBUEG (20000407)
        {
            TCHAR   tcBuf[500],tcBuf2[500];
            memset(tcBuf,0,500*sizeof(TCHAR));
            mbstowcs(tcBuf,(const char *)pBuf,ulBytes);
            wsprintf(tcBuf2,TEXT("--->[%s]"),tcBuf);
            ::OutputDebugString(tcBuf2);
        }*/
    }
    CATCH( CFileException, ex ){
        // error
        TCHAR   msg[200];
        ex->GetErrorMessage(msg,200,NULL);
        ::OutputDebugString(msg);
    }END_CATCH

    PutLog(TEXT("$$$ CnPluTotalDb::WriteDbFile"),ulBytes,(ULONG)PLUTOTAL_SUCCESS,NULL);
    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotal::DeleteDbFile(const SHORT nTblID){
#ifndef _WIN32_WCE_EMULATION
    // close file
    CnPluTotal::CloseDBFile();

    // database file name
/* ### DEL
    LPCTSTR lpFname = _FileName(nTblID);
    if(lpFname == NULL)
        return PLUTOTAL_E_ILLEAGAL;
    CString strDbFileName(m_strDbPath);
    strDbFileName += lpFname;
### */
    CString strDbFileName = _DbPathName(nTblID);

    if(!::DeleteFile(strDbFileName))
        return  PLUTOTAL_E_FAILURE;
#endif
	// 08/30/01
    SHORT   nStsTblID = GetStatusTblID(nTblID);
    DelStatusBuf(nStsTblID);    // ### ADD (032400)
        
    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotal::MoveDbFile(const SHORT nFromTblID,const SHORT nToTblID){
#ifndef _WIN32_WCE_EMULATION
    // close file
    CnPluTotal::CloseDBFile();

    CString strFromDbFileName = _DbPathName(nFromTblID);
    CString strToDbFileName   = _DbPathName(nToTblID);

    if(!::MoveFile(strFromDbFileName, strToDbFileName))
        return  PLUTOTAL_E_FAILURE;
#endif
    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotal::CopyDbFile(const SHORT nFromTblID,const SHORT nToTblID){
#ifndef _WIN32_WCE_EMULATION
    // close file
    CnPluTotal::CloseDBFile();

    CString strFromDbFileName = _DbPathName(nFromTblID);
    CString strToDbFileName   = _DbPathName(nToTblID);

    if(!::CopyFile(strFromDbFileName, strToDbFileName, FALSE))
        return  PLUTOTAL_E_FAILURE;
#endif
    // ### MAKING (04112000)
    // ::CopyFile(src,dst,FALSE);
    return  PLUTOTAL_SUCCESS;
}


// ### ADD Saratoga (053100)
ULONG   CnPluTotal::StartBackup(void){
    return  m_BackUp.StartCopy();
}

// ### ADD Saratoga (060700)
ULONG   CnPluTotal::StartRestore(void){
    return  m_BackUp.StartCopy(FALSE);
}

// ### ADD Saratoga (053100)
ULONG   CnPluTotal::GetBackupStatus(void){
    return  m_BackUp.GetStatus();
}


void    CnPluTotal::PutLog(LPCTSTR szLabel,const SHORT nTblID,const PLUTOTAL_REC &RecData,const ULONG ulSts){
#ifdef  _CNPLUTOTAL_TRACE
    TCHAR   tBuf[512],tblnm[30];
    wsprintf(tBuf,TEXT("{ (%s) - (%d)  %06ld  %06ld } (%ld)"),(LPCTSTR)(CnString((const char *)RecData.auchPluNumber,13)),
        RecData.sAdjIdx,RecData.lTotal,RecData.lCounter,ulSts);
    wsprintf(tblnm,TEXT(" [%s] "),_TableName(nTblID));
    dbgtrc.PutLog(szLabel,tblnm,tBuf);
#endif
}

void    CnPluTotal::PutLog(LPCTSTR szLabel,const SHORT nTblID,const char aucPluNumber,const ULONG ulSts){
#ifdef  _CNPLUTOTAL_TRACE
    TCHAR   tBuf[512],tblnm[30];
    wsprintf(tBuf,TEXT("{ (%s) } (%ld)"),(LPCTSTR)CnString((const char *)aucPluNumber,13),ulSts);
    wsprintf(tblnm,TEXT(" [%s] "),_TableName(nTblID));
    dbgtrc.PutLog(szLabel,tblnm,tBuf);
#endif
}

void    CnPluTotal::PutLog(LPCTSTR szLabel,const SHORT nTblID,const PLUTOTAL_STATUS StatusRec,const ULONG ulSts){
#ifdef  _CNPLUTOTAL_TRACE
    TCHAR   tBuf[512],tblnm[30];
    wsprintf(tBuf,TEXT("{ [%04X]  %02d/%02d/%04d - %02d/%02d/%04d   %ld  (%ld)(%ld) } (%ld)"),StatusRec.ulResetStatus,
        StatusRec.dateFrom.usMonth,StatusRec.dateFrom.usDay,StatusRec.dateFrom.usYear,
        StatusRec.dateTo.usMonth,StatusRec.dateTo.usDay,StatusRec.dateTo.usYear,
        StatusRec.lAllTotal,StatusRec.lAmount,StatusRec.lVersion,ulSts );
    wsprintf(tblnm,TEXT(" [%s] "),_TableName(nTblID));
    dbgtrc.PutLog(szLabel,tblnm,tBuf);
#endif
}

void    CnPluTotal::PutLog(LPCTSTR szLabel,const ULONG ulBytes,const ULONG ulSts,CFileException * pEx){
#ifdef  _CNPLUTOTAL_DBFILE_TRACE
    TCHAR   tBuf[512],fname[256],tErrMsg[255];
    if(pEx != NULL){
        memset(tErrMsg,0,sizeof(tErrMsg));
        pEx->GetErrorMessage(tErrMsg,255);
        wsprintf(tBuf,TEXT("{ %ld bytes } (%ld)\n---> %s"),ulBytes,ulSts,tErrMsg);
    }else
        wsprintf(tBuf,TEXT("{ %ld bytes } (%ld)"),ulBytes,ulSts);
    wsprintf(fname,TEXT(" [%s] "),(LPCTSTR)(m_fDbFile.GetFilePath()) );
    dbgtrc.PutLog(szLabel,fname,tBuf);
#endif
}

void    CnPluTotal::PutLog(LPCTSTR szLabel, const ULONGLONG ullBytes, const ULONG ulSts, CFileException* pEx) {
#ifdef  _CNPLUTOTAL_DBFILE_TRACE
    TCHAR   tBuf[512], fname[256], tErrMsg[255];
    if (pEx != NULL) {
        memset(tErrMsg, 0, sizeof(tErrMsg));
        pEx->GetErrorMessage(tErrMsg, 255);
        wsprintf(tBuf, TEXT("{ %ld bytes } (%ld)\n---> %s"), ulBytes, ulSts, tErrMsg);
    }
    else
        wsprintf(tBuf, TEXT("{ %ld bytes } (%ld)"), ulBytes, ulSts);
    wsprintf(fname, TEXT(" [%s] "), (LPCTSTR)(m_fDbFile.GetFilePath()));
    dbgtrc.PutLog(szLabel, fname, tBuf);
#endif
}

int CnPluTotal::SearchTbl(const SHORT sTblID,int *nTblIdx){
    int idx = 0;
    while(!(s_lstDb[idx].strName.IsEmpty())){
        int idx2 = 0;
        while(!(s_lstDb[idx].lstTbl[idx2].strName.IsEmpty())){
            if(s_lstDb[idx].lstTbl[idx2].nId == sTblID){
                *nTblIdx = idx2;
                return  idx;
            }
            idx2++;
        }
        idx++;
    }
	idx = (-1);
	NHPOS_ASSERT_TEXT((idx >= 0), "CnPluTotal::SearchTbl(): sTblID not found.");
    return -1;
}


SHORT   CnPluTotal::GetStatusTblID(const SHORT nTblID)const{
    switch(nTblID){
        case PLUTOTAL_ID_DAILY_CUR  :
        case PLUTOTAL_ID_DAILY_CUR_S:
            return  PLUTOTAL_ID_DAILY_CUR_S;
        case PLUTOTAL_ID_DAILY_SAV  :
        case PLUTOTAL_ID_DAILY_SAV_S:
            return  PLUTOTAL_ID_DAILY_SAV_S;
        case PLUTOTAL_ID_PTD_CUR    :
        case PLUTOTAL_ID_PTD_CUR_S  :
            return  PLUTOTAL_ID_PTD_CUR_S;
        case PLUTOTAL_ID_PTD_SAV    :
        case PLUTOTAL_ID_PTD_SAV_S  :
            return  PLUTOTAL_ID_PTD_SAV_S;
        case PLUTOTAL_ID_TMP_CUR  :
        case PLUTOTAL_ID_TMP_CUR_S:
            return  PLUTOTAL_ID_TMP_CUR_S;
        default:
			NHPOS_ASSERT_TEXT(0, "CnPluTotal::GetStatusTblID(): Bad nTblID.");
            return -1;
    }
}


/*** private ****************************************************************/

LPCTSTR CnPluTotal::_TableName(const SHORT nTblID){
    int     nDbCnt,nTblCnt;
    nDbCnt = CnPluTotal::SearchTbl(nTblID,&nTblCnt);
    if(nDbCnt >= 0)
        return  s_lstDb[nDbCnt].lstTbl[nTblCnt].strName;
    else
        return  NULL;
    // try -- catch ???
}


CString CnPluTotal::_DbPathName(const SHORT nTblID){
    CString strDbName;
#ifdef XP_PORT
	strDbName += PLUTOTAL_DB_BACKUP_PATH;
#endif

    int     nDbCnt,nTblCnt;
    nDbCnt = CnPluTotal::SearchTbl(nTblID,&nTblCnt);
    if(nDbCnt >= 0){
#ifndef XP_PORT
        strDbName = (LPCTSTR)s_lstDb[nDbCnt].strPath;
#endif
        strDbName += s_lstDb[nDbCnt].strName;
    }
    else{
        strDbName.Empty();
    }
    return  strDbName;
}

CString CnPluTotal::_DbFileName(const SHORT nTblID){
    CString strDbName;
    int     nDbCnt,nTblCnt;
    nDbCnt = CnPluTotal::SearchTbl(nTblID,&nTblCnt);
    if(nDbCnt >= 0){
        //strDbName = (LPCTSTR)s_lstDb[nDbCnt].strPath;
        strDbName += s_lstDb[nDbCnt].strName;
    }
    else{
        strDbName.Empty();
    }
    return  strDbName;
}


/* ### DEL
LPCTSTR CnPluTotal::_FileName(const SHORT nTblID){
    int     nDbCnt,nTblCnt;
    nDbCnt = CnPluTotal::SearchTbl(nTblID,&nTblCnt);
    if(nDbCnt >= 0)
        return  (LPCTSTR)s_lstDb[nDbCnt].strName;
    else
        return  (LPCTSTR)NULL;
}
### */

CnPluTotalDb*   CnPluTotal::_DataBaseObject(const SHORT nTblID) {
    int     nDbCnt,nTblCnt;
    nDbCnt = CnPluTotal::SearchTbl(nTblID,&nTblCnt);
    if(nDbCnt >= 0)
        return  s_lstDb[nDbCnt].pDb;
    else
        return  NULL;
}

__CnTblFormat&  CnPluTotal::_TableInfo(const SHORT nTblID) {
    int     nDbCnt,nTblCnt;
    nDbCnt = CnPluTotal::SearchTbl(nTblID,&nTblCnt);
    return  s_lstDb[nDbCnt].lstTbl[nTblCnt];
}


int     CnPluTotal::_StsBufIdx(const SHORT nStsTblID)const{
    for(int cnt = 0;cnt < STSTBL_NUM;cnt++){
        if(m_StatusBuf[cnt].nStsTblIdx == nStsTblID)
            return  cnt;
    }
    return  -1;
}



/*** DEBUG & TESTS **********************************************************/
void    CnPluTotal::__GenPluNo(const LONG lNo,LPTSTR pPluNo){
    char    aBuf[20];
    TCHAR   tBuf[20];
    wsprintf(tBuf,TEXT("000000%06ld"),lNo);
    wcstombs(aBuf,tBuf,12);

    SHORT    i,sEven,sOdd,cdigit;
    for (sEven = sOdd = i = 0 ; i < 12 ; ++i) {
        if (i % 2) {
            sOdd  += (SHORT)(aBuf[i]&0x0f);        /* odd addr, Saratoga */
        } else {
            sEven += (SHORT)(aBuf[i]&0x0f);        /* even addr, Saratoga */
        }
    }
    cdigit = (SHORT)((sEven + sOdd*3) % 10);        /* Saratoga */
    cdigit = (SHORT)((10 - cdigit) % 10);           /* Saratoga */
    aBuf[12] = (UCHAR)(cdigit | 0x30);              /* return CD about plu# */

    ::mbstowcs(pPluNo,aBuf,13);
}


ULONG   CnPluTotal::__DbgMakeTestData(const SHORT nTblID,const LONG lFrom,const LONG lTo,const LONG lStep){
    // check args
    if( lFrom <= 0 ||  lFrom >= lTo || lStep >= (lTo - lFrom) || lStep <= 0)
        return  PLUTOTAL_E_ILLEAGAL;

    ClearTable(nTblID);

    CnPluTotalDb*   pDB = _DataBaseObject(nTblID);
    if(pDB == NULL)
        return  PLUTOTAL_E_ILLEAGAL;
    ULONG ulSts = pDB->OpenRec(_TableName(nTblID));
    if(ulSts == PLUTOTAL_SUCCESS){
        COleVariant     vWrk;
        long    lIdx[] = {0,0,0};

        // field names
        COleSafeArray   saFields;
#if defined(MAKEPARMS)
        MakeParmsPluTotals(saFields);
#else
        saFields.CreateOneDim(VT_VARIANT, PLUTOTAL_REC_FLD_NUM, NULL, 0);
        lIdx[0] = 0; vWrk.Clear(); vWrk.SetString(SQLVAR_PLUNUMBER,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_ADJINDEX,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_TOTAL,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_COUNTER,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);

#if defined(DCURRENCY_LONGLONG)
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_TOTALVOID,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk.SetString(SQLVAR_COUNTERVOID,VT_BSTR);
        saFields.PutElement(lIdx,&vWrk);
#endif
#endif

        // field values
        COleSafeArray   saValues;
        saValues.CreateOneDim(VT_VARIANT, PLUTOTAL_REC_FLD_NUM, NULL, 0);

        lIdx[0] = 0 ; vWrk.Clear(); vWrk.SetString(TEXT("0000000000000"),VT_BSTR);
        saValues.PutElement(lIdx,&vWrk);
        lIdx[0]++ ; vWrk.Clear(); vWrk = (const short)1 ;
        saValues.PutElement(lIdx,&vWrk);
        lIdx[0]++ ; vWrk.Clear(); vWrk = (const DCURRENCY)1 ;
        saValues.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk = (const long)1 ;
        saValues.PutElement(lIdx,&vWrk);

#if defined(DCURRENCY_LONGLONG)
        lIdx[0]++ ; vWrk.Clear(); vWrk = (const DCURRENCY)0 ;
        saValues.PutElement(lIdx,&vWrk);
        lIdx[0]++; vWrk.Clear(); vWrk = (const long)0 ;
        saValues.PutElement(lIdx,&vWrk);
#endif

        TCHAR   szPluNo[20];
        szPluNo[13] = 0;
#ifdef DEBUG_PLUTOTAL_OUTPUT
        TCHAR   szDMsg[256];
#endif
        LONG    lRecCnt = 0;
        for(long cnt = lFrom ; cnt <= lTo ; cnt += lStep){
            //wsprintf(szPluNo, TEXT("0000000%06ld"),cnt );
            __GenPluNo(cnt,szPluNo);

            lIdx[0] = 0 ; vWrk.Clear(); vWrk.SetString(szPluNo,VT_BSTR);
            saValues.PutElement(lIdx,&vWrk);

            ulSts = pDB->AddRec(saFields,saValues);
            if(ulSts != PLUTOTAL_SUCCESS)
                break;
#ifdef DEBUG_PLUTOTAL_OUTPUT
            if((cnt % (lStep * 100)) == 0){
                wsprintf(szDMsg,TEXT("*** %s [%ld]\n"),_TableName(nTblID),cnt);
                ::OutputDebugString(szDMsg);
            }
#endif
            lRecCnt++;
        }
        vWrk.Clear();                           // V1.0.0.4
        saFields.Clear();                     // V1.0.0.4
        saValues.Clear();                     // V1.0.0.4
//        saFields.Destroy();                     // V1.0.0.4
//        saValues.Destroy();                     // V1.0.0.4
        if(ulSts == PLUTOTAL_SUCCESS){
            PLUTOTAL_STATUS psSts;
            psSts.ulResetStatus = 0;
            psSts.dateFrom.usYear = 2000;
            psSts.dateFrom.usMonth = 4;
            psSts.dateFrom.usDay = 1;
            psSts.dateFrom.usHour = 0;
            psSts.dateFrom.usMin = 0;
            psSts.dateFrom.usSec = 0;
            psSts.dateTo.usYear = 2000;
            psSts.dateTo.usMonth = 4;
            psSts.dateTo.usDay = 1;
            psSts.dateTo.usHour = 0;
            psSts.dateTo.usMin = 0;
            psSts.dateTo.usSec = 0;
            psSts.lAllTotal = lRecCnt;
            psSts.lVersion = 100;
            SetStsInfo(nTblID,psSts, TRUE);
        }
        pDB->CloseRec();
    }
    return  ulSts;
}


/****************************************************************************/
//#ifdef  __DEL_061600
// ******* OLD Functions ***********
