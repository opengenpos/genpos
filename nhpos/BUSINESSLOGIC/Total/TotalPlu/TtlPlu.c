/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTALPLU, TotalPlu Function Library  Subroutine             
* Category    : TOTAL, NCR 2172 US Saratoga Application                     
* Program Name: TtlPlu.c                                                    
* --------------------------------------------------------------------------
* Abstract:        
*   SHORT TtlTTotalCheckIndFin()  - Check Reset Report Enable Status of Individual Financial R3.1
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name       : Description
* 12/24/99 : 01.00.00 : K.Iwata      : 2172 Rel1.0(Saratoga) initial
* 08/19/15 : 02.02.01 : R.Chambers   : changes to TtlPLUOptimize() to reduce sleep time, add logs.
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/
/* #define LOGGING 1 */
#include	<tchar.h>
#include <memory.h>
#include <string.h>
#if defined(LOGGING)
#include <winbase.h>
#endif

#include <ecr.h>
#include <pif.h>
#include <plu.h>
#include <csttl.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <csstbpar.h>
#include <regstrct.h>
#include <transact.h>
#include <ttl.h>
#include <rfl.h>

#include "ttltum.h"
#include "PluTtlD.h"
#include "stdio.h"
#include "appllog.h"

/*** global vals ***/
PLUTOTAL_DB_HANDLE      g_hdDB;             /* database handle          */
PLUTOTAL_FILE_HANDLE    g_hdDBFile;         /* database file handle     */

SHORT   g_nCurrentTblId = 0;
USHORT  g_nGetRecCount = 0;

TCHAR FARCONST szTtlPluPtdCurFileName[] = PLUTOTAL_DBNM_PTD_CUR;   /* Base Total File Name */
TCHAR FARCONST szTtlPluPtdSavFileName[] = PLUTOTAL_DBNM_PTD_SAV;   /* Base Total File Name */

VOID    _InitBackUp(VOID);

VOID    _SetPluTotalStatusVal(ULONG ulResetSts,DATE_TIME *dtFrom,DATE_TIME *dtTo,LONG lAllTotal,LONG lAmount,PLUTOTAL_STATUS *ppsRec);
SHORT   _TtlPluClass2TblID(const UCHAR uchMajorClass,const UCHAR uchMinorClass);
VOID    _PltTtlRec2TtlPlu(const PLUTOTAL_REC src,TTLPLU *pdst);
VOID    _PltTtlSts2TtlPlu(const PLUTOTAL_STATUS src,TTLPLU *pdst); /* ### ADD (042600) */
BOOL    _CheckTable(const SHORT nTblId);
VOID    _SetCurrentDateStsRec(PLUTOTAL_STATUS * ppsRec,const SHORT nMode);
ULONG   _CreateStatusRec(const SHORT nTblID);
SHORT   _GetNextBackupTable(const SHORT nBackupTblIDCur);

VOID    _DATE_TIME2PLUTOTAL_DATE(const DATE_TIME dtDate,PLUTOTAL_DATE *pPD);
VOID    _PLUTOTAL_DATE2DATE_TIME(const PLUTOTAL_DATE pdDate,DATE_TIME *pDt);

#if defined(LOGGING)
SHORT   _CreatePluTotal( SHORT nTblID, ULONG ulPLUNum );
#endif

/*
===========================================================================
* ### ADD 2172 Rel1.0 (01/14/00)
**Synopsis:     SHORT TtlPLUOpenDB(VOID)
*
*    Input:     Nothing
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Initialize PLU Total File 
*               TTL_FAIL            - database error
*
** Description  This function Open the PLUTOTAL database.
*
*		Memory Leak 7.8.05
*			We will only call this function once. That is during the 
*			initialization of the total function library 'TtlInitial()'
*			The call to PluTotalOpen in this function will result in 
*			the application consuming memory that is never returned
*			with the TtlPLUCloseDB() call. Until the PluTotalAPI is 
*			restructured to not consume memory that is not released,
*			this function should only be called once.
*============================================================================
*/
SHORT   TtlPLUOpenDB(VOID){
    SHORT   nSts = PLUTOTAL_SUCCESS;
	SHORT   sPTD = RflGetPtdFlagByType(FLEX_PLU_ADR);  /* create ptd file by option, 08/30/01 */
	ULONG ulSts;

    PifRequestSem(usTtlSemRW);

    /*  */
    if(g_hdDBFile != PLUTOTAL_HANDLE_NULL)
		//Memory Leak 7.11.05
		//We do not need to close the File, because we only close it now
		//once at the closing of the application
//        PluTotalCloseFile(g_hdDBFile);
    g_hdDBFile = PLUTOTAL_HANDLE_NULL;

    /* close the total database */
    if(g_hdDB != PLUTOTAL_HANDLE_NULL){
        PluTotalClose(g_hdDB);
        g_hdDB = PLUTOTAL_HANDLE_NULL;
    }

	
    /* open the total database */
	ulSts = PluTotalOpen(&g_hdDB, sPTD, FALSE);
    if( ulSts != PLUTOTAL_SUCCESS)
	{
		switch(ulSts)
		{
		case PLUTOTAL_DB_ERROR_MSDEDOWN:
            NHPOS_ASSERT_TEXT(0, "**ERROR**: TtlPLUOpenDB() PLUTOTAL_DB_ERROR_MSDEDOWN - FAULT_ERROR_MSDE_OPEN_DB. SQL Server or SQLite ODBC driver.");
            nSts = PLUTOTAL_DB_ERROR_MSDEDOWN;
			break;
		default:
			nSts = TTL_FAIL;
			break;
		}
	}

    PifReleaseSem(usTtlSemRW);
    return  nSts;
}

SHORT   TtlPLUCloseDB(VOID){
    SHORT   nSts = PLUTOTAL_SUCCESS;

    PifRequestSem(usTtlSemRW);

	//Memory Leak 7.11.05
	//We do not need to close the File, because we only close it now
	//once at the closing of the application
/*  if(g_hdDBFile != PLUTOTAL_HANDLE_NULL){
//        PluTotalCloseFile(g_hdDBFile);
        g_hdDBFile = PLUTOTAL_HANDLE_NULL;
    }*/

    /* open the total database */
    PluTotalClose(g_hdDB);

    g_hdDB = PLUTOTAL_HANDLE_NULL;

    PifReleaseSem(usTtlSemRW);
    return  nSts;
}
/*
===========================================================================
**Synopsis:     SHORT TtlPLUInit(VOID *TmpBuff)
*
*    Input:     VOID *pTmpBuff      - Pointer of Work Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Initialize PLU Total File 
*               TTL_FAIL            - database error
*
** Description  This function Initializes the PLU Total File.
*
*============================================================================
*/

SHORT   TtlPLUInit(VOID *pTmpBuff){
    ULONG           ulRecNum;
    BOOL            blPtdFlg;
    DATE_TIME       dtWrk;
    PLUTOTAL_STATUS psRec;
	SHORT   sPTD = RflGetPtdFlagByType(FLEX_PLU_ADR);  /* create ptd file by option, 08/30/01 */
	char aszErrorBuffer[128];

	NHPOS_NONASSERT_NOTE("==STATE", "==STATE:  TtlPLUInit() started.");

    /*  */
    _InitBackUp();

    if(g_hdDB != PLUTOTAL_HANDLE_NULL){
        PluTotalClose(g_hdDB);
        g_hdDB = PLUTOTAL_HANDLE_NULL;
    }

	if(PluTotalOpen(&g_hdDB, sPTD, FALSE) != PLUTOTAL_SUCCESS) {
		NHPOS_NONASSERT_NOTE("==WARNING", "==WARNING:  TtlPLUInit() - PluTotalOpen() failed.");
        return  TTL_FAIL;
	}

    /*** PLU Total File Initialize ***/
    PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_DAILY_CUR);
    if(PluTotalCreateTbl(g_hdDB,(SHORT)PLUTOTAL_ID_DAILY_CUR,PLUTOTAL_REC_MAX) != PLUTOTAL_SUCCESS) {
		sprintf(aszErrorBuffer,"TtlPLUInit() calling PluTotalCreateTbl()  PLUTOTAL_ID_DAILY_CUR");
		NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
        goto    TTLPLUINI_ERR;
	}

    /*** check & initialize the PTD table ***/
    if(PluTotalGetRecCnt(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_CUR,&ulRecNum) == PLUTOTAL_SUCCESS){
        blPtdFlg = TRUE;
        PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_CUR);
        if(PluTotalCreateTbl(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_CUR,PLUTOTAL_REC_MAX) != PLUTOTAL_SUCCESS) {
			sprintf(aszErrorBuffer,"TtlPLUInit() calling PluTotalCreateTbl()  PLUTOTAL_ID_PTD_CUR");
			NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
            goto    TTLPLUINI_ERR;
		}
    }
    else{
        /* no PTD */
        blPtdFlg = FALSE;
    }

    /*** set From date ***/
    /*** Current/Daily get/set status info ***/
    PifGetDateTime(&dtWrk); /* get date */
    _SetPluTotalStatusVal(0L,&dtWrk,NULL,0L,0L,&psRec);
    if(PluTotalSetStsInfo(g_hdDB,PLUTOTAL_ID_DAILY_CUR_S,psRec) != PLUTOTAL_SUCCESS) {
		sprintf(aszErrorBuffer,"TtlPLUInit() calling PluTotalSetStsInfo()  PLUTOTAL_ID_DAILY_CUR_S");
		NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
        goto    TTLPLUINI_ERR;
	}

    /*** Current/PTD get/set status info ***/
    if(blPtdFlg){
        _SetPluTotalStatusVal(0L,&dtWrk,&dtWrk,0L,0L,&psRec);
        if(PluTotalSetStsInfo(g_hdDB,PLUTOTAL_ID_PTD_CUR_S,psRec) != PLUTOTAL_SUCCESS) {
			sprintf(aszErrorBuffer,"TtlPLUInit() calling PluTotalSetStsInfo()  PLUTOTAL_ID_PTD_CUR_S");
			NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
            goto    TTLPLUINI_ERR;
		}
    }

    return  TTL_SUCCESS;           /* Return Success */

