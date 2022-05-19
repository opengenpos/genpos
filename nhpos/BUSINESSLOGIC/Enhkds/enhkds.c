/*
******************************************************************************
**                                                                          **
**        *=*=*=*=*=*=*=*                                                   **
**        *  NCR 2170   *             NCR Corporation, E&M OISO             **
**     @  *=*=*=*=*=*=*=*  0             (C) Copyright, 1993                **
**    <|\/~             ~\/|>                                               **
**   _/^\_               _/^\_                                              **
**                                                                          **
******************************************************************************
*=============================================================================
* Title       : Enhanced Kitchen Device System Main
* Category    : Enhanced Kitchen Device System Manager, NCR 2170 US HOSP Appl Enh.
* Program Name: Enhkds.C
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*              KdsInit()
*              KdsSendData()
*               KdsAtoI()  removed by R. Chambers, GenPOS Rel 2.2.1
*               KdsAtoL()
*               KdsFarAtoI()  removed by R. Chambers, GenPOS Rel 2.2.1
*               KdsItoA()
*               KdsLtoA()
*               KdsConvData()
*               KdsCalcCRC()
*               KdsSetTerminalNo()
*               KdsGetHostAddr()
*               KdsNetOpen()
*-----------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name       : Description
* Dec-21-99 : 01.00.00 : M.Ozawa      :initial
* Jan-21-03 : 02.00.00 : E.Smith      : SR 14 Expand KDS to support 8 Devices
* Mar-19-15 : 02.02.01 : R.Chambers   : cleanup and logs added.
*=============================================================================
*=============================================================================
* PVCS Entry
* ----------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*=============================================================================
*/

/**
;=============================================================================
;+                  I N C L U D E     F I L E s                              +
;=============================================================================
**/
/**------- MS - C ------**/
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <appllog.h>
#include <rfl.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <enhkds.h>
#include "enhkdsin.h"

/*
==============================================================================
+                  S T A T I C   R A M s                                     +
==============================================================================
*/
/* -- semaphore handle -- */
PifSemHandle usKdsSemRWSpool = PIF_SEM_INVALID_HANDLE;         /* semaphore handle for access to spool buffer */
PifSemHandle usKdsSemStubFunc = PIF_SEM_INVALID_HANDLE;        /* semaphore handle for stub function */
PifSemHandle usKdsSemSendSpool = PIF_SEM_INVALID_HANDLE;         /* semaphore handle for access to send */

/* -- spool buffer & pointer for access -- */
USHORT  ausKdsWriteP[KDS_NUM_CRT];
USHORT  usKdsSendP;

UCHAR  fbKdsControl;            /* control flag */

KDSRING aKdsRing[KDS_NUM_CRT][KDS_SPOOL_SIZE];
KDSRING KdsSendRing[KDS_SPOOL_SIZE];

ULONG   ulKdsMsgSeq;

KDSIPADDR   aKdsIPAddr[KDS_NUM_CRT];
USHORT	fKDSBackupDown[KDS_NUM_CRT];
USHORT   KDSBackupInfo[KDS_NUM_CRT];

USHORT  fKdsService;

static 	USHORT iKdsList[] = {
		KDS_IP1_ADR, KDS_IP2_ADR, KDS_IP3_ADR, KDS_IP4_ADR,
		//*********************************************
		//	usFive through usEight added for SR14
		//	ESMITH
		KDS_IP5_ADR, KDS_IP6_ADR, KDS_IP7_ADR, KDS_IP8_ADR,
		0xffff
	};


/**
;=============================================================================
;+              P R O G R A M    D E C L A R A T I O N s                     +
;=============================================================================
**/

