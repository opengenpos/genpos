#include	<tchar.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>

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

BOOL	_CheckTable(const SHORT nTblId);
ULONG	_CreateStatusRec(const SHORT nTblID);

SHORT	__TtlPLUupdate__(USHORT usStatus, TCHAR *puchPLUNo, UCHAR uchAjectNo,
								LONG lAmount, LONG lCounter, VOID *pTmpBuff);

LONG	__GetNum_fromMnemonic(TCHAR * pMnemonic);
void	__GenPluNo(LONG lNo,TCHAR * pPluNo);
void	__PutLog(int mode,long lCnt,char * szMsg);
BOOL	_CheckPTD(void);

BOOL	CopyFilesTest(int mode);
void	__PutLog2(int);



BOOL	TtlPLUupdate_Test_2(USHORT usStatus, TCHAR *puchPLUNo, UCHAR uchAjectNo,
								LONG lAmount, LONG lCounter, VOID *pTmpBuff,SHORT * psUpdateStat){

	SYSCONFIG CONST	*pSysConfig;
	RECPLU      	RecPlu;			/* record data (Saratoga)	*/
	ITEMNO			ItemNo;			/* plu no (Saratoga)	*/
	USHORT			usRecLen;
	USHORT			usStat;
	LONG			cnt,num;
	TCHAR			pluno[20];
	TCHAR			cWrk[100];
	//char			cWrk[100];
    ULONG   		ulTime, ulPluNo;;

	*psUpdateStat = TTL_SUCCESS;

	pSysConfig = PifSysConfig();                    /* get system config */
/*    if(pSysConfig->ausOption[7] != 2172 && 
			memcmp(puchPLUNo,"0000000000017",PLU_NUMBER_LEN) != 0)*/
	if(pSysConfig->ausOption[7] != 2172)
		return	FALSE;

	/* mnemonic == "A0A2 || A0A3 || A0A4" */
	memset(&ItemNo,0,sizeof(ITEMNO));
	_tcsncpy(ItemNo.aucPluNumber,puchPLUNo,PLU_NUMBER_LEN);
	//memcpy(ItemNo.aucPluNumber,puchPLUNo,PLU_NUMBER_LEN);
	usStat = PluReadRecord(FILE_PLU,&ItemNo,1,(VOID *)&RecPlu,&usRecLen);

	num = __GetNum_fromMnemonic(RecPlu.aucMnemonic);

	if(_tcsncmp(RecPlu.aucMnemonic,_T("A0A2"),4) == 0){
	__PutLog(1,0,NULL);
		/* up */
		for(cnt = 1;cnt <= num;cnt++){
			__GenPluNo(cnt,pluno);
			*psUpdateStat = __TtlPLUupdate__(usStatus,pluno,(UCHAR)1,lAmount,lCounter,pTmpBuff);
			if(*psUpdateStat != TTL_SUCCESS)
				break;
			/* out log */
			if((cnt % 100) == 0)
				__PutLog(3,cnt,NULL);
		}
	__PutLog(2,0,NULL);
	}
	else if(_tcsncmp(RecPlu.aucMnemonic,_T("A0A3"),4) == 0){
	__PutLog(1,0,NULL);
		/* down */
		for(cnt = num;1 <= cnt ;cnt--){
			__GenPluNo(cnt,pluno);
			*psUpdateStat = __TtlPLUupdate__(usStatus,pluno,(UCHAR)1,lAmount,lCounter,pTmpBuff);
			if(*psUpdateStat != TTL_SUCCESS)
				break;
			/* out log */
			if((cnt % 100) == 0)
				__PutLog(3,cnt,NULL);
		}

	__PutLog(2,0,NULL);
	}
	else if(_tcsncmp(RecPlu.aucMnemonic,_T("A0A4"),4) == 0){
		LONG	numOdd = (num % 2);
		LONG	numMax = num/2 + numOdd;
	__PutLog(1,0,NULL);
		for(cnt = 1;cnt <= numMax;cnt++){
			__GenPluNo(cnt,pluno);
			*psUpdateStat = __TtlPLUupdate__(usStatus,pluno,(UCHAR)1,lAmount,lCounter,pTmpBuff);
			if(*psUpdateStat != TTL_SUCCESS)
				break;

			if(!(cnt == numMax && numOdd != 0)){
				__GenPluNo(num - (cnt-1),pluno);
				*psUpdateStat = __TtlPLUupdate__(usStatus,pluno,(UCHAR)1,lAmount,lCounter,pTmpBuff);
				if(*psUpdateStat != TTL_SUCCESS)
					break;
			}
			/* out log */
			if((cnt % 100) == 0)
				__PutLog(3,cnt,NULL);
		}
	__PutLog(2,0,NULL);
	}
	else if(_tcsncmp(RecPlu.aucMnemonic,_T("A0B1"),4) == 0){
		_tcsncpy(cWrk,&RecPlu.aucMnemonic[5],2);
		//memcpy(cWrk,&RecPlu.aucMnemonic[5],2);
		cWrk[2] = '\0';
		CopyFilesTest(_ttoi(cWrk));
	} else
	if(_tcsncmp(RecPlu.aucMnemonic,_T("A0A5"),4) == 0){
	__PutLog(1,0,NULL);
    	PifGetTickCount( &ulTime );
		srand(ulTime);
		for(cnt = 1;cnt <= num;cnt++){
			ulPluNo = rand();
			if (cnt > num/2) {
				ulPluNo += rand()*10000;
			}
			__GenPluNo(ulPluNo,pluno);
			*psUpdateStat = __TtlPLUupdate__(usStatus,pluno,(UCHAR)1,lAmount,lCounter,pTmpBuff);
			if(*psUpdateStat != TTL_SUCCESS)
				break;
			/* out log */
			if((cnt % 100) == 0)
				__PutLog(3,cnt,NULL);
		}
	__PutLog(2,0,NULL);
	}
	else
		return	FALSE;

	return	TRUE;
}