TTLPLUINI_ERR:
    PluTotalClose(g_hdDB);
    g_hdDB = PLUTOTAL_HANDLE_NULL;
    return  TTL_FAIL;           /* Return Failer */
}


/*
*============================================================================
**Synopsis:     SHORT  TtlPLUCreate(ULONG ulPLUNum, BOOL fPTD, UCHAR uchMethod)
*
*    Input:     ULONG  usPLUNum        - No of creation PLU
*    Input:     BOOL    fPTD            - Exist or not exits PTD File flag
*               UCHAR   uchMethod       - Creation Method
*                           TTL_COMP_CREATE : Compulsory Create File
*                           TTL_CHECK_CREATE: Check Create File
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     ( 0)    - Successed PLU File Creation 
*               TTL_CHECK_RAM   (-6)    - Can not PLU File Creation
*                                         (Check The RAM Size)  
*
** Description  This function creates PLU total file.
*
*============================================================================
*/

SHORT  TtlPLUCreate(ULONG ulPLUNum, BOOL fPTD, UCHAR uchMethod){
    SHORT   nSts = TTL_SUCCESS;
    ULONG    blCreateDaily = FALSE,blCreatePTD = FALSE;
    ULONG    blStsDaily = FALSE,blStsPTD = FALSE, ulStatus;
    SHORT    sPTD = fPTD;
	USHORT   usPluTableIndic = 0;

	{
		char  xBuff[128];
		sprintf (xBuff, "==NOTE: TtlPLUCreate() called.  fPTD %d, uchMethod = %d", fPTD, uchMethod);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
	}

    PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */

    PifRequestSem(usTtlSemRW);

    /* open the database */
    if(g_hdDB == PLUTOTAL_HANDLE_NULL){
		ulStatus = PluTotalOpen(&g_hdDB, sPTD, FALSE);
		NHPOS_ASSERT(ulStatus == PLUTOTAL_SUCCESS);
        if( ulStatus != PLUTOTAL_SUCCESS)
		{
			switch(ulStatus)
			{
				case PLUTOTAL_DB_ERROR_MSDEDOWN:
					NHPOS_ASSERT_TEXT(0, "**ERROR**: PluTotalOpen() PLUTOTAL_DB_ERROR_MSDEDOWN - FAULT_ERROR_MSDE_OPEN_DB. SQL Server or SQLite ODBC driver.");
					goto TTLPLUCONNEC_ERR;
				default:
					goto TTLPLUCREATE_ERR;
			}
		}
    }

	/* if sequential total read function is executed */
    if (g_nCurrentTblId) {
	    PluTotalReleaseRec(g_hdDB,g_nCurrentTblId);
	}
    g_nCurrentTblId = 0;

    /*** Delete Tables ***/
    switch(uchMethod){
        case    TTL_COMP_CREATE:
            /* drop table Current/Daily,PTD */
            PluTotalDropTbl(g_hdDB,PLUTOTAL_ID_DAILY_CUR);
            PluTotalDropTbl(g_hdDB,PLUTOTAL_ID_PTD_CUR);
            PluTotalDropTbl(g_hdDB,PLUTOTAL_ID_DAILY_SAV);
            PluTotalDropTbl(g_hdDB,PLUTOTAL_ID_PTD_SAV);
            blCreateDaily = TRUE;
            if(fPTD)
                blCreatePTD = TRUE;
            break;
        case    TTL_CHECK_CREATE:
            /* check Daily & PTD */
            blStsDaily = _CheckTable(PLUTOTAL_ID_DAILY_CUR);
            blStsPTD   = _CheckTable(PLUTOTAL_ID_PTD_CUR);
            if((blStsDaily && blStsPTD && fPTD) ||
                (blStsDaily && !blStsPTD && !fPTD)){
                PifReleaseSem(usTtlSemRW);
			    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
                return  TTL_SUCCESS;
            }
            /* Daily */
            if(!blStsDaily)
                blCreateDaily = TRUE;
            /* PTD */
            PluTotalDropTbl(g_hdDB,PLUTOTAL_ID_PTD_CUR);
            PluTotalDropTbl(g_hdDB,PLUTOTAL_ID_PTD_SAV);
            if(fPTD)
                blCreatePTD = TRUE;
            break;
        default:
            PifReleaseSem(usTtlSemRW);
		    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
			NHPOS_ASSERT_TEXT(0,"TtlPLUCreate(): Unknown method.");
            return  TTL_FAIL;
    }

    /* check plu number */
    if (ulPLUNum == 0) {
        PifReleaseSem(usTtlSemRW);
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
        return  TTL_SUCCESS;
    }

    /*** Create Tables ***/
    if(blCreateDaily){
		usPluTableIndic = PLUTOTAL_ID_DAILY_CUR;
        if(PluTotalCreateTbl (g_hdDB, PLUTOTAL_ID_DAILY_CUR, ulPLUNum) != PLUTOTAL_SUCCESS) {
			NHPOS_ASSERT_TEXT (0,"TtlPLUCreate(): PluTotalCreateTbl(g_hdDB,PLUTOTAL_ID_DAILY_CUR,ulPLUNum) != PLUTOTAL_SUCCESS.");
            goto    TTLPLUCREATE_ERR;
		}
        if(_CreateStatusRec (PLUTOTAL_ID_DAILY_CUR) != PLUTOTAL_SUCCESS) {
			NHPOS_ASSERT_TEXT (0,"TtlPLUCreate(): _CreateStatusRec(PLUTOTAL_ID_DAILY_CUR) != PLUTOTAL_SUCCESS.");
            goto    TTLPLUCREATE_ERR;
		}

        if(!_CheckTable(PLUTOTAL_ID_DAILY_SAV)){
			usPluTableIndic = PLUTOTAL_ID_DAILY_SAV;
			if(PluTotalCreateTbl (g_hdDB, PLUTOTAL_ID_DAILY_SAV, ulPLUNum) != PLUTOTAL_SUCCESS) {
				NHPOS_ASSERT_TEXT (0,"TtlPLUCreate(): PluTotalCreateTbl(g_hdDB,PLUTOTAL_ID_DAILY_SAV,ulPLUNum) != PLUTOTAL_SUCCESS.");
	            goto    TTLPLUCREATE_ERR;
			}
			if(_CreateStatusRec (PLUTOTAL_ID_DAILY_SAV) != PLUTOTAL_SUCCESS) {
				NHPOS_ASSERT_TEXT (0,"TtlPLUCreate(): _CreateStatusRec(PLUTOTAL_ID_DAILY_SAV) != PLUTOTAL_SUCCESS.");
        	    goto    TTLPLUCREATE_ERR;
			}
		}
#if defined(LOGGING)
        _CreatePluTotal( PLUTOTAL_ID_DAILY_CUR, ulPLUNum );
#endif
    }

    if(blCreatePTD){
        /* create PTD table */
		usPluTableIndic = PLUTOTAL_ID_PTD_CUR;
        if(PluTotalCreateTbl (g_hdDB, PLUTOTAL_ID_PTD_CUR, ulPLUNum) != PLUTOTAL_SUCCESS) {
			NHPOS_ASSERT_TEXT (0,"TtlPLUCreate(): PluTotalCreateTbl(g_hdDB,PLUTOTAL_ID_PTD_CUR,ulPLUNum) != PLUTOTAL_SUCCESS.");
            goto TTLPLUCREATE_ERR;
		}
        if(_CreateStatusRec (PLUTOTAL_ID_PTD_CUR) != PLUTOTAL_SUCCESS) {
			NHPOS_ASSERT_TEXT (0,"TtlPLUCreate(): _CreateStatusRec(PLUTOTAL_ID_PTD_CUR) != PLUTOTAL_SUCCESS.");
            goto    TTLPLUCREATE_ERR;
		}

        if(!_CheckTable (PLUTOTAL_ID_PTD_SAV)){
			usPluTableIndic = PLUTOTAL_ID_PTD_SAV;
			if(PluTotalCreateTbl (g_hdDB, PLUTOTAL_ID_PTD_SAV, ulPLUNum) != PLUTOTAL_SUCCESS) {
				NHPOS_ASSERT_TEXT (0,"TtlPLUCreate(): PluTotalCreateTbl(g_hdDB,PLUTOTAL_ID_PTD_SAV,ulPLUNum) != PLUTOTAL_SUCCESS.");
	            goto    TTLPLUCREATE_ERR;
			}
			if(_CreateStatusRec (PLUTOTAL_ID_PTD_SAV) != PLUTOTAL_SUCCESS) {
				NHPOS_ASSERT_TEXT (0,"TtlPLUCreate(): _CreateStatusRec(PLUTOTAL_ID_PTD_SAV) != PLUTOTAL_SUCCESS.");
        	    goto    TTLPLUCREATE_ERR;
			}
		}
#if defined(LOGGING)
        _CreatePluTotal( PLUTOTAL_ID_PTD_CUR, ulPLUNum );
#endif
    }

    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
    return  TTL_SUCCESS;

TTLPLUCREATE_ERR:
    PifReleaseSem(usTtlSemRW);
    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
    PifLog(MODULE_TTL_TUM_PLU, LOG_ERROR_PLU_CREATE_TABLE);
    PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM_PLU), 1);  // abs(-1) or TTL_FAIL
    PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM_PLU), usPluTableIndic);
    return  TTL_FAIL;


TTLPLUCONNEC_ERR:
	PifReleaseSem(usTtlSemRW);
    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
    PifAbort(MODULE_TTL_TUM_PLU, FAULT_ERROR_MSDE_OPEN_DB);
	return TTL_FAIL;
}