/*
*=============================================================================
** Format  : SHORT  KdsGetHostAddr(USHORT *pfsHostAddr)
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis:
*=============================================================================
*/
static SHORT KdsGetHostAddr(USHORT usKds)
{
    PARAKDSIP ParaKdsIp;
	USHORT usKDSNo;

    /* read host ip address */
    ParaKdsIp.uchMajorClass = CLASS_PARAKDSIP;
    ParaKdsIp.uchAddress = (UCHAR)(usKds);
    CliParaRead(&ParaKdsIp);

	switch(usKds)
	{
		case KDS_IP1_ADR:
			usKDSNo = 0;
			break;
		case KDS_IP2_ADR:
			usKDSNo = 1;
			break;
		case KDS_IP3_ADR:
			usKDSNo = 2;
			break;
		case KDS_IP4_ADR:
			usKDSNo = 3;
			break;
		case KDS_IP5_ADR:
			usKDSNo = 4;
			break;
		case KDS_IP6_ADR:
			usKDSNo = 5;
			break;
		case KDS_IP7_ADR:
			usKDSNo = 6;
			break;
		case KDS_IP8_ADR:
			usKDSNo = 7;
			break;
		default:
			return FALSE;
			break;
	}

	//SR 14 ESMITH
	memcpy(aKdsIPAddr[usKDSNo].ausIPAddr, ParaKdsIp.parakdsIPPort.uchIpAddress, PARA_KDSIP_LEN);

    /* read host port no. */
    //SR 14 ESMITH
	aKdsIPAddr[usKDSNo].usPortNo = ParaKdsIp.parakdsIPPort.usNHPOSPort;
	aKdsIPAddr[usKDSNo].usFPortNo = ParaKdsIp.parakdsIPPort.usPort;

    /* check valid ip address/port no */
	if ((aKdsIPAddr[usKDSNo].usPortNo != 0) && (aKdsIPAddr[usKDSNo].usFPortNo != 0) && //SR14 ESMITH
        ((aKdsIPAddr[usKDSNo].ausIPAddr[0] != 0) ||
         (aKdsIPAddr[usKDSNo].ausIPAddr[1] != 0) ||
         (aKdsIPAddr[usKDSNo].ausIPAddr[2] != 0) ||
         (aKdsIPAddr[usKDSNo].ausIPAddr[3] != 0)) ){
        return TRUE;
    } else {
        return FALSE;
    }
}

/*
*=============================================================================
** Format  : VOID   KdsInit(VOID)
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function is called from application startup routine.
*            starts send thread, make semaphore.
*=============================================================================
*/
VOID   KdsInit(VOID)
{
    USHORT i, bAddrExists = 0;

	/* -- create semaphore -- */
    usKdsSemRWSpool  = PifCreateSem(1);         /* for access to spool buffer */
    usKdsSemStubFunc = PifCreateSem(1);         /* for access to stub function */
    usKdsSemSendSpool = PifCreateSem(0);         /* for access to send function */

    for (i=0; i < KDS_NUM_CRT; i++) {
        aKdsIPAddr[i].sHandle = -1;
    }

    fKdsService = FALSE;
    fbKdsControl = 0;
    ulKdsMsgSeq = 0L;

	/* 12/22/01 */
    memset(aKdsRing, 0, sizeof(aKdsRing));
    memset(KdsSendRing, 0, sizeof(KdsSendRing));

    for (i=0; i < KDS_NUM_CRT; i++) {
        ausKdsWriteP[i] = 0;
    }

    usKdsSendP = 0;

	// Simple test during initialization to check that host addresses
	// specified are valid.
	bAddrExists = 0;
	for (i = 0; iKdsList[i] < 0xffff; i++) {
		bAddrExists = (bAddrExists || KdsGetHostAddr(iKdsList[i]));
	}
	if (! bAddrExists) {
		PifLog (MODULE_KPS, LOG_EVENT_KPS_KDS_NO_IP_ADDRESS);
	}

}
/*
*=============================================================================
** Format  : VOID   KdsOpen(VOID)
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function is called from application startup routine.
*            starts send thread, make semaphore.
*=============================================================================
*/
VOID   KdsOpen(VOID)
{
	CHAR  CONST *aszKdsRecvThreadName = "ENH_KDS_RECV";   /* thread name */
	VOID (THREADENTRY *pFuncRecv)(VOID) = (VOID (THREADENTRY *)(VOID))KdsRecvProcess;
    SHORT  i;

	memset(&KDSBackupInfo, 0x00, sizeof(KDSBackupInfo));
    /* not service */
    if (CliParaMDCCheckField(MDC_DSP_ON_FLY3, MDC_PARAM_BIT_D) == 0) {
        fKdsService = FALSE;
		NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION: KDS service not Ethernet Type MDC 363 Bit D.");
        return;
    }

    if (fKdsService == TRUE) {
        /* already started */
        return;
    }

    PifRequestSem(usKdsSemStubFunc);
    PifRequestSem(usKdsSemRWSpool);             /* request sem. for access to spool buffer */

    fbKdsControl &= ~KDS_ALREADY_DISP;           /* set disp error flag */

/*** not clear ring buffer by each kdsopen, 12/21/01
    memset(aKdsRing, 0, sizeof(aKdsRing));
    memset(KdsSendRing, 0, sizeof(KdsSendRing));
    for (i=0; i<KDS_NUM_CRT; i++) {
        ausKdsWriteP[i] = 0;
    }
    usKdsSendP = 0;
***/

    PifReleaseSem(usKdsSemRWSpool);             /* release sem. for access buffer */

	for (i = 0; iKdsList[i] < 0xffff; i++) {
		if (KdsGetHostAddr(iKdsList[i]) == TRUE) {

			if (KdsNetOpen(iKdsList[i]) == TRUE) {
				/*
					start receive thread, if UDP/IP has opened by another port no
					We specify the KDS number (1 thru KDS_NUM_CRT) for the KDS 
					receive thread to use so that it knows which KDS device it
					is supposed to be handling.
				 */
				PifBeginThread(pFuncRecv, 0, 0, PRIO_ENH_KDS, aszKdsRecvThreadName, (USHORT)(i+1));
				fKdsService = TRUE;
			}
		}
	}

    if (fKdsService == TRUE) {
		CHAR  CONST *aszKdsSendThreadName = "ENH_KDS_SEND";   /* thread name */
		VOID (THREADENTRY *pFuncSend)(VOID) = (VOID (THREADENTRY *)(VOID))KdsSendProcess;

		NHPOS_NONASSERT_NOTE("==STATE", "==STATE: KDS service started.");

        /* start receive thread, if UDP/IP has opened by another port no. */
        PifBeginThread(pFuncSend, 0, 0, PRIO_ENH_KDS, aszKdsSendThreadName, usKdsSendP);	/* 12/21/01 */
	} else {
		NHPOS_NONASSERT_NOTE("==STATE", "==STATE: KDS service Not started.");
	}

    PifReleaseSem(usKdsSemStubFunc);            /* release sem. for stub function */
}