SHORT	__TtlPLUupdate__(USHORT usStatus, TCHAR *puchPLUNo, UCHAR uchAjectNo,
								LONG lAmount, LONG lCounter, VOID *pTmpBuff){
	UCHAR	uchAdjIdxWrk;
	PLUTOTAL_REC 	prRecBuf,prRecBufPTD;
	PLUTOTAL_STATUS	psStatus,psStatusPTD;
	BOOL	blPTDFlg;
	SHORT	nSts = TTL_SUCCESS;
	ULONG	ulRecCnt,ulSts;

	/*** Adjective Index ***/
    if (uchAjectNo != 0)
		uchAdjIdxWrk = (UCHAR)(((uchAjectNo - 1) % 5) + 1);
	else
		uchAdjIdxWrk = 1;

	/*** update Current Daily/PTD ***/
	ulSts = PluTotalFind(g_hdDB,PLUTOTAL_ID_DAILY_CUR,puchPLUNo,uchAdjIdxWrk,&prRecBuf);
	if(ulSts == PLUTOTAL_NOT_FOUND){
		/* status */
		PluTotalGetRecCnt(g_hdDB,PLUTOTAL_ID_DAILY_CUR,&ulRecCnt);
		if(ulRecCnt == 0){
			/* create stsatus info */
			if(_CreateStatusRec(PLUTOTAL_ID_DAILY_CUR) != PLUTOTAL_SUCCESS)
				goto	TTLPLUUPDATE_ERR;
		}

		/* AddNew! */
		_tcsncpy(prRecBuf.auchPluNumber,puchPLUNo,PLUTOTAL_PLUNO_LEN);
		//memcpy(prRecBuf.auchPluNumber,puchPLUNo,PLUTOTAL_PLUNO_LEN);
		prRecBuf.sAdjIdx = (SHORT)uchAdjIdxWrk;
		prRecBuf.lTotal = 0L;
		prRecBuf.lCounter = 0L;
		if(PluTotalInsert(g_hdDB,PLUTOTAL_ID_DAILY_CUR,prRecBuf) != PLUTOTAL_SUCCESS)
			goto TTLPLUUPDATE_ERR;

	}else if(ulSts != PLUTOTAL_SUCCESS){
			goto TTLPLUUPDATE_ERR;
	}

	/* Current PTD */
	blPTDFlg = _CheckTable(PLUTOTAL_ID_PTD_CUR);
	if(blPTDFlg){
		ulSts = PluTotalFind(g_hdDB,PLUTOTAL_ID_PTD_CUR,puchPLUNo,uchAdjIdxWrk,&prRecBufPTD);
		if(ulSts == PLUTOTAL_NOT_FOUND){

			/* status */
			PluTotalGetRecCnt(g_hdDB,PLUTOTAL_ID_PTD_CUR,&ulRecCnt);
			if(ulRecCnt == 0){
				/* create stsatus info */
				if(_CreateStatusRec(PLUTOTAL_ID_PTD_CUR) != PLUTOTAL_SUCCESS)
					goto	TTLPLUUPDATE_ERR;
			}

			/* AddNew */
			/* ### MOD bug fix (041700) */
			/* ### OLD if(PluTotalInsert(g_hdDB,PLUTOTAL_ID_PTD_CUR,prRecBuf) != PLUTOTAL_SUCCESS)*/
			_tcsncpy(prRecBufPTD.auchPluNumber,puchPLUNo,PLUTOTAL_PLUNO_LEN);
			//memcpy(prRecBufPTD.auchPluNumber,puchPLUNo,PLUTOTAL_PLUNO_LEN);
			prRecBufPTD.sAdjIdx = (SHORT)uchAdjIdxWrk;
			prRecBufPTD.lTotal = 0L;
			prRecBufPTD.lCounter = 0L;
			if(PluTotalInsert(g_hdDB,PLUTOTAL_ID_PTD_CUR,prRecBufPTD) != PLUTOTAL_SUCCESS)
				goto TTLPLUUPDATE_ERR;
		}else if(ulSts != PLUTOTAL_SUCCESS){
			goto TTLPLUUPDATE_ERR;
		}
	}
	prRecBuf.lTotal += lAmount;
	prRecBuf.lCounter += lCounter;

	/* Daily */
	if(PluTotalUpdate(g_hdDB,PLUTOTAL_ID_DAILY_CUR,prRecBuf) != PLUTOTAL_SUCCESS)
		goto TTLPLUUPDATE_ERR;
	/* PTD */
	if(blPTDFlg){
		prRecBufPTD.lTotal += lAmount;
		prRecBufPTD.lCounter += lCounter;
		if(PluTotalUpdate(g_hdDB,PLUTOTAL_ID_PTD_CUR,prRecBufPTD) != PLUTOTAL_SUCCESS)
			goto TTLPLUUPDATE_ERR;
	}

	/*** update STATUS tables ***/
	//_SetPluTotalStatusVal(0,NULL,NULL,0,&psStatus);	/* init PLUTOTAL_STATUS */
	memset(&psStatus,0,sizeof(PLUTOTAL_STATUS));
	if(PluTotalGetStsInfo(g_hdDB,(SHORT)PLUTOTAL_ID_DAILY_CUR,&psStatus) != PLUTOTAL_SUCCESS)
		goto TTLPLUUPDATE_ERR;
	if(blPTDFlg){
		//_SetPluTotalStatusVal(0,NULL,NULL,0,&psStatusPTD);
		memset(&psStatusPTD,0,sizeof(PLUTOTAL_STATUS));
		if(PluTotalGetStsInfo(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_CUR,&psStatusPTD) != PLUTOTAL_SUCCESS)
			goto TTLPLUUPDATE_ERR;
	}

	if ((usStatus & (TTL_TUP_CREDT + TTL_TUP_HASH)) == 0) {	/* Check Credit PLU/PLU */
        /* ### 2170 GP : PLUTtl.PLUAllTotal.lAmount += lAmount; */
		psStatus.lAmount += lAmount;
		psStatusPTD.lAmount += lAmount;	/* Update All PLU Total Amount */
		if (usStatus & TTL_TUP_SCALE) {
			if (lCounter < 0L) {
                /* ### 2172GP : PLUTtl.PLUAllTotal.lCounter += -1L; */
				psStatus.lAllTotal += -1L;
				psStatusPTD.lAllTotal += -1L;	/* Update All PLU Total Counter */ 
			} else {
                /* ### 2172GP : PLUTtl.PLUAllTotal.lCounter += 1L;	*/
				psStatus.lAllTotal += 1L;
				psStatusPTD.lAllTotal += 1L;	/* Update All PLU Total Counter */ 
			}
		} else {
            /* ### 2172GP : PLUTtl.PLUAllTotal.lCounter += lCounter;	*/
			psStatus.lAllTotal += lCounter;
			psStatusPTD.lAllTotal += lCounter;	/* Update All PLU Total Counter */ 
		}
	}

	/* update status table */
	if(PluTotalSetStsInfo(g_hdDB,PLUTOTAL_ID_DAILY_CUR_S,psStatus) != PLUTOTAL_SUCCESS)
		goto TTLPLUUPDATE_ERR;
	if(blPTDFlg){
		if(PluTotalSetStsInfo(g_hdDB,PLUTOTAL_ID_PTD_CUR_S,psStatusPTD) != PLUTOTAL_SUCCESS)
			goto TTLPLUUPDATE_ERR;
	}

	return	TTL_SUCCESS;

TTLPLUUPDATE_ERR:
	return	TTL_FAIL;
}