/*
*============================================================================
**Synopsis:     SHORT  TtlPLUDeleteFile(ULONG ulPLUNum, BOOL fPTD, UCHAR uchMethod)
*
*    Input:     ULONG  ulPLUNum        - No of creation PLU
*               BOOL    fPTD            - Exist or not exits PTD File flag
*               UCHAR   uchMethod       - Delete Method
*                           TTL_COMP_DEL : Compulsory Delete File
*                           TTL_CHECK_DEL: Check Delte File
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     ( 0)    - Successed PLU File Creation 
*
** Description  This function creates PLU total file.
*
*============================================================================
*/
SHORT  TtlPLUDeleteFile(ULONG ulPLUNum, BOOL fPTD, UCHAR uchMethod)
{
    BOOL    blStsPTD = FALSE;
    SHORT   sPTD = fPTD;

    PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
		
    switch(uchMethod){
        case    TTL_COMP_DEL:
            PluTotalDropTbl(g_hdDB,(const SHORT)PLUTOTAL_ID_DAILY_CUR);
            PluTotalDropTbl(g_hdDB,(const SHORT)PLUTOTAL_ID_PTD_CUR);
            PluTotalDropTbl(g_hdDB,(const SHORT)PLUTOTAL_ID_DAILY_SAV);
            PluTotalDropTbl(g_hdDB,(const SHORT)PLUTOTAL_ID_PTD_SAV);
            break;
        default:
        case    TTL_CHECK_DEL:
            /* check PTD table */
            blStsPTD = _CheckTable(PLUTOTAL_ID_PTD_CUR);
            if((!blStsPTD && fPTD) || (blStsPTD && !fPTD)){
	// This drop removed for VCS field issue in which PLU total database tables seem to be getting
	// dropped for no reason on startup.
	// Richard Chambers, Oct 14, 2013
//                PluTotalDropTbl(g_hdDB,(const SHORT)PLUTOTAL_ID_DAILY_CUR);
//                PluTotalDropTbl(g_hdDB,(const SHORT)PLUTOTAL_ID_PTD_CUR);
//                PluTotalDropTbl(g_hdDB,(const SHORT)PLUTOTAL_ID_DAILY_SAV);
//                PluTotalDropTbl(g_hdDB,(const SHORT)PLUTOTAL_ID_PTD_SAV);
            }
            break;
    }

    PifReleaseSem(usTtlSemRW);
    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
    return  TTL_SUCCESS;
}

/*
*============================================================================
**Synopsis:     SHORT TtlTreadPLU(TTLPLU *pTotal, VOID *pTmpBuff)
*
*    Input:     TTLPLU *pTotal          - Major & Minor Class, PLU Number
*               VOID *pTmpBuff          - Work Buffer Pointer
*
*   Output:     TTLPLU *pTotal          - PLU Total File Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Reading PLU Total File 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_NOTINFILE       - Not In File PLU 
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_NOTPTD          - Not Exist PTD
*
** Description  This function reads the PLU Total File.
*
*============================================================================
*/

SHORT TtlTreadPLU(TTLPLU *pTotal, VOID *pTmpBuff)
{
    SHORT           nTblID;
    PLUTOTAL_REC    prRecBuf;
    PLUTOTAL_STATUS psStatus;
    SHORT   nSts = TTL_SUCCESS;
    ULONG   ulDbSts;

    if(g_hdDB == PLUTOTAL_HANDLE_NULL)
        return  TTL_FILE_HANDLE_ERR;

    /* class No --> table ID */
    nTblID = _TtlPluClass2TblID(pTotal->uchMajorClass,pTotal->uchMinorClass);
    if(nTblID == TTL_NOTCLASS)
        return  TTL_NOTCLASS;

    /*PifRequestSem(usTtlSemRW);*/

    /* check PTD table */
    if(nTblID == PLUTOTAL_ID_PTD_CUR || nTblID == PLUTOTAL_ID_PTD_SAV){
        if(! _CheckTable(nTblID)){
            /*PifReleaseSem(usTtlSemRW);*/
            return  TTL_NOTPTD;
        }
    }

    /* search (get) record */
    ulDbSts = PluTotalFind(g_hdDB,nTblID,pTotal->auchPLUNumber,pTotal->uchAdjectNo,&prRecBuf);
    if(PluTotalGetStsInfo(g_hdDB,nTblID,&psStatus) == PLUTOTAL_SUCCESS){
        /* Trom & To DATE */
        pTotal->FromDate.usMDay   = psStatus.dateFrom.usDay ;
        pTotal->FromDate.usMonth  = psStatus.dateFrom.usMonth ;
#if defined(DCURRENCY_LONGLONG)
        pTotal->FromDate.usYear  = psStatus.dateFrom.usYear ;
#endif
        pTotal->FromDate.usMin    = psStatus.dateFrom.usMin ;
        pTotal->FromDate.usHour   = psStatus.dateFrom.usHour ;
    /*  pTotal->FromDate. = psStatus.usYear ;   */
    /*  pTotal->FromDate. = psStatus. ;         */
        pTotal->ToDate.usMDay   = psStatus.dateTo.usDay ;
        pTotal->ToDate.usMonth  = psStatus.dateTo.usMonth ;
#if defined(DCURRENCY_LONGLONG)
        pTotal->ToDate.usYear  = psStatus.dateTo.usYear ;
#endif
        pTotal->ToDate.usMin    = psStatus.dateTo.usMin ;
        pTotal->ToDate.usHour   = psStatus.dateTo.usHour ;
    }
    switch(ulDbSts){
        case    PLUTOTAL_SUCCESS:   /* success */
            /* PLUTOTAL_REC --> TTLPLU */
            _PltTtlRec2TtlPlu(prRecBuf,pTotal);
            pTotal->PLUAllTotal.lCounter = psStatus.lAllTotal;
            pTotal->PLUAllTotal.lAmount = psStatus.lAmount; /* ### MOD pTotal->PLUAllTotal.lAmount = 0; /* not used */
            nSts = TTL_SUCCESS;
            break;
        case    PLUTOTAL_NOT_FOUND: /* record not found */
            nSts = TTL_NOTINFILE;
            break;
        default:                    /* database error */
            nSts = TTL_FAIL;
			{
				char xBuff[128];
				sprintf (xBuff, "TtlTreadPLU(): TTL_FAIL ulDbSts = %d", ulDbSts);
				NHPOS_NONASSERT_TEXT(xBuff);
			}
            break;
    }

    /* sleep for reg. mode operation */
    g_nGetRecCount++;
    if (g_nGetRecCount > 10) {
		PifSleep(50);
		g_nGetRecCount = 0;
	}
    
    return  nSts;
}