/*
*=============================================================================
** Format  : VOID   KdsClose(VOID)
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function is called from application startup routine.
*            starts send thread, make semaphore.
*=============================================================================
*/
VOID   KdsClose(VOID)
{
    USHORT i;

    if (fKdsService == FALSE) {
        return;
    }

    PifRequestSem(usKdsSemStubFunc);
    PifRequestSem(usKdsSemRWSpool);             /* request sem. for access to spool buffer */

    fKdsService = FALSE;

    for (i = 1; i <= KDS_NUM_CRT; i++) {
        KdsNetClose(i);
    }

    PifReleaseSem(usKdsSemRWSpool);             /* release sem. for access buffer */
    PifReleaseSem(usKdsSemStubFunc);            /* release sem. for stub function */
    PifReleaseSem(usKdsSemSendSpool);           /* release sem. for access to send process */
}

/*
*=============================================================================
** Format  : SHORT    KdsSendData(KDSSENDDATA);
*
*    Input : pData              -print data
*   Output : DFL_PERFORM        -function success (END_FRAME data)
*            DFL_BUSY           -function fail    (can not write to spool)
*            DFL_FAIL           -function fail    (wrong data received)
*    InOut : none
** Return  : none
*
** Synopsis: This function writes pData to spool buffer
*=============================================================================
*/
SHORT    KdsSendData(KDSSENDDATA *pData)
{
	SHORT     sRetStatus = KDS_PERFORM;
    KDSINF    SendInf;

    if (fKdsService == FALSE) {
        return (KDS_PERFORM);
    }

    PifRequestSem(usKdsSemStubFunc);
    PifRequestSem(usKdsSemRWSpool);             /* request sem. for access to spool buffer */

    ulKdsMsgSeq++;
    if (ulKdsMsgSeq > KDS_MAX_SEQNO) {
        ulKdsMsgSeq = 1L;
    }

    KdsConvData(pData, &SendInf);

    /* -- write to spool buffer -- */
    /* PifRequestSem(usKdsSemRWSpool);             / request sem. for access to spool buffer */
    if ( KDS_BUSY == KdsWriteSpool(&SendInf)) {    /* write to spool buffer */
        /* -- if cannot write -- */
        sRetStatus = KDS_BUSY;
    }

    PifReleaseSem(usKdsSemRWSpool);             /* release sem. for access buffer */
    PifReleaseSem(usKdsSemStubFunc);            /* release sem. for stub function */
    return (sRetStatus);
}

/*
*=============================================================================
** Format  : VOID   KdsAtoL(LONG *plDest, UCHAR *pszStr, USHORT usLen);
*
*    Input : plDest             -destination buffer
*            pszStr             -source strings pointer
*            usLen              -strings length
*
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function converts ASCII data to short data.
*=============================================================================
*/
VOID   KdsAtoL(LONG *plDest, UCHAR *pszStr, USHORT usLen)
{
	UCHAR  aszWork[24] = {0};

	if (usLen > 20) usLen = 20;
    memcpy(aszWork, pszStr, usLen);
    *plDest = atol(aszWork);
}

