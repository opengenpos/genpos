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
* Title       : TOTALPLU, TotalPlu database update Function Library  Subroutine
* Category    : TOTAL, NCR 2172 US Saratoga Application                     
* Program Name: TtlPluUp.c                                                    
* --------------------------------------------------------------------------
* Compiler    : VisualC++ Ver. 6.00 by Microsoft Corp.                      
* Memory Model:                                                             
* Options     :                                                             
* --------------------------------------------------------------------------
* Abstract:        
*   SHORT TtlPLUupdate(...)       - update database
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :Ver.Rev.:   Name    : Description
* 01/20/00 : 1.00.00: K.Iwata   : 2172 Rel1.0(Saratoga) initial
*
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


extern  PLUTOTAL_DB_HANDLE      g_hdDB;             /* database handle          */

/*VOID  _SetPluTotalStatusVal(ULONG ulResetSts,DATE_TIME *dtFrom,DATE_TIME *dtTo,LONG lAllTotal,PLUTOTAL_STATUS *ppsRec);*/
BOOL    _CheckTable(const SHORT nTblId);
ULONG   _CreateStatusRec(const SHORT nTblID);

#ifndef __DBTEST
#define __DBTEST

#ifdef  __DBTEST
BOOL    TtlPLUupdate_Test_2(USHORT usStatus, TCHAR *puchPLUNo, UCHAR uchAjectNo,
                                LONG lAmount, LONG lCounter, VOID *pTmpBuff,SHORT * psUpdateStat);
#endif

/*
*============================================================================
**Synopsis:     SHORT TtlPLUupdate(USHORT usStatus, UCHAR *puchPLUNo,
*                                  UCHAR uchAjectNo, LONG lAmount, 
*                                  LONG lCounter, VOID *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               UCHAR puchPLUNo         - PLU Number
*               UCHAR uchAjectNo        - Ajective Number
*               LONG lAmount            - Update Amount
*               LONG lCounter           - Update Counter
*               VOID *pTmpBuff          - Pointer of Work Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - PLU Update Seccess  
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_SIZEOVER        - Max Size Over
*
** Description  This function affects PLU Total.                Saratoga
*============================================================================
*/