/*
*============================================================================
**Synopsis:     SHORT TtlTresetPLU(UCHAR uchClass, VOID *pTmpBuff)
*
*    Input:     UCHAR uchClass      - Minor Class
*               VOID *pTmpBuff      - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Reseting PLU Total File 
*               TTL_NOTISSUED       - Disable reset Report Status
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_NOTPTD          - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function Resets the PLU Total File.
*
*============================================================================
*/
SHORT TtlTresetPLU(UCHAR uchClass, TTLPLU  *pPLUTotal)
{
    SHORT   nFromTblID,nToTblID;
    ULONG   ulSts,ulResetStsTmp;
    PLUTOTAL_STATUS psStatus,psStatusNew;
    DATE_TIME   dtCur;
	SHORT   sPTD = RflGetPtdFlagByType(FLEX_PLU_ADR);  /* create ptd file by option, 08/30/01 */

    /* get table id */
    if((nFromTblID = _TtlPluClass2TblID((UCHAR)CLASS_TTLPLU,uchClass)) < 0)
        return  TTL_NOTCLASS;

    switch(nFromTblID){
        case  PLUTOTAL_ID_DAILY_CUR:
            nToTblID = PLUTOTAL_ID_DAILY_SAV;
            break;
        case  PLUTOTAL_ID_PTD_CUR:
            if(!_CheckTable(PLUTOTAL_ID_PTD_CUR)){ /* ### ADD (041700) */
                return  TTL_NOTPTD;
            }

            nToTblID = PLUTOTAL_ID_PTD_SAV;
            break;
        default:
			{
				char xBuff[128];
				sprintf (xBuff, "TtlTresetPLU(): TTL_NOTCLASS nFromTblID = %d", nFromTblID);
				NHPOS_NONASSERT_TEXT(xBuff);
			}
			return  TTL_NOTCLASS;
    }

    /*PifRequestSem(usTtlSemRW);*/

	/* if sequential total read function is executed */
    if (g_nCurrentTblId) {
	    PluTotalReleaseRec(g_hdDB,g_nCurrentTblId);
	}
    g_nCurrentTblId = 0;

    /* get date */
    PifGetDateTime(&dtCur);

    /* get OLD status */
    if(PluTotalGetStsInfo(g_hdDB,nFromTblID,&psStatus) == PLUTOTAL_SUCCESS){
        /* ### MOD bug fix (041700) */
        if(psStatus.ulResetStatus & (ULONG)TTL_NOTRESET) {
			NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TTL_NOTRESET status TtlTresetPLU() ulResetStatus & (ULONG)TTL_NOTRESET")
            return  TTL_NOT_ISSUED;
		}
    }
    else{
		NHPOS_ASSERT_TEXT(0, "TTL_FAIL: PluTotalGetStsInfo() failed. TtlTresetPLU()")
        return  TTL_FAIL;
    }

//#if defined(_WIN32_WCE) && !defined(_WIN32_WCE_EMULATION)
    if ( PluTotalClose( g_hdDB ) != PLUTOTAL_SUCCESS )
    {
		NHPOS_ASSERT_TEXT(0, "TTL_FAIL: PluTotalClose() failed. TtlTresetPLU()")
        return ( TTL_FAIL );
    }
    if ( PluTotalOpen( &g_hdDB, sPTD, TRUE ) != PLUTOTAL_SUCCESS )
    {
		NHPOS_ASSERT_TEXT(0, "TTL_FAIL: PluTotalOpen() failed. TtlTresetPLU()")
        return ( TTL_FAIL );
    }
#if (defined(_WIN32_WCE) && !defined(_WIN32_WCE_EMULATION))// || defined(XP_PORT)
    if ( PluTotalCreateTbl( g_hdDB, PLUTOTAL_ID_TMP_CUR, 0 ) != PLUTOTAL_SUCCESS ) {
        return ( TTL_FAIL );
    }
//#ifndef _WIN32_WCE_EMULATION
	/* optimize PLU database, by temporary database file for performance up, V1.0.15 */
    ulSts = PluTotalMoveTbl(g_hdDB,nFromTblID, PLUTOTAL_ID_TMP_CUR);		/* move to temporary db */
    if ( ulSts == PLUTOTAL_SUCCESS )
    {
        if ( PluTotalClose( g_hdDB ) != PLUTOTAL_SUCCESS )
        {
            return ( TTL_FAIL );
        }
        if ( PluTotalDelFile( nToTblID ) != PLUTOTAL_SUCCESS )
        {
            /* return ( TTL_FAIL ); */
        }
	    if ( PluTotalCopyFile( PLUTOTAL_ID_TMP_CUR, nToTblID ) != PLUTOTAL_SUCCESS )	/* copy temporary db to saved db*/
	    {
	        if ( PluTotalDelFile( PLUTOTAL_ID_TMP_CUR ) != PLUTOTAL_SUCCESS )
    	    {
        	    /* return ( TTL_FAIL ); */
	        }
    	    if ( PluTotalOpen( &g_hdDB, sPTD, FALSE ) != PLUTOTAL_SUCCESS )
        	{
            	return ( TTL_FAIL );
	        }
	    	return (TTL_FAIL);
    	}
        if ( PluTotalDelFile( nFromTblID ) != PLUTOTAL_SUCCESS )
        {
            /* return ( TTL_FAIL ); */
        }
        if ( PluTotalDelFile( PLUTOTAL_ID_TMP_CUR ) != PLUTOTAL_SUCCESS )
        {
            /* return ( TTL_FAIL ); */
        }
        if ( PluTotalOpen( &g_hdDB, sPTD, FALSE ) != PLUTOTAL_SUCCESS )
        {
            return ( TTL_FAIL );
        }
        if ( PluTotalCreateTbl( g_hdDB, nFromTblID, 0 ) != PLUTOTAL_SUCCESS )
        {
            return ( TTL_FAIL );
        }
    }
    else
    {
        return ( TTL_FAIL );
    }
#else
    ulSts = PluTotalMoveTbl(g_hdDB,nFromTblID,nToTblID);
#if ! defined (USE_ORIGINAL)
    /* --- Fix a glitch (2001/06/20)
        To reduce data base file size, the current PLU file should be deleted
        from physical disk. If the function deletes only logical database in
        .CDB file, the actual file size of .CDB file will not be truncated.
        The large size of database lets POS appplication be slow.
    --- */
    if ( ulSts == PLUTOTAL_SUCCESS )
    {
/* #ifndef _WIN32_WCE_EMULATION
        if ( PluTotalClose( g_hdDB ) != PLUTOTAL_SUCCESS )
            return ( TTL_FAIL );
        if ( PluTotalDelFile( nFromTblID ) != PLUTOTAL_SUCCESS )
            return ( TTL_FAIL );
        if ( PluTotalOpen( &g_hdDB ) != PLUTOTAL_SUCCESS )
            return ( TTL_FAIL );
        if ( PluTotalCreateTbl( g_hdDB, nFromTblID, 0 ) != PLUTOTAL_SUCCESS )
            return ( TTL_FAIL );
#endif */
    }
    else
    {
        return ( TTL_FAIL );
    }
#endif
#endif
    /* set TO date */
    psStatus.dateTo.usYear  = dtCur.usYear;
    psStatus.dateTo.usMonth = dtCur.usMonth;
    psStatus.dateTo.usDay   = dtCur.usMDay;
    psStatus.dateTo.usHour  = dtCur.usHour  ;
    psStatus.dateTo.usMin   = dtCur.usMin   ;
    psStatus.dateTo.usSec   = dtCur.usSec   ;

	/* ### ADD (042500) Bug fix */
	ulResetStsTmp = psStatus.ulResetStatus;
	psStatus.ulResetStatus |= (ULONG)TTL_NOTRESET;

    PluTotalSetStsInfo(g_hdDB,nToTblID,psStatus);
    _SetPluTotalStatusVal(ulResetStsTmp,&dtCur,&dtCur,0L,0L,&psStatusNew);
    PluTotalSetStsInfo(g_hdDB,nFromTblID,psStatusNew);

    /*PifReleaseSem(usTtlSemRW);*/
    if(ulSts != PLUTOTAL_SUCCESS){
        return  TTL_FAIL;
    }
#if !defined(_WIN32_WCE)// || !defined(XP_PORT)
    if ( PluTotalClose( g_hdDB ) != PLUTOTAL_SUCCESS )
    {
        return ( TTL_FAIL );
    }
    if ( PluTotalOpen( &g_hdDB, sPTD, FALSE ) != PLUTOTAL_SUCCESS )
    {
        return ( TTL_FAIL );
    }
#endif
    return  TTL_SUCCESS;
}

SHORT TtlWriteSavFilePlu (UCHAR uchClass)
{
	SHORT   nFromTblID;
	SHORT   sRecStatus;
	ULONG   ulPluRecNum;
													   /* get table id */
	if ((nFromTblID = _TtlPluClass2TblID(CLASS_TTLPLU, uchClass)) < 0)
		return  TTL_NOTCLASS;

	/* Reset PLU Total File */
	TtlPLUOpenRecNoSem(nFromTblID, PLUTOTAL_SORT_PLUNO, &ulPluRecNum);
	do {
		TTLPLU      TtlPlu;

		sRecStatus = TtlPLUGetRecNoSem(&TtlPlu);

		if (sRecStatus == TTL_SUCCESS || sRecStatus == TTL_EOF)
			TtlWriteSavFile(CLASS_TTLPLU, &TtlPlu, sizeof(TtlPlu), 0);
	} while (sRecStatus == TTL_SUCCESS);

	TtlPLUCloseRecNoSem();

	return  TTL_SUCCESS;
}

SHORT TtlExportPLU(UCHAR uchClass, TCHAR *pFileName)
{
    SHORT            nFromTblID;
    ULONG            ulSts;
    PLUTOTAL_STATUS  psStatus;
    DATE_TIME        dtCur;
	SHORT     sPTD = RflGetPtdFlagByType(FLEX_PLU_ADR);  /* create ptd file by option, 08/30/01 */

    /* get table id */
    if((nFromTblID = _TtlPluClass2TblID((UCHAR)CLASS_TTLPLU, uchClass)) < 0)
        return  TTL_NOTCLASS;

    /*PifRequestSem(usTtlSemRW);*/

	/* if sequential total read function is executed */
    if (g_nCurrentTblId) {
	    PluTotalReleaseRec(g_hdDB,g_nCurrentTblId);
	}
    g_nCurrentTblId = 0;

    /* get date */
    PifGetDateTime(&dtCur);

    /* get OLD status */
    if (PluTotalGetStsInfo(g_hdDB, nFromTblID, &psStatus) == PLUTOTAL_SUCCESS){
        /* ### MOD bug fix (041700) */
        if(psStatus.ulResetStatus & (ULONG)TTL_NOTRESET) {
			NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TTL_NOTRESET status TtlExportPLU() ulResetStatus & (ULONG)TTL_NOTRESET")
            return  TTL_NOT_ISSUED;
		}
    }
    else {
		NHPOS_ASSERT_TEXT(0, "TTL_FAIL: PluTotalGetStsInfo() failed. TtlExportPLU()")
        return  TTL_FAIL;
    }

//#if defined(_WIN32_WCE) && !defined(_WIN32_WCE_EMULATION)
    if ( PluTotalClose( g_hdDB ) != PLUTOTAL_SUCCESS )
    {
		NHPOS_ASSERT_TEXT(0, "TTL_FAIL: PluTotalClose() failed. TtlExportPLU()")
        return ( TTL_FAIL );
    }
    if ( PluTotalOpen( &g_hdDB, sPTD, TRUE ) != PLUTOTAL_SUCCESS )
    {
		NHPOS_ASSERT_TEXT(0, "TTL_FAIL: PluTotalOpen() failed. TtlExportPLU()")
        return ( TTL_FAIL );
    }

#if (defined(_WIN32_WCE) && !defined(_WIN32_WCE_EMULATION))// || defined(XP_PORT)
    if ( PluTotalCreateTbl( g_hdDB, PLUTOTAL_ID_TMP_CUR, 0 ) != PLUTOTAL_SUCCESS ) {
        return ( TTL_FAIL );
    }
//#ifndef _WIN32_WCE_EMULATION
	/* optimize PLU database, by temporary database file for performance up, V1.0.15 */
    ulSts = PluTotalExportTblToFile(g_hdDB, nFromTblID, pFileName);
#else
    ulSts = PluTotalExportTblToFile(g_hdDB, nFromTblID, pFileName);
#if ! defined (USE_ORIGINAL)
    /* --- Fix a glitch (2001/06/20)
        To reduce data base file size, the current PLU file should be deleted
        from physical disk. If the function deletes only logical database in
        .CDB file, the actual file size of .CDB file will not be truncated.
        The large size of database lets POS appplication be slow.
    --- */
    if ( ulSts == PLUTOTAL_SUCCESS )
    {
/* #ifndef _WIN32_WCE_EMULATION
        if ( PluTotalClose( g_hdDB ) != PLUTOTAL_SUCCESS )
            return ( TTL_FAIL );
        if ( PluTotalDelFile( nFromTblID ) != PLUTOTAL_SUCCESS )
            return ( TTL_FAIL );
        if ( PluTotalOpen( &g_hdDB ) != PLUTOTAL_SUCCESS )
            return ( TTL_FAIL );
        if ( PluTotalCreateTbl( g_hdDB, nFromTblID, 0 ) != PLUTOTAL_SUCCESS )
            return ( TTL_FAIL );
#endif */
    }
    else
    {
        return ( TTL_FAIL );
    }
#endif
#endif

    /*PifReleaseSem(usTtlSemRW);*/
    if(ulSts != PLUTOTAL_SUCCESS){
        return  TTL_FAIL;
    }
#if !defined(_WIN32_WCE)// || !defined(XP_PORT)
    if ( PluTotalClose( g_hdDB ) != PLUTOTAL_SUCCESS )
    {
        return ( TTL_FAIL );
    }
    if ( PluTotalOpen( &g_hdDB, sPTD, FALSE ) != PLUTOTAL_SUCCESS )
    {
        return ( TTL_FAIL );
    }
#endif
    return  TTL_SUCCESS;
}


