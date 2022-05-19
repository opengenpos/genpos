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
* Program Name: TtlPluSb.c                                                  
* --------------------------------------------------------------------------
* Compiler    : VisualC++ Ver. 6.00 by Microsoft Corp.                      
* Memory Model:                                                             
* Options     :                                                             
* --------------------------------------------------------------------------
* Abstract:        
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
#include	<tchar.h>
#include <memory.h>
#include <string.h>

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


extern	PLUTOTAL_DB_HANDLE		g_hdDB;				/* database handle			*/

VOID	_SetPluTotalStatusVal(ULONG ulResetSts,DATE_TIME *dtFrom,DATE_TIME *dtTo,
	LONG lAllTotal,LONG lAmount/* ### ADD 2172 (Saratoga)(041800) */,PLUTOTAL_STATUS *ppsRec){

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


SHORT	_TtlPluClass2TblID(const UCHAR uchMajorClass, const UCHAR uchMinorClass){
	if(uchMajorClass != CLASS_TTLPLU){
		return	TTL_NOTCLASS;
	}
	switch(uchMinorClass){
		case CLASS_TTLCURDAY:		/* Current Daily File */
			return	(SHORT)PLUTOTAL_ID_DAILY_CUR;

		case CLASS_TTLCURPTD:		/* Current PTD File */
			return	(SHORT)PLUTOTAL_ID_PTD_CUR;

		case	CLASS_TTLSAVDAY:	/*  */
			return	(SHORT)PLUTOTAL_ID_DAILY_SAV;

		case	CLASS_TTLSAVPTD:
			return	(SHORT)PLUTOTAL_ID_PTD_SAV;

		default:
			NHPOS_ASSERT_TEXT(0, "**WARNING: _TtlPluClass2TblID() unknown uchMinorClass.");
			return	TTL_NOTCLASS;
	}
}

VOID	_PltTtlRec2TtlPlu(const PLUTOTAL_REC src,TTLPLU *pdst){
	
	/* plu number */
	_tcsncpy(pdst->auchPLUNumber,src.auchPluNumber,PLUTOTAL_PLUNO_LEN);

	/* adjective index */
	pdst->uchAdjectNo = (UCHAR)src.sAdjIdx;

	/* total */
	pdst->PLUTotal.lAmount = src.lTotal;

	/* counter */
	pdst->PLUTotal.lCounter = src.lCounter;

	/* ResetStatus,ALLTOTAL,AMOUNT,From/To Date  ---> see _PltTtlSts2TtlPlu */
}

/* ### ADD (042600) */
VOID	_PltTtlSts2TtlPlu(const PLUTOTAL_STATUS src,TTLPLU *pdst){
	/* reset status */
	pdst->uchResetStatus = (UCHAR)src.ulResetStatus;

	/* from/to */
	pdst->FromDate.usMin		= src.dateFrom.usMin;
	pdst->FromDate.usHour	= src.dateFrom.usHour;
	pdst->FromDate.usMDay	= src.dateFrom.usDay;
	pdst->FromDate.usMonth	= src.dateFrom.usMonth;
#if defined(DCURRENCY_LONGLONG)
	pdst->FromDate.usYear	= src.dateFrom.usYear;
#endif
	pdst->ToDate.usMin		= src.dateTo.usMin;
	pdst->ToDate.usHour	= src.dateTo.usHour;
	pdst->ToDate.usMDay	= src.dateTo.usDay;
	pdst->ToDate.usMonth	= src.dateTo.usMonth;
#if defined(DCURRENCY_LONGLONG)
	pdst->ToDate.usYear	= src.dateTo.usYear;
#endif

	/* total */
	pdst->PLUAllTotal.lAmount = src.lAmount;

	/* counter */
	pdst->PLUAllTotal.lCounter = src.lAllTotal;
}


/* DATE_TIME -> PLUTOTAL_DATE */
VOID	_DATE_TIME2PLUTOTAL_DATE(const DATE_TIME dtDate,PLUTOTAL_DATE *pPD){
		pPD->usYear    = dtDate.usYear;
		pPD->usMonth   = dtDate.usMonth;
		pPD->usDay     = dtDate.usMDay;
		pPD->usHour    = dtDate.usHour;
		pPD->usMin     = dtDate.usMin;
		pPD->usSec     = dtDate.usSec;
}

/* PLUTOTAL_DATE -> DATE_TIME */
VOID	_PLUTOTAL_DATE2DATE_TIME(const PLUTOTAL_DATE pdDate,DATE_TIME *pDt){
		pDt->usYear    = pdDate.usYear ;
		pDt->usMonth   = pdDate.usMonth;
		pDt->usMDay    = pdDate.usDay  ;
		pDt->usHour    = pdDate.usHour ;
		pDt->usMin     = pdDate.usMin  ;
		pDt->usSec     = pdDate.usSec  ;
}


BOOL	_CheckTable(const SHORT nTblId){
	/*ULONG	ulRecNum;*/

	if(g_hdDB != PLUTOTAL_HANDLE_NULL){
	/*	if(PluTotalGetRecCnt(g_hdDB,nTblId,&ulRecNum) == PLUTOTAL_SUCCESS)*/
		if(PluTotalCheckTbl(g_hdDB,nTblId) == PLUTOTAL_SUCCESS){
			return	TRUE;
		}
	}
	return	FALSE;
}


VOID	_SetCurrentDateStsRec(PLUTOTAL_STATUS * ppsRec,const SHORT nMode){
	DATE_TIME	dtCur;

	/* get date */
	PifGetDateTime(&dtCur);

	switch(nMode){
		case	0:	/* all clear */
			ppsRec->dateFrom.usMonth = 0;
			ppsRec->dateFrom.usDay   = 0;
			ppsRec->dateFrom.usYear  = 0;

			ppsRec->dateFrom.usHour = 0;
			ppsRec->dateFrom.usMin = 0;
			ppsRec->dateFrom.usSec = 0;

			ppsRec->dateTo.usMonth = 0;
			ppsRec->dateTo.usDay   = 0;
			ppsRec->dateTo.usYear  = 0;

			ppsRec->dateTo.usHour = 0;
			ppsRec->dateTo.usMin = 0;
			ppsRec->dateTo.usSec = 0;

			return;
		case	1:	/* set from date */
			ppsRec->dateFrom.usMonth = dtCur.usMonth;
			ppsRec->dateFrom.usDay   = dtCur.usMDay;
			ppsRec->dateFrom.usYear  = dtCur.usYear;
			ppsRec->dateFrom.usHour  = dtCur.usHour;
			ppsRec->dateFrom.usMin   = dtCur.usMin;
			ppsRec->dateFrom.usSec   = dtCur.usSec;
			return;
		case	2:	/* set to date */
			ppsRec->dateTo.usMonth = dtCur.usMonth;
			ppsRec->dateTo.usDay   = dtCur.usMDay;
			ppsRec->dateTo.usYear  = dtCur.usYear;
			ppsRec->dateTo.usHour = dtCur.usHour;
			ppsRec->dateTo.usMin  = dtCur.usMin;
			ppsRec->dateTo.usSec  = dtCur.usSec;
			return;
		case	3:	/* both */
			ppsRec->dateFrom.usMonth = dtCur.usMonth;
			ppsRec->dateFrom.usDay   = dtCur.usMDay;
			ppsRec->dateFrom.usYear  = dtCur.usYear;
			ppsRec->dateFrom.usHour = dtCur.usHour;
			ppsRec->dateFrom.usMin  = dtCur.usMin;
			ppsRec->dateFrom.usSec  = dtCur.usSec;

			ppsRec->dateTo.usMonth = dtCur.usMonth;
			ppsRec->dateTo.usDay   = dtCur.usMDay;
			ppsRec->dateTo.usYear  = dtCur.usYear;
			ppsRec->dateTo.usHour = dtCur.usHour;
			ppsRec->dateTo.usMin  = dtCur.usMin;
			ppsRec->dateTo.usSec  = dtCur.usSec;
			return;
		default:
			return; /* bug! */
	}
}


ULONG	_CreateStatusRec(const SHORT nTblID){
	PLUTOTAL_STATUS	psRec;

	memset(&psRec,0,sizeof(PLUTOTAL_STATUS));
	_SetPluTotalStatusVal(0L,NULL,NULL,0L,0L,&psRec);
	_SetCurrentDateStsRec(&psRec,3);

	return	PluTotalSetStsInfo(g_hdDB,nTblID,psRec);
}


SHORT	_GetNextBackupTable(const SHORT nCurrent){
	switch(nCurrent){
		case	0:
			if(_CheckTable(PLUTOTAL_ID_DAILY_CUR))
				return	PLUTOTAL_ID_DAILY_CUR;
			else
				return	-1;	/* FAILER */

		case	PLUTOTAL_ID_DAILY_CUR:
			return	PLUTOTAL_ID_DAILY_SAV;

		case	PLUTOTAL_ID_DAILY_SAV:
			if(_CheckTable(PLUTOTAL_ID_PTD_CUR))
				return	PLUTOTAL_ID_PTD_CUR;
			else
				return	0;

		case	PLUTOTAL_ID_PTD_CUR:
				return	PLUTOTAL_ID_PTD_SAV;

		case	PLUTOTAL_ID_PTD_SAV:
				return	0;
		default:
				return	-1;
	}
}

/*=========================================================================*/