/*
*=============================================================================
** Format  : SHORT    KdsWriteSpool(KDSINF *pData)
*
*    Input : pData              -print data
*   Output :
*    InOut : none
** Return  : KDS_PERFORM        -success to write spool buffer
*            KDS_BUSY           -not success to write spool buffer
** Synopsis: This function writes pData to spool buffer
*
*=============================================================================
*/
SHORT  KdsWriteSpool(KDSINF *pData)
{
    KdsSendRing[usKdsSendP].ulSeqNo = ulKdsMsgSeq; /* set sequence no. at each ring, 12/21/01 */
    memcpy (&KdsSendRing[usKdsSendP].KdsInf, pData, sizeof(KDSINF)); /* write data header to spool buffer */

    usKdsSendP++;
    if (usKdsSendP >= KDS_SPOOL_SIZE) {
        usKdsSendP = 0;
    }

    PifReleaseSem(usKdsSemSendSpool);         /* release sem. for access buffer */

    return (KDS_PERFORM);
}


/*
*=============================================================================
** Format  : VOID   KdsItoA(USHORT usSorc, UCHAR *pszStr, USHORT usStrLen)
*
*    Input : usSorc             -source data
*            usStrLen           -destination buffer length
*    Output: pszStr             -destination buffer
** Return  : none
*
** Synopsis: This function converts long data to ASCII.
*=============================================================================
*/
VOID   KdsItoA(USHORT usSorc,  UCHAR *pszStr, USHORT usStrLen)
{
	UCHAR auchWork[16] = {0};
    USHORT usLen;

    _itoa((SHORT)usSorc, auchWork, 10);          /* "10" = radix */

    usLen = strlen(auchWork);
    if (usLen > usStrLen) usLen = usStrLen;

	// these are ANSI characters not UNICODE characters.
    memset(pszStr, '0', usStrLen - usLen);
    memcpy(&(pszStr[usStrLen - usLen]), auchWork, usLen);
}

/*
*=============================================================================
** Format  : VOID   KdsLtoA(USHORT usSorc, UCHAR *pszStr, USHORT usStrLen)
*
*    Input : usSorc             -source data
*            usStrLen           -destination buffer length
*    Output: pszStr             -destination buffer
** Return  : none
*
** Synopsis: This function converts long data to ASCII.
*=============================================================================
*/
VOID   KdsLtoA(ULONG ulSorc,  UCHAR *pszStr, USHORT usStrLen)
{
    USHORT usLen;
	UCHAR auchWork[16] = {0};

    _ltoa((LONG)ulSorc, auchWork, 10);          /* "10" = radix */

    usLen = strlen(auchWork);
    if (usLen > usStrLen) usLen = usStrLen;

	// these are ANSI characters not UNICODE characters.
    memset(pszStr, '0', usStrLen - usLen);
    memcpy(&(pszStr[usStrLen - usLen]), auchWork, usLen);
}