/*
*============================================================================
**Synopsis:     SHORT TtlTIssuedPLU(TTLPLU *pTotal, VOID *pTmpBuff, 
*                                      BOOL fIssued)
*
*    Input:     TTLPLU *pTotal      - Major & Minor Class, PLU Number
*               VOID *pTmpBuff      - Pointer of Work Buffer
*               UCHAR uchStat       - Status
*               UCHAR uchMethod     - Method
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Setting Reset Enable Flag of 
*                                     PLU Total File 
*               TTL_NOTCLASS        - Set Illigal Class No
*               TTL_NOTPTD          - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function sets the Reset Enable Flag of PLU Total File.
*
*============================================================================
*/
SHORT   TtlTIssuedPLU(TTLPLU *pTotal, VOID *pTmpBuff,UCHAR uchStat, UCHAR uchMethod){
    SHORT   nTblID;
    PLUTOTAL_STATUS psStatus;
    BOOL    bHit;
    SHORT   nSts;

    /* get table ID */
    if((nTblID = _TtlPluClass2TblID(pTotal->uchMajorClass,pTotal->uchMinorClass)) < 0)
        return  TTL_NOTCLASS;

    /* read status */
    /*PifRequestSem(usTtlSemRW);*/
/* ### OLD 
    if(PluTotalGetStsInfo(g_hdDB,nTblID,&psStatus) != PLUTOTAL_SUCCESS){
        switch(nTblID){
            case    PLUTOTAL_ID_PTD_CUR:
            case    PLUTOTAL_ID_PTD_SAV:
                nSts = TTL_NOTPTD; break;
            default:
                nSts = TTL_FAIL; break;
        }
        goto    TTLTISSUEDPLU_EXIT;
    }
### */
    bHit = _CheckTable(nTblID); /* hit test */
    switch(nTblID){
        case    PLUTOTAL_ID_DAILY_CUR:
            if(!bHit)
                return  TTL_FAIL;
            break;

        case    PLUTOTAL_ID_DAILY_SAV:
            if(!bHit)
                return  TTL_ISSUED;
            break;

        case    PLUTOTAL_ID_PTD_CUR:
            if(!bHit)
                return  TTL_NOTPTD; 
            break;

        case    PLUTOTAL_ID_PTD_SAV:
            if(!bHit){
                if(_CheckTable(PLUTOTAL_ID_PTD_CUR))
                    return  TTL_ISSUED;
                return  TTL_NOTPTD;
            }
            break;
        default:
            nSts = TTL_FAIL;
			{
				char xBuff[128];
				sprintf (xBuff, "TtlTIssuedPLU(): TTL_FAIL nTblID = %d", nTblID);
				NHPOS_NONASSERT_TEXT(xBuff);
			}
			break;
    }
    if(PluTotalGetStsInfo(g_hdDB,nTblID,&psStatus) != PLUTOTAL_SUCCESS) {
		NHPOS_ASSERT_TEXT(0, "TTL_FAIL: PluTotalGetStsInfo() failed. TtlTIssuedPLU()")
        return  TTL_FAIL;
    }

    /*** check/set/reset flag ***/
    switch(uchMethod){
        case    TTL_CHECKFLAG:  /* check */
            if  (psStatus.ulResetStatus & (ULONG)uchStat) {
				char  xBuff[128];
				sprintf (xBuff, "TTL_NOT_ISSUED: case TTL_CHECKFLAG TtlTIssuedPLU() ulResetStatus 0x%x uchStat 0x%x", psStatus.ulResetStatus, (ULONG)uchStat);
				NHPOS_ASSERT_TEXT(0, xBuff)
                nSts = TTL_NOT_ISSUED;
			}
            else
                nSts = TTL_ISSUED;
            break;
        case    TTL_SETFLAG:    /* set */
            psStatus.ulResetStatus |= (ULONG)uchStat;
            if(PluTotalSetStsInfo(g_hdDB,nTblID,psStatus) == PLUTOTAL_SUCCESS)
                nSts = TTL_SUCCESS;
            else {
				char  xBuff[128];
				sprintf (xBuff, "TTL_FAIL: case TTL_SETFLAG TtlTIssuedPLU() ulResetStatus 0x%x uchStat 0x%x", psStatus.ulResetStatus, (ULONG)uchStat);
				NHPOS_ASSERT_TEXT(0, xBuff)
                nSts = TTL_FAIL;
			}
            break;
        default:                /* reset */
            psStatus.ulResetStatus &= ~((ULONG)uchStat);
            if(PluTotalSetStsInfo(g_hdDB,nTblID,psStatus) == PLUTOTAL_SUCCESS)
                nSts = TTL_SUCCESS;
            else {
				NHPOS_ASSERT_TEXT(0, "TTL_FAIL: case default TtlTIssuedPLU()")
                nSts = TTL_FAIL;
			}
            break;
    }

/*TTLTISSUEDPLU_EXIT:*/
    /*PifReleaseSem(usTtlSemRW);*/
    return  nSts;
}


/*
*============================================================================
**Synopsis:     SHORT TtlTTotalCheckPLU(TTLPLU *pTotal, VOID *pTmpBuff) 
*
*    Input:     TTLPLU *pTotal          - Major & Minor Class, PLU Number
*               VOID *pTmpBuff          - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     - PLU Total is 0
**              TTL_NOTZERO     - PLU Total is not 0
*               TTL_NOTCLASS    - Set Illigal Class No
*               TTL_NOTPTD      - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function checks the PLU Total is 0.
*
*============================================================================
*/
SHORT   TtlTTotalCheckPLU(TTLPLU *pTotal, VOID *pTmpBuff){
    SHORT           nTblID;
    ULONG           ulDbSts,ulRecCnt;
    PLUTOTAL_REC    prRecBuf;

    if(g_hdDB == PLUTOTAL_HANDLE_NULL)
        return  TTL_FILE_HANDLE_ERR;

    if((nTblID = _TtlPluClass2TblID(pTotal->uchMajorClass,pTotal->uchMinorClass)) == TTL_NOTCLASS)
        return  TTL_NOTCLASS;

    /* PifRequestSem(usTtlSemRW);*/
    if (! TtlTotalCheckPLUNo(pTotal->auchPLUNumber) && (pTotal->uchAdjectNo == 0)) {

        ulDbSts = PluTotalGetRecCnt(g_hdDB,nTblID,&ulRecCnt);
        switch(pTotal->uchMinorClass) {
            case CLASS_TTLCURDAY:      /* Current Daily File */
            case CLASS_TTLSAVDAY:      /* Previous Daily File */
                /* Check No of Current/Previous Daily PLU Number */
                if(ulDbSts == PLUTOTAL_SUCCESS && ulRecCnt > 0) {
                    /*PifReleaseSem(usTtlSemRW);*/
                    return  TTL_NOTZERO;
                }
                break;
            case CLASS_TTLCURPTD:      /* Current PTD File */
            case CLASS_TTLSAVPTD:      /* Previous PTD File */
                if(ulDbSts != PLUTOTAL_SUCCESS)
                    return  TTL_NOTPTD;
                if(ulRecCnt > 0)
                    return  TTL_NOTZERO;
                break;
            default:
				{
					char xBuff[128];
					sprintf (xBuff, "TtlTTotalCheckPLU(): TTL_NOTCLASS pTotal->uchMinorClass = %d", pTotal->uchMinorClass);
					NHPOS_NONASSERT_TEXT(xBuff);
				}
                return  TTL_NOTCLASS;
        }
    }
    else{
        /* check the target PLU */
        ulDbSts = PluTotalFind(g_hdDB,nTblID,pTotal->auchPLUNumber,pTotal->uchAdjectNo,&prRecBuf);
        switch(ulDbSts){
            case    PLUTOTAL_SUCCESS:
                if(prRecBuf.lTotal > 0 || prRecBuf.lCounter > 0)
                    return  TTL_NOTZERO;
            case    PLUTOTAL_NOT_FOUND: /* record not found (== 2170GP::TTL_NOTINFILE) */
                    break;
            default:
				{
					char xBuff[128];
					sprintf (xBuff, "TtlTTotalCheckPLU(): TTL_FAIL PluTotalFind - ulDbSts = %d", ulDbSts);
					NHPOS_NONASSERT_TEXT(xBuff);
				}
                return  TTL_FAIL;
        }
    }
    return  TTL_SUCCESS;
}