SHORT   TtlPLUupdate(USHORT usStatus, TCHAR *puchPLUNo, UCHAR uchAjectNo,
                                DCURRENCY lAmount, LONG lCounter, VOID *pTmpBuff){
    UCHAR           uchAdjIdxWrk;
	PLUTOTAL_REC    prRecBuf = {0}, prRecBufPTD = {0};
    PLUTOTAL_STATUS psStatus = { 0 }, psStatusPTD = { 0 };
    SHORT           nSts = TTL_SUCCESS;
    CONST USHORT    usPTDFlag = RflGetPtdFlagByType(FLEX_PLU_ADR);
    ULONG           ulRecCnt = 0, ulSts = 0, ulSts1 = 0;
	char aszErrorBuffer[128];
#if defined(LOGGING)
#pragma message("***ENABLE LOGGING***")
    ULONG   ulStartTime, ulEndTime;
    WCHAR   szMsg[ 256 ];

    PifGetTickCount( &ulStartTime );
#endif

	if (usStatus & TTL_TUP_IGNORE) return (TTL_SUCCESS);

	/*** Adjective Index ***/
    if (uchAjectNo != 0)
        uchAdjIdxWrk = (UCHAR)(((uchAjectNo - 1) % 5) + 1);
    else
        uchAdjIdxWrk = 1;

    /* ### TEST 041400 */
#ifdef  __DBTEST
#pragma message("### CAUTION!!!  PluTotal.DLL (db size test)")
    if(TtlPLUupdate_Test_2(usStatus,puchPLUNo,uchAdjIdxWrk,lAmount,lCounter,pTmpBuff,&nSts) == TRUE){
        return  TTL_SUCCESS;
    }
#endif


    /*** update Current Daily/PTD ***/
    ulSts = PluTotalFind(g_hdDB,PLUTOTAL_ID_DAILY_CUR,puchPLUNo,uchAdjIdxWrk,&prRecBuf);
    if(ulSts == PLUTOTAL_NOT_FOUND){
        /* status */
        PluTotalGetRecCnt(g_hdDB,PLUTOTAL_ID_DAILY_CUR,&ulRecCnt);
        if(ulRecCnt == 0){
            /* create stsatus info */
            if((ulSts1 = _CreateStatusRec(PLUTOTAL_ID_DAILY_CUR)) != PLUTOTAL_SUCCESS) {
				sprintf(aszErrorBuffer,"TtlPLUupdate() calling _CreateStatusRec() Status1:  %d",ulSts1);
				NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
                return TTL_STATUS_DCUR;
            }
        }

        /* AddNew! */
        _tcsncpy(prRecBuf.auchPluNumber,puchPLUNo,PLUTOTAL_PLUNO_LEN);
        prRecBuf.sAdjIdx = (SHORT)uchAdjIdxWrk;
        prRecBuf.lTotal = 0L;
        prRecBuf.lCounter = 0L;

        if((ulSts1 = PluTotalInsert(g_hdDB,PLUTOTAL_ID_DAILY_CUR,prRecBuf)) != PLUTOTAL_SUCCESS) {
			sprintf(aszErrorBuffer,"TtlPLUupdate() calling PluTotalInsert()  Return: %d",ulSts1);
			NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
			return TTL_INSERT_DCUR;
        }

    }else if(ulSts != PLUTOTAL_SUCCESS){
		sprintf(aszErrorBuffer,"TtlPLUupdate() calling PluTotalFind()  Return: %d",ulSts);
		NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
        return TTL_FIND_DCUR;
    }

    /* Current PTD */

    /* --- Dec/14/2000 --- */
    //paraFlexMem.uchMajorClass = CLASS_PARAFLEXMEM;
    //paraFlexMem.uchAddress    = FLEX_PLU_ADR;
    //CliParaRead(&paraFlexMem);

    /* --- Dec/14/2000 --- */
	if(usPTDFlag == TRUE){
        ulSts = PluTotalFind(g_hdDB,PLUTOTAL_ID_PTD_CUR,puchPLUNo,uchAdjIdxWrk,&prRecBufPTD);
        if(ulSts == PLUTOTAL_NOT_FOUND){

            /* status */
            PluTotalGetRecCnt(g_hdDB,PLUTOTAL_ID_PTD_CUR,&ulRecCnt);
            if(ulRecCnt == 0){
                /* create stsatus info */
                if((ulSts1 = _CreateStatusRec(PLUTOTAL_ID_PTD_CUR)) != PLUTOTAL_SUCCESS) {
					sprintf(aszErrorBuffer,"TtlPLUupdate() calling _CreateStatusRec()  Return: %d",ulSts1);
					NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
                    return TTL_STATUS_PTD_CUR;
                }
            }

            /* AddNew */
            /* ### MOD bug fix (041700) */
            /* ### OLD if(PluTotalInsert(g_hdDB,PLUTOTAL_ID_PTD_CUR,prRecBuf) != PLUTOTAL_SUCCESS)*/
            _tcsncpy(prRecBufPTD.auchPluNumber,puchPLUNo,PLUTOTAL_PLUNO_LEN);
            prRecBufPTD.sAdjIdx = (SHORT)uchAdjIdxWrk;
            prRecBufPTD.lTotal = 0L;
            prRecBufPTD.lCounter = 0L;
            if((ulSts1 = PluTotalInsert(g_hdDB,PLUTOTAL_ID_PTD_CUR,prRecBufPTD)) != PLUTOTAL_SUCCESS) {
				sprintf(aszErrorBuffer,"TtlPLUupdate() calling PluTotalInsert()  Return: %d",ulSts1);
				NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
                return TTL_INSERT_PTD_CUR;
            }
        }else if(ulSts != PLUTOTAL_SUCCESS){
			sprintf(aszErrorBuffer,"TtlPLUupdate() calling PluTotalFind()  Return: %d",ulSts);
			NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
            return TTL_FIND_PTD_CUR;
        }
    }
    prRecBuf.lTotal += lAmount;
    prRecBuf.lCounter += lCounter;

#if defined(DCURRENCY_LONGLONG)
	switch ( usStatus & (TTL_TUP_PVOID | TTL_TUP_MVOID | TTL_TUP_SALES_RETURN_1 | TTL_TUP_SALES_RETURN_2 | TTL_TUP_SALES_RETURN_3)) {
		case TTL_TUP_PVOID:    // Preselect Transaction Void
		case TTL_TUP_MVOID:    // Cursor Void of a Serviced Guest Check item
			prRecBuf.lTotalVoid += lAmount;
			prRecBuf.lCounterVoid += lCounter;
			break;
		case TTL_TUP_SALES_RETURN_1:
			prRecBuf.lTotalRtn1 += lAmount;
			prRecBuf.lCounterRtn1 += lCounter;
			break;
		case TTL_TUP_SALES_RETURN_2:
			prRecBuf.lTotalRtn2 += lAmount;
			prRecBuf.lCounterRtn2 += lCounter;
			break;
		case TTL_TUP_SALES_RETURN_3:
			prRecBuf.lTotalRtn3 += lAmount;
			prRecBuf.lCounterRtn3 += lCounter;
			break;
	}
#endif

    /* Daily */
    if((ulSts1 = PluTotalUpdate(g_hdDB,PLUTOTAL_ID_DAILY_CUR,prRecBuf)) != PLUTOTAL_SUCCESS) {
		sprintf(aszErrorBuffer,"TtlPLUupdate() calling PluTotalUpdate()  Return: %d",ulSts1);
		NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
        return TTL_UPDATE_DCUR;
    }
    /* PTD */
    /* --- Dec/14/2000 --- */
	if(usPTDFlag == TRUE){
        prRecBufPTD.lTotal += lAmount;
        prRecBufPTD.lCounter += lCounter;
        if((ulSts1 = PluTotalUpdate(g_hdDB,PLUTOTAL_ID_PTD_CUR,prRecBufPTD)) != PLUTOTAL_SUCCESS) {
			sprintf(aszErrorBuffer,"TtlPLUupdate() calling PluTotalUpdate()  Return: %d",ulSts1);
			NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
            return TTL_UPDATE_PTD_CUR;
        }
    }

    /*** update STATUS tables ***/
    if((ulSts1 = PluTotalGetStsInfo(g_hdDB,(SHORT)PLUTOTAL_ID_DAILY_CUR,&psStatus)) != PLUTOTAL_SUCCESS) {
		sprintf(aszErrorBuffer,"TtlPLUupdate() calling PluTotalGetStsInfo()  Return: %d",ulSts1);
		NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
        return TTL_TSTAT_GDCUR;
    }

    /* --- Dec/14/2000 --- */
    // read the Period to Date totals to update them only if we are collecting the PTD totals.
    // below we modify these totals however we only update the PTD totals if we are collecting them.
	if(usPTDFlag == TRUE){
        if((ulSts1 = PluTotalGetStsInfo(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_CUR,&psStatusPTD)) != PLUTOTAL_SUCCESS) {
			sprintf(aszErrorBuffer,"TtlPLUupdate() calling PluTotalGetStsInfo()  Return: %d",ulSts1);
			NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
            return TTL_TSTAT_GPTD_CUR;
        }
    }

    if ((usStatus & (TTL_TUP_CREDT + TTL_TUP_HASH)) == 0) { /* Check Credit PLU/PLU */
        DATE_TIME   dtCur = { 0 };

		PifGetDateTime(&dtCur);

		/* set the current TO date */
		psStatus.dateTo.usYear  = dtCur.usYear;
		psStatus.dateTo.usMonth = dtCur.usMonth;
		psStatus.dateTo.usDay   = dtCur.usMDay;
		psStatus.dateTo.usHour  = dtCur.usHour;
		psStatus.dateTo.usMin   = dtCur.usMin;
		psStatus.dateTo.usSec   = dtCur.usSec;

        psStatus.lAmount += lAmount;
        psStatusPTD.lAmount += lAmount; /* Update All PLU Total Amount. will be discarded if we aren't collecting PTD totals */
        if (usStatus & TTL_TUP_SCALE) {
            if (lCounter < 0L) {
                /* ### 2172GP : PLUTtl.PLUAllTotal.lCounter += -1L; */
                psStatus.lAllTotal += -1L;
                psStatusPTD.lAllTotal += -1L;   /* Update All PLU Total Counter. will be discarded if we aren't collecting PTD totals */ 
            } else {
                /* ### 2172GP : PLUTtl.PLUAllTotal.lCounter += 1L;  */
                psStatus.lAllTotal += 1L;
                psStatusPTD.lAllTotal += 1L;    /* Update All PLU Total Counter. will be discarded if we aren't collecting PTD totals */ 
            }
        } else {
            /* ### 2172GP : PLUTtl.PLUAllTotal.lCounter += lCounter;    */
            psStatus.lAllTotal += lCounter;
            psStatusPTD.lAllTotal += lCounter;  /* Update All PLU Total Counter. will be discarded if we aren't collecting PTD totals */ 
        }
    }

    /* update status table */
    if((ulSts1 = PluTotalSetStsInfo(g_hdDB,PLUTOTAL_ID_DAILY_CUR_S,psStatus)) != PLUTOTAL_SUCCESS) {
		sprintf(aszErrorBuffer,"TtlPLUupdate() calling PluTotalSetStsInfo()  Return: %d",ulSts1);
		NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
        return TTL_TSTAT_SDCUR;
    }

    /* --- Dec/14/2000 --- */
    // update the Period to Date totals if we are collecting them.
    // if not collecting PTD totals then the PLU totals database is not updated.
	if(usPTDFlag == TRUE) {
        if((ulSts1 = PluTotalSetStsInfo(g_hdDB,PLUTOTAL_ID_PTD_CUR_S,psStatusPTD)) != PLUTOTAL_SUCCESS) {
			sprintf(aszErrorBuffer,"TtlPLUupdate() calling PluTotalSetStsInfo()  Return: %d",ulSts1);
			NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
            return TTL_TSTAT_SPTD_CUR;
        }
    }
#if defined(LOGGING)
    PifGetTickCount( &ulEndTime );
    wsprintf( szMsg,
              TEXT("TtlPLUupdate Complete [%5lu]ms\r\n"),
              ulEndTime - ulStartTime );
    OutputDebugString( szMsg );
#endif
    return  TTL_SUCCESS;
}