/*
*=============================================================================
** Format  : VOID   KdsConvData(KDSSENDDATA *pData, KDSINF *pInf)
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis:

  !!!!RPH!!!!RPH!!!!RPH!!!!RPH!!!
  KDS problems
*=============================================================================
*/
VOID    KdsConvData(KDSSENDDATA *pData, KDSINF *pInf)
{
	int j;
	PARATERMINALINFO TermInfo;
    //LONG lTotal;   Unnecessary after GSU SR 12 Fix by CRW 10/28/2002

	TermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
	TermInfo.uchAddress = (UCHAR)KdsGetTerminalNo();
	ParaTerminalInfoParaRead (&TermInfo);

	pInf->ulForegroundColor = pData->ulForeGroundColor;
	pInf->ulBackgroundColor = pData->ulBackGroundColor;
	pInf->uchOrderDec = pData->uchOrderDec;
	//RPH fill with dummy data until implemented
	KdsItoA(TermInfo.TerminalInfo.usLanguageID, pInf->auchLanguageId,KDS_LANGID_LEN);
	KdsItoA(TermInfo.TerminalInfo.usSubLanguageID, pInf->auchSubLanguageId,KDS_SUBLANGID_LEN);
	KdsLtoA(TermInfo.TerminalInfo.ulCodePage,pInf->auchCodePage,KDS_CODEPG_LEN);
	memset(pInf->auchDummy, '0', sizeof(pInf->auchDummy));
	//RPH fill with dummy data until implemented

    pInf->uchSTX = (UCHAR)KDS_STX;
    pInf->auchMsgIndType[0] = _T('1');
    pInf->auchMsgIndType[1] = _T('0');
    pInf->auchMsgIndType[2] = _T('0');
    pInf->auchMsgIndType[3] = _T('1');
    pInf->uchMsgType = _T('0');
    memset(pInf->auchSubMsgInfo, ' ', KDS_SMI_LEN);
    KdsLtoA(ulKdsMsgSeq, pInf->auchMsgSeqNo, KDS_MSN_LEN);
    KdsItoA(KdsGetTerminalNo(), pInf->auchTerminal, KDS_TERM_LEN);
    /* pInf->auchPort[0] = */
    KdsItoA(pData->usLineNum, pInf->aszLineNum, KDS_LINE_LEN);
    KdsItoA((USHORT)pData->uchTransMode1, &pInf->szTransMode1, 1);
    KdsItoA((USHORT)pData->uchTransMode2, &pInf->szTransMode2, 1);
    KdsItoA(pData->usLineMainType, pInf->aszLineMainType, KDS_LMT_LEN);
    KdsItoA(pData->usSubType1, pInf->aszSubType1, KDS_ST1_LEN);
    if (pData->auchSubType2[0]) {
		int len = strlen(pData->auchSubType2);
        memset(pInf->aszSubType2, ' ', KDS_ST2_LEN);
        memcpy(&pInf->aszSubType2[KDS_ST2_LEN-len], pData->auchSubType2, len);
    } else {
        KdsLtoA(pData->ulSubType2, pInf->aszSubType2, KDS_ST2_LEN);
    }
    KdsItoA(pData->sQty, pInf->aszQty, KDS_QTY_LEN);
    //_tcsncpy(pInf->aszPluNo, pData->aszPluNo, KDS_PLU_LEN);
    for(j = 0; j < KDS_PLU_LEN; j++)
	{
		pInf->aszPluNo[j] = (UCHAR)pData->aszPluNo[j];
	}
	//memcpy(pInf->aszPluNo, pData->aszPluNo, KDS_PLU_LEN);
    /* pInf->aszRouting[0] = */
    KdsItoA((USHORT)(pData->ulOperatorId % 1000), pInf->aszOperatorId, KDS_OPEID_LEN);
    _tcsncpy(pInf->aszLineData, pData->aszLineData, KDS_LINE_DATA_LEN);


	// fix to send a negative price to the Scoreboard/KDS for GSU SR 12 by CRW on 10/28/2002
	// simply send itemTotal to the conversion function,
	// remove the ternary operation the used to ensured positive itemTotal was sent
	KdsLtoA(pData->lItemTotal, pInf->aszItemTotal,  KDS_I_TOTAL_LEN);
    KdsLtoA(pData->lTransTotal,pInf->aszTransTotal,  KDS_T_TOTAL_LEN);
    KdsLtoA(pData->lTransTax, pInf->aszTransTax, KDS_T_TAX_LEN);
    //_tcsncpy(pInf->aszRouting, pData->auchCRTNo, KDS_ROUTE_LEN);  /* by FVT */
    memcpy(pInf->aszRouting, pData->auchCRTNo, KDS_ROUTE_LEN);  /* by FVT */
    pInf->uchETX = (UCHAR)KDS_ETX;
    /* pInf->uchCRC = KdsCalcCRC((UCHAR *)pData, sizeof(KDSINF)-1); */
}

/*
*=============================================================================
** Format  : UCHAR   KdsCalcCRC(UCHAR *puchData, USHORT usLen)
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis:
*=============================================================================
*/
UCHAR KdsCalcCRC(UCHAR *puchData, USHORT usLen)
{
    USHORT  i;
    UCHAR uchCRC;

    /* calculate CRC check character */
    uchCRC = *puchData;
    for (i = 1; i < usLen - 1; i++) {
        uchCRC ^= puchData[i];
    }

    return (uchCRC);
}


/*
*=============================================================================
** Format  : USHORT   KdsGetTerminalNo(VOID)
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis:
*=============================================================================
*/
USHORT KdsGetTerminalNo(VOID)
{
    SYSCONFIG CONST *SysCon = PifSysConfig();

    return (USHORT)SysCon->auchLaddr[3];
}


/* ---------- End of enhkds.c ---------- */