PLUTOTAL_STATUS g_psStatus;
SHORT   TtlPLUOpenRec(const SHORT nTblID,const ULONG ulSearchOpt,ULONG *pulRecNum){
    ULONG   ulSts;

    PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */

    if(g_hdDB == PLUTOTAL_HANDLE_NULL) {
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
		NHPOS_ASSERT_TEXT(0, "TtlPLUOpenRec(): TTL_FAIL  PLUTOTAL_HANDLE_NULL");
        return  TTL_FAIL;
    }

    if (g_nCurrentTblId) {
		/* if already opened by another thread */
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
		NHPOS_ASSERT_TEXT(0, "TtlPLUOpenRec(): TTL_FAIL  g_nCurrentTblId nonZero. Already Opened.");
        return  TTL_FAIL;
	}
	
    PifRequestSem(usTtlSemRW);

    ulSts = PluTotalGetStsInfo(g_hdDB,nTblID,&g_psStatus);
    if(ulSts != PLUTOTAL_SUCCESS){
        PifReleaseSem(usTtlSemRW);
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
		{
			char xBuff[128];
			sprintf (xBuff, "TtlPLUOpenRec(): TTL_FAIL PluTotalGetStsInfo - ulSts = %d", ulSts);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
        return  TTL_FAIL;
    }

    /* open Recordset */
    ulSts = PluTotalSelectRec(g_hdDB,nTblID,ulSearchOpt);
    if(ulSts != PLUTOTAL_SUCCESS){
        PifReleaseSem(usTtlSemRW);
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
		{
			char xBuff[128];
			sprintf (xBuff, "TtlPLUOpenRec(): TTL_FAIL PluTotalSelectRec - ulSts = %d", ulSts);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
        return  TTL_FAIL;
    }

    /* record count */
    ulSts = PluTotalGetNumRec(g_hdDB,nTblID,pulRecNum);
    if(ulSts != PLUTOTAL_SUCCESS){
        PluTotalReleaseRec(g_hdDB,nTblID);
        PifReleaseSem(usTtlSemRW);
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
		{
			char xBuff[128];
			sprintf (xBuff, "TtlPLUOpenRec(): TTL_FAIL PluTotalGetNumRec - ulSts = %d", ulSts);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
        return  TTL_FAIL;
    }

    if(*pulRecNum > 0){
        /* move to the first record */
        ulSts = PluTotalFirstRec(g_hdDB,nTblID);
        if(ulSts != PLUTOTAL_SUCCESS){
            PluTotalReleaseRec(g_hdDB,nTblID);
            PifReleaseSem(usTtlSemRW);
	    	PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
			{
				char xBuff[128];
				sprintf (xBuff, "TtlPLUOpenRec(): TTL_FAIL PluTotalFirstRec - ulSts = %d", ulSts);
				NHPOS_NONASSERT_TEXT(xBuff);
			}
            return  TTL_FAIL;
        }
    }

    /* save current table ID */
    g_nCurrentTblId = nTblID;

    PifReleaseSem(usTtlSemRW);
   	PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */

    return  TTL_SUCCESS;
}

SHORT   TtlPLUOpenRecNoSem(const SHORT nTblID, const ULONG ulSearchOpt, ULONG *pulRecNum) {
	ULONG   ulSts;
	SHORT   sRet = TTL_SUCCESS;

	if (g_hdDB == PLUTOTAL_HANDLE_NULL) {
		NHPOS_ASSERT_TEXT(0, "TtlPLUOpenRecNoSem(): TTL_FAIL  PLUTOTAL_HANDLE_NULL");
		return  TTL_FAIL;
	}

	if (g_nCurrentTblId) {
		/* if already opened by another thread */
		NHPOS_ASSERT_TEXT(0, "TtlPLUOpenRecNoSem(): TTL_FAIL  g_nCurrentTblId nonZero. Already Opened.");
		return  TTL_FAIL;
	}

	do {
		ulSts = PluTotalGetStsInfo(g_hdDB, nTblID, &g_psStatus);
		if (ulSts != PLUTOTAL_SUCCESS) {
			{
				char xBuff[128];
				sprintf(xBuff, "TtlPLUOpenRecNoSem(): TTL_FAIL PluTotalGetStsInfo - ulSts = %d", ulSts);
				NHPOS_NONASSERT_TEXT(xBuff);
			}
			sRet = TTL_FAIL;
			break;
		}

		/* open Recordset */
		ulSts = PluTotalSelectRec(g_hdDB, nTblID, ulSearchOpt);
		if (ulSts != PLUTOTAL_SUCCESS) {
			{
				char xBuff[128];
				sprintf(xBuff, "TtlPLUOpenRecNoSem(): TTL_FAIL PluTotalSelectRec - ulSts = %d", ulSts);
				NHPOS_NONASSERT_TEXT(xBuff);
			}
			sRet = TTL_FAIL;
			break;
		}

		/* record count */
		ulSts = PluTotalGetNumRec(g_hdDB, nTblID, pulRecNum);
		if (ulSts != PLUTOTAL_SUCCESS) {
			PluTotalReleaseRec(g_hdDB, nTblID);
			{
				char xBuff[128];
				sprintf(xBuff, "TtlPLUOpenRecNoSem(): TTL_FAIL PluTotalGetNumRec - ulSts = %d", ulSts);
				NHPOS_NONASSERT_TEXT(xBuff);
			}
			sRet = TTL_FAIL;
			break;
		}

		if (*pulRecNum > 0) {
			/* move to the first record */
			ulSts = PluTotalFirstRec(g_hdDB, nTblID);
			if (ulSts != PLUTOTAL_SUCCESS) {
				PluTotalReleaseRec(g_hdDB, nTblID);
				{
					char xBuff[128];
					sprintf(xBuff, "TtlPLUOpenRecNoSem(): TTL_FAIL PluTotalFirstRec - ulSts = %d", ulSts);
					NHPOS_NONASSERT_TEXT(xBuff);
				}
				sRet = TTL_FAIL;
				break;
			}
		}

		/* save current table ID */
		g_nCurrentTblId = nTblID;
		sRet = TTL_SUCCESS;
	} while (0);

	return  sRet;
}


SHORT   TtlPLUGetRec(TTLPLU * pDst){
    PLUTOTAL_REC    prBuf;
    ULONG           ulSts;
    SHORT           nSts = TTL_SUCCESS;

    PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */

    if(g_hdDB == PLUTOTAL_HANDLE_NULL) {
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
		NHPOS_ASSERT_TEXT(0, "TtlPLUGetRec(): TTL_FAIL  PLUTOTAL_HANDLE_NULL");
        return  TTL_FAIL;
    }

    if (g_nCurrentTblId == 0) {
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
		NHPOS_ASSERT_TEXT(0, "TtlPLUGetRec(): TTL_FAIL  g_nCurrentTblId nonZero. Already Opened.");
        return  TTL_FAIL;
	}

    PifRequestSem(usTtlSemRW);

    ulSts = PluTotalGetRec(g_hdDB,g_nCurrentTblId,&prBuf);
    switch(ulSts){
        case    PLUTOTAL_SUCCESS:   /* normal */
            nSts = TTL_SUCCESS;
            break;
        case    PLUTOTAL_EOF:       /* end of recordset */
            nSts = TTL_EOF;
            break;
        default:                    /* database error */
			{
				char xBuff[128];
				sprintf (xBuff, "TtlPLUGetRec(): TTL_FAIL PluTotalGetRec - ulSts = %d", ulSts);
				NHPOS_NONASSERT_TEXT(xBuff);
			}
            nSts = TTL_FAIL;
            break;
    }
/*  if(ulSts != PLUTOTAL_SUCCESS && ulSts != PLUTOTAL_EOF){
        nSts = TTL_FAIL;
    }*/
    /*ulSts = PluTotalNextRec(g_hdDB,g_nCurrentTblId);*/

    _PltTtlRec2TtlPlu(prBuf,pDst);
    _PltTtlSts2TtlPlu(g_psStatus,pDst); /* ### ADD (042600) */
    
    /* sleep for reg. mode operation */
    g_nGetRecCount++;
    if (g_nGetRecCount > 10) {
		PifSleep(50);
		g_nGetRecCount = 0;
	}
    
    PifReleaseSem(usTtlSemRW);

   	PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
    return  nSts;
}

SHORT   TtlPLUGetRecNoSem(TTLPLU * pDst) {
	PLUTOTAL_REC    prBuf;
	ULONG           ulSts;
	SHORT           nSts = TTL_SUCCESS;

	if (g_hdDB == PLUTOTAL_HANDLE_NULL) {
		NHPOS_ASSERT_TEXT(0, "TtlPLUGetRecNoSem(): TTL_FAIL  PLUTOTAL_HANDLE_NULL");
		return  TTL_FAIL;
	}

	if (g_nCurrentTblId == 0) {
		NHPOS_ASSERT_TEXT(0, "TtlPLUGetRecNoSem(): TTL_FAIL  g_nCurrentTblId nonZero. Already Opened.");
		return  TTL_FAIL;
	}

	ulSts = PluTotalGetRec(g_hdDB, g_nCurrentTblId, &prBuf);
	switch (ulSts) {
	case    PLUTOTAL_SUCCESS:   /* normal */
		nSts = TTL_SUCCESS;
		break;
	case    PLUTOTAL_EOF:       /* end of recordset */
		nSts = TTL_EOF;
		break;
	default:                    /* database error */
		{
			char xBuff[128];
			sprintf(xBuff, "TtlPLUGetRecNoSem(): TTL_FAIL PluTotalGetRec - ulSts = %d", ulSts);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
		nSts = TTL_FAIL;
		break;
	}
	/*  if(ulSts != PLUTOTAL_SUCCESS && ulSts != PLUTOTAL_EOF){
	nSts = TTL_FAIL;
	}*/
	/*ulSts = PluTotalNextRec(g_hdDB,g_nCurrentTblId);*/

	_PltTtlRec2TtlPlu(prBuf, pDst);
	_PltTtlSts2TtlPlu(g_psStatus, pDst); /* ### ADD (042600) */

										 /* sleep for reg. mode operation */
	g_nGetRecCount++;
	if (g_nGetRecCount > 10) {
		PifSleep(50);
		g_nGetRecCount = 0;
	}

	return  nSts;
}


SHORT   TtlPLUCloseRec(VOID){
    SHORT   nSts = TTL_SUCCESS;

    PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */
    if(g_hdDB == PLUTOTAL_HANDLE_NULL) {
		g_nCurrentTblId = 0;
	   	PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
        return  TTL_FAIL;
    }

    /* close recordset */
    PifRequestSem(usTtlSemRW);
    if (g_nCurrentTblId) {
	    if(PluTotalReleaseRec(g_hdDB,g_nCurrentTblId) == PLUTOTAL_SUCCESS)
	        nSts = TTL_SUCCESS;
    	else
	        nSts = TTL_FAIL;
	}
    g_nCurrentTblId = 0;
    PifReleaseSem(usTtlSemRW);

    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */

    return  nSts;
}

SHORT   TtlPLUCloseRecNoSem(VOID) {
	SHORT   nSts = TTL_SUCCESS;

	if (g_hdDB == PLUTOTAL_HANDLE_NULL) {
		g_nCurrentTblId = 0;
		return  TTL_FAIL;
	}

	/* close recordset */
	if (g_nCurrentTblId) {
		if (PluTotalReleaseRec(g_hdDB, g_nCurrentTblId) == PLUTOTAL_SUCCESS)
			nSts = TTL_SUCCESS;
		else
			nSts = TTL_FAIL;
	}
	g_nCurrentTblId = 0;
	return  nSts;
}

SHORT   TtlPLUGetGetRecStatus(VOID) {
	if (g_nCurrentTblId) {
		return TRUE;
	} else {
		return FALSE;
	}
}

VOID    TtlPLUGetDbError(ULONG * pulPluTtlErr,ULONG * pulDBError){
    // ### MAKING
    *pulPluTtlErr = 0L;
    *pulDBError = 0L;
}


/*
*============================================================================
* ### ADD Saratoga (053100)
**Synopsis:     SHORT TtlPLUStartBackup(VOID)
*               SHORT TtlPLUStartRestore(VOID)
*               SHORT TtlPLUGetStatusBackup(LONG * pTbcStatus)
*
*    Input:     pTbcStatus          - copy status
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Success
*               TTL_FAIL            - PluTotal Error
*
** Description  Save the PLUTOTAL DB Files.
*
*============================================================================
*/

SHORT TtlPLUStartBackup(VOID){
    SHORT sRet;
    ULONG ulSts = PluTotalStartBackup();

    switch(ulSts){
        case PLUTOTAL_BK_ACCEPTED_CLEARLY:
            sRet = TTL_BK_ACCEPTED_CLEARLY;
            break;
        case PLUTOTAL_BK_ACCEPTED_LOWSTORAGE:
            sRet = TTL_BK_ACCEPTED_LOWSTORAGE;
            break;
        case PLUTOTAL_BK_DENIED_BUSY:
            sRet = TTL_BK_DENIED_BUSY;
            break;
        case PLUTOTAL_BK_DENIED_NOHEAP:
            sRet = TTL_BK_DENIED_NOHEAP;
            break;
        case PLUTOTAL_BK_DENIED_NOSTORAGE:
            sRet = TTL_BK_DENIED_NOSTORAGE;
            break;
        default:
            sRet = TTL_FAIL;
    }
	if (sRet != TTL_BK_ACCEPTED_CLEARLY) {
		char xBuff[128];

		sprintf (xBuff, "==STATE: TtlPLUStartBackup() ulSts %d sRet %d", ulSts, sRet);
		NHPOS_NONASSERT_NOTE("==STATE", xBuff);
	}
    return sRet;
}

SHORT TtlPLUStartRestore(VOID){
    SHORT sRet;
    ULONG ulSts = PluTotalStartRestore();

    switch(ulSts){
        case PLUTOTAL_BK_ACCEPTED_CLEARLY:
            sRet = TTL_BK_ACCEPTED_CLEARLY;
            break;
        case PLUTOTAL_BK_ACCEPTED_LOWSTORAGE:
            sRet = TTL_BK_ACCEPTED_LOWSTORAGE;
            break;
        case PLUTOTAL_BK_DENIED_BUSY:
            sRet = TTL_BK_DENIED_BUSY;
            break;
        case PLUTOTAL_BK_DENIED_NOHEAP:
            sRet = TTL_BK_DENIED_NOHEAP;
            break;
        case PLUTOTAL_BK_DENIED_NOSTORAGE:
            sRet = TTL_BK_DENIED_NOSTORAGE;
            break;
        default:
            sRet = TTL_FAIL;
    }
	if (sRet != TTL_BK_ACCEPTED_CLEARLY) {
		char xBuff[128];

		sprintf (xBuff, "==STATE: TtlPLUStartRestore() ulSts %d sRet %d", ulSts, sRet);
		NHPOS_NONASSERT_NOTE("==STATE", xBuff);
	}
    return sRet;
}

SHORT TtlPLUGetBackupStatus(VOID){
    ULONG   ulSts = PluTotalGetBackupStatus();
    switch(ulSts){
        case    PLUTOTAL_BK_STATUS_READY:
            return  TTL_BK_STAT_READY;
        case    PLUTOTAL_BK_STATUS_BUSY:
            return  TTL_BK_STAT_BUSY;
        default:
            return  TTL_FAIL;
    }
}

#if defined(LOGGING)
SHORT   _CreatePluTotal( SHORT nTblID, ULONG ulNumPlu )
{
    SYSCONFIG CONST	*pSysConfig;
    ULONG   Return;
    ULONG   i;
    PLUTOTAL_REC    Plu, PrevPlu;
    TCHAR    PluNumber[ PLUTOTAL_PLUNO_LEN + 1 ];
    ULONG   ulStartTime, ulEndTime, ulPlu;
    WCHAR   szMsg[ 256 ];

    pSysConfig = PifSysConfig();
    if ( pSysConfig->ausOption[ 7 ] != 9999 )
    {
        return TTL_SUCCESS;
    }

    OutputDebugString( TEXT("\r\nCreating PLU Total...") );
    PifGetTickCount( &ulStartTime );

    memset( &PrevPlu, 0x00, sizeof( PLUTOTAL_REC ));
    memset( &Plu,     0x00, sizeof( PLUTOTAL_REC ));
    Plu.usSize      = sizeof( PLUTOTAL_REC );
    Plu.lTotal      = 1000;
    Plu.lCounter    = 10;

    i = 1;
    do
    {
        PifGetTickCount( &ulPlu );
        _stprintf( PluNumber, _T("%03lu%010lu"), ulPlu % 1000, ulPlu );
        Plu.sAdjIdx = ( SHORT )( i % 5 + 1 );
        _tcsncpy( Plu.auchPluNumber, PluNumber, PLUTOTAL_PLUNO_LEN );
        //memcpy( Plu.auchPluNumber, PluNumber, PLUTOTAL_PLUNO_LEN );

        Return = PluTotalInsert( g_hdDB, nTblID, Plu );

        if ( Return == PLUTOTAL_SUCCESS )
        {
            if (( PrevPlu.auchPluNumber[ 0 ] != 0x00 ) &&
                ( PrevPlu.sAdjIdx != 0 ))
            {
                Return = PluTotalFind( g_hdDB,
                                       nTblID,
                                       PrevPlu.auchPluNumber,
                                       ( UCHAR )PrevPlu.sAdjIdx,
                                       &PrevPlu );

                PrevPlu.lTotal   += 999;
                PrevPlu.lCounter += 99;

                Return = PluTotalUpdate( g_hdDB, nTblID, PrevPlu );
            }
            _tcsncpy( PrevPlu.auchPluNumber, Plu.auchPluNumber, PLUTOTAL_PLUNO_LEN );
            //memcpy( PrevPlu.auchPluNumber, Plu.auchPluNumber, PLUTOTAL_PLUNO_LEN );
            PrevPlu.sAdjIdx = Plu.sAdjIdx;
        }
        i++;
    }
    while (( Return == PLUTOTAL_SUCCESS ) && ( i <= 0xA00 ));

    PifGetTickCount( &ulEndTime );
    wsprintf( szMsg, TEXT(" Complete [%lu]ms\r\n"), ulEndTime - ulStartTime );
    OutputDebugString( szMsg );

    wsprintf( szMsg, TEXT("Num of PLU Totals:[%lu]\r\n"), i - 1 );
    OutputDebugString( szMsg );

    return ( SHORT )(( Return == PLUTOTAL_SUCCESS ) ? TTL_SUCCESS : TTL_FAIL );
}
#endif

SHORT TtlPLUOptimize(UCHAR uchClass)
{
    SHORT    sError = 0;
	SHORT    sLoopCntReadWrt = 0, sLoopCntBackupStatus = 0;

	{
		char  xBuff[128];
		sprintf (xBuff, "==STATE: TtlPLUOptimize() start uchClass %d TtlReadWrtChk() = %d", uchClass, TtlReadWrtChk());
		NHPOS_NONASSERT_NOTE("==STATE", xBuff);
	}

    /* wait for total update is done completely, 06/05/01 */
    while(TTL_SUCCESS != TtlReadWrtChk()) { /* total still exist */
		sLoopCntReadWrt++;
        PifSleep(500);
    }

    /* 02/12/01 */
    sError = TtlPLUGetBackupStatus();
    while (sError == TTL_BK_STAT_BUSY) {
		 sLoopCntBackupStatus++;
		 PifSleep(500);
        sError = TtlPLUGetBackupStatus();
    }

    PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */
    if(g_hdDB == PLUTOTAL_HANDLE_NULL){
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
		NHPOS_NONASSERT_NOTE("==WARNING", "==WARNING:     TtlPLUOptimize() FAIL.");
		return TTL_FAIL;
	}

    PifRequestSem(usTtlSemRW);

	/* if sequential total read function is executed */
    if (g_nCurrentTblId) {
	    PluTotalReleaseRec(g_hdDB,g_nCurrentTblId);
	}
    g_nCurrentTblId = 0;

	sError = TtlPLUOptimizeDB(uchClass);

    PifReleaseSem(usTtlSemRW);

    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */

	{
		char xBuff[128];
		sprintf (xBuff, "==STATE:     TtlPLUOptimize() complete sError = %d sLoopCntReadWrt = %d sLoopCntBackupStatus = %d.", sError, sLoopCntReadWrt, sLoopCntBackupStatus);
		NHPOS_NONASSERT_NOTE("==STATE", xBuff);
	}
    return  sError;
}

SHORT TtlPLUOptimizeDB(UCHAR uchClass)
{
    ULONG   ulSts;
    SHORT   nFromTblID;
	SHORT   sPTD = RflGetPtdFlagByType(FLEX_PLU_ADR);  /* create ptd file by option, 08/30/01 */

    /* get table id */
    if((nFromTblID = _TtlPluClass2TblID((UCHAR)CLASS_TTLPLU, uchClass)) < 0)
        return  TTL_NOTCLASS;

	/* optimize daily plu database */
    if ( (ulSts = PluTotalClose(g_hdDB)) != PLUTOTAL_SUCCESS )
    {
		char  xBuff[128];

		sprintf (xBuff, "**TTL_FAIL: PluTotalClose() 1 failed. TtlPLUOptimizeDB() uchClass %d ulSts = %d", uchClass, ulSts);
		NHPOS_ASSERT_TEXT(ulSts == PLUTOTAL_SUCCESS, xBuff)
        return ( TTL_FAIL );
    }
    if ( (ulSts = PluTotalOpen(&g_hdDB, sPTD, TRUE)) != PLUTOTAL_SUCCESS )
    {
		char  xBuff[128];

		sprintf (xBuff, "**TTL_FAIL: PluTotalOpen() 1 failed. TtlPLUOptimizeDB() uchClass %d ulSts = %d", uchClass, ulSts);
		NHPOS_ASSERT_TEXT(ulSts == PLUTOTAL_SUCCESS, xBuff)
        return ( TTL_FAIL );
    }
    ulSts = PluTotalMoveTbl(g_hdDB, nFromTblID, PLUTOTAL_ID_TMP_CUR);		/* move to temporary db */
    if ( ulSts != PLUTOTAL_SUCCESS )
	{
		char  xBuff[128];
		sprintf (xBuff, "==WARNING: PluTotalMoveTbl() 1. TtlPLUOptimizeDB() uchClass %d ulSts = %d", uchClass, ulSts);
		NHPOS_ASSERT_TEXT(ulSts == PLUTOTAL_SUCCESS, xBuff);
	}
    if ( ulSts == PLUTOTAL_SUCCESS )
    {
		if ( (ulSts = PluTotalClose(g_hdDB)) != PLUTOTAL_SUCCESS )
		{
			char  xBuff[128];

			sprintf (xBuff, "**TTL_FAIL: PluTotalClose() 2 failed. TtlPLUOptimizeDB() uchClass %d ulSts = %d", uchClass, ulSts);
			NHPOS_ASSERT_TEXT(ulSts == PLUTOTAL_SUCCESS, xBuff)
			return ( TTL_FAIL );
		}
		if ( (ulSts = PluTotalOpen(&g_hdDB, sPTD, TRUE)) != PLUTOTAL_SUCCESS )
		{
			char  xBuff[128];

			sprintf (xBuff, "**TTL_FAIL: PluTotalOpen() 2 failed. TtlPLUOptimizeDB() uchClass %d ulSts = %d", uchClass, ulSts);
			NHPOS_ASSERT_TEXT(ulSts == PLUTOTAL_SUCCESS, xBuff)
			return ( TTL_FAIL );
		}

		ulSts = PluTotalMoveTbl(g_hdDB, PLUTOTAL_ID_TMP_CUR, nFromTblID);	/* move from temporaty db */
		if ( ulSts != PLUTOTAL_SUCCESS )
		{
			char  xBuff[128];

			sprintf (xBuff, "==WARNING: PluTotalMoveTbl() 2. TtlPLUOptimizeDB() uchClass %d ulSts = %d", uchClass, ulSts);
			NHPOS_ASSERT_TEXT(ulSts == PLUTOTAL_SUCCESS, xBuff);
		}
		if ( (ulSts = PluTotalClose(g_hdDB)) != PLUTOTAL_SUCCESS )
		{
			char  xBuff[128];

			sprintf (xBuff, "**TTL_FAIL: PluTotalClose() 3 failed. TtlPLUOptimizeDB() uchClass %d ulSts = %d", uchClass, ulSts);
			NHPOS_ASSERT_TEXT(ulSts == PLUTOTAL_SUCCESS, xBuff)
			return ( TTL_FAIL );
		}
		if ( (ulSts = PluTotalOpen(&g_hdDB, sPTD, FALSE)) != PLUTOTAL_SUCCESS )
		{
			char  xBuff[128];

			sprintf (xBuff, "**TTL_FAIL: PluTotalOpen() 3 failed. TtlPLUOptimizeDB() uchClass %d ulSts = %d", uchClass, ulSts);
			NHPOS_ASSERT_TEXT(ulSts == PLUTOTAL_SUCCESS, xBuff)
			return ( TTL_FAIL );
		}
    }

    return  TTL_SUCCESS;
}


/*
*============================================================================
**Synopsis:     SHORT TtlTresetPLU(UCHAR uchClass, VOID *pTmpBuff)
*
*    Input:     UCHAR uchClass      - Minor Class
*               VOID *pTmpBuff      - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Reseting PLU Total File 
*               TTL_NOTISSUED       - Disable reset Report Status
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_NOTPTD          - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function Resets the PLU Total File.
*
*============================================================================
*/
SHORT TtlTDeletePLU(UCHAR uchClass, TTLPLU *pPLUTotal)
{
    SHORT   nFromTblID;
    ULONG   ulResetStsTmp;
    PLUTOTAL_STATUS psStatus, psStatusNew;
    DATE_TIME   dtCur;
	SHORT   sPTD = RflGetPtdFlagByType(FLEX_PLU_ADR);  /* create ptd file by option, 08/30/01 */

    /* get table id */
    if((nFromTblID = _TtlPluClass2TblID((UCHAR)CLASS_TTLPLU,uchClass)) < 0)
        return  TTL_NOTCLASS;

    /*PifRequestSem(usTtlSemRW);*/

	/* if sequential total read function is executed */
    if (g_nCurrentTblId) {
	    PluTotalReleaseRec(g_hdDB,g_nCurrentTblId);
	}
    g_nCurrentTblId = 0;

    /* get date */
    PifGetDateTime(&dtCur);

    /* get OLD status */
    if(PluTotalGetStsInfo(g_hdDB,nFromTblID,&psStatus) == PLUTOTAL_SUCCESS){
        /* ### MOD bug fix (041700) */
        if(psStatus.ulResetStatus & (ULONG)TTL_NOTRESET) {
			NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTDeletePLU() ulResetStatus & (ULONG)TTL_NOTRESET")
            return  TTL_NOT_ISSUED;
		}
    }
    else{
		NHPOS_ASSERT_TEXT(0, "TTL_FAIL: TtlTDeletePLU()")
        return  TTL_FAIL;
    }

    /* --- Fix a glitch (2001/06/20)
        To reduce data base file size, the current PLU file should be deleted
        from physical disk. If the function deletes only logical database in
        .CDB file, the actual file size of .CDB file will not be truncated.
        The large size of database lets POS appplication be slow.
    --- */
#if (defined(_WIN32_WCE) && !defined(_WIN32_WCE_EMULATION))// || defined(XP_PORT)
        if ( PluTotalClose( g_hdDB ) != PLUTOTAL_SUCCESS )
        {
            return ( TTL_FAIL );
        }
        if ( PluTotalDelFile( nFromTblID ) != PLUTOTAL_SUCCESS )
        {
            /* return ( TTL_FAIL ); */
        }
        if ( PluTotalOpen( &g_hdDB, sPTD, FALSE ) != PLUTOTAL_SUCCESS )
        {
            return ( TTL_FAIL );
        }
        if ( PluTotalCreateTbl( g_hdDB, nFromTblID, 0 ) != PLUTOTAL_SUCCESS )
        {
            return ( TTL_FAIL );
        }
#else
	PluTotalDropTbl(g_hdDB,_TtlPluClass2TblID((UCHAR)CLASS_TTLPLU,uchClass));
#endif

    /* set TO date */
    psStatus.dateTo.usYear  = dtCur.usYear;
    psStatus.dateTo.usMonth = dtCur.usMonth;
    psStatus.dateTo.usDay   = dtCur.usMDay;
    psStatus.dateTo.usHour  = dtCur.usHour  ;
    psStatus.dateTo.usMin   = dtCur.usMin   ;
    psStatus.dateTo.usSec   = dtCur.usSec   ;

	ulResetStsTmp = psStatus.ulResetStatus;

    /* set FROM date */
/* ### DEL (041700)    if (pPLUTotal->uchResetStatus & TTL_NOTRESET) */
/* ### DEL (042500)    psStatus.ulResetStatus |= (ULONG)TTL_NOTRESET;*/

    /* ### MOD (042500) _SetPluTotalStatusVal(psStatus.ulResetStatus,&dtCur,&dtCur,0L,0L,&psStatusNew);*/
    _SetPluTotalStatusVal(ulResetStsTmp,&dtCur,&dtCur,0L,0L,&psStatusNew);
    PluTotalSetStsInfo(g_hdDB,nFromTblID,psStatusNew);

    /*PifReleaseSem(usTtlSemRW);*/
    return  TTL_SUCCESS;
}


/*=========================================================================*/