/*
	usPluTtlType should be one of the following values:
	 - PLUTOTAL_ID_DAILY_CUR
	 - PLUTOTAL_ID_PTD_CUR
**/
SHORT   TtlPLU_AddTotal (USHORT usPluTtlType, TCHAR *puchPLUNo, UCHAR uchAjectNo,
                                DCURRENCY lAmount, LONG lCounter, VOID *pTmpBuff)
{
	SHORT            sPluTtlType;
    UCHAR            uchAdjIdxWrk;
    PLUTOTAL_REC     prRecBuf;
    SHORT            nSts = TTL_SUCCESS;
    ULONG            ulRecCnt, ulSts, ulSts1;
	char             aszErrorBuffer[128];

	switch (usPluTtlType) {
		case CLASS_TTLCURDAY:		/* Current Daily File */
			sPluTtlType = (SHORT)PLUTOTAL_ID_DAILY_CUR;
			break;

		case CLASS_TTLCURPTD:		/* Current PTD File */
			sPluTtlType = (SHORT)PLUTOTAL_ID_PTD_CUR;
			break;
		default:
			return TTL_STATUS_DCUR;
			break;
	}

    /*** Adjective Index ***/
    if (uchAjectNo != 0)
        uchAdjIdxWrk = (UCHAR)(((uchAjectNo - 1) % 5) + 1);
    else
        uchAdjIdxWrk = 1;

    /*** update Current Daily/PTD ***/
    ulSts = PluTotalFind (g_hdDB, usPluTtlType, puchPLUNo, uchAdjIdxWrk, &prRecBuf);
    if (ulSts == PLUTOTAL_NOT_FOUND){
        /* status */
        PluTotalGetRecCnt (g_hdDB, usPluTtlType, &ulRecCnt);
        if (ulRecCnt == 0){
            /* create stsatus info */
            if ((ulSts1 = _CreateStatusRec (usPluTtlType)) != PLUTOTAL_SUCCESS) {
				sprintf (aszErrorBuffer, "TtlPLUupdate() calling _CreateStatusRec() Status1:  %d", ulSts1);
				NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
                return TTL_STATUS_DCUR;
            }
        }

        /* AddNew! */
        _tcsncpy (prRecBuf.auchPluNumber, puchPLUNo, PLUTOTAL_PLUNO_LEN);
        prRecBuf.sAdjIdx = (SHORT)uchAdjIdxWrk;
        prRecBuf.lTotal = 0L;
        prRecBuf.lCounter = 0L;
        if ((ulSts1 = PluTotalInsert (g_hdDB, usPluTtlType, prRecBuf)) != PLUTOTAL_SUCCESS) {
			sprintf (aszErrorBuffer, "TtlPLUupdate() calling PluTotalInsert()  Return: %d",ulSts1);
			NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
			return TTL_INSERT_DCUR;
        }
    } else if (ulSts != PLUTOTAL_SUCCESS) {
		sprintf (aszErrorBuffer, "TtlPLUupdate() calling PluTotalFind()  Return: %d", ulSts);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
        return TTL_FIND_DCUR;
    }

    prRecBuf.lTotal += lAmount;
    prRecBuf.lCounter += lCounter;

    /* Daily */
    if((ulSts1 = PluTotalUpdate (g_hdDB, usPluTtlType, prRecBuf)) != PLUTOTAL_SUCCESS) {
		sprintf (aszErrorBuffer, "TtlPLUupdate() calling PluTotalUpdate()  Return: %d", ulSts1);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
        return TTL_UPDATE_DCUR;
    }

    return  TTL_SUCCESS;
}

#endif