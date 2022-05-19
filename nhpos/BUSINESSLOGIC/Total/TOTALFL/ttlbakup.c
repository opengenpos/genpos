/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1995       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*
*  NeighborhoodPOS Software Development Project
*  Department of Information Systems
*  College of Information Technology
*  Georgia Southern University
*  Statesboro, Georgia
*
*  Copyright 2014 Georgia Southern University Research and Services Foundation
* 
*===========================================================================
* Title       : TOTAL, Total Function Library (Backup)                        
* Category    : TOTAL, NCR 2170 US Hospitality Application         
* Program Name: Ttlbakup.c                                                      
* --------------------------------------------------------------------------
* Abstract:        
*           SHORT  TtlReqBackup()   - Receive Total File
*           SHORT  TtlReqBackEach() - Receive each total file
*
*           SHORT  TtlRespBackup()  - Send response data
*           USHORT TtlResBackEach() - Send Base total file
*     
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data   : Ver.Rev. :   Name     : Description
* Jul-27-92 : 00.00.01 : H.Nakashima:                                    
* Mar- 2-95 : 03.00.00 : hkato      : R3.0
* Nov-13-95 : 03.01.00 : T.Nakahata : R3.1 Initial
*       Add Service Time Total (Daily/PTD) and Individual Financial (Daily)
* Apr-09-96 : 03.01.03 : T.Nakahata : check file size for service time backup
* Aug-11-99 : 03.05.00 : K.Iwata    : R3.5 Remove WAITER_MODEL
* Aug-13-99 : 03.05.00 : K.Iwata    : marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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
#include <tchar.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <ecr.h>
#include <pif.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <csttl.h>
#include <csstbfcc.h>
#include <csstbstb.h>
#include <ttl.h>
#include <appllog.h>

/*
*============================================================================
**Synopsis:     SHORT  TtlReqBackup(VOID)
*
*    Input:     
*
*   Output:     Nothing
*    InOut:
*
** Return:      TTL_SUCCES       Success 
*               TTl_FAIL         Fail
*
** Description  This function receives the Total File.               R3.0
*
*============================================================================
*/
SHORT  TtlReqBackup(VOID)
{
    SHORT   sError;
	char    xBuff[128];
	char    *xBuffFmt = "==WARNING: TtlReqBackup() Log %d, shFile %d, sError %d";


    if (TTL_SUCCESS != (sError = TtlReqBackEach(hsTtlBaseHandle, TTL_BAK_BASE))) {    /* backup base file */
		PifLog(MODULE_TTL_TFL_BACKUP, TTL_BAK_BASE);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(hsTtlBaseHandle));
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(sError));
		sprintf (xBuff, xBuffFmt, TTL_BAK_BASE, hsTtlBaseHandle, sError);
		NHPOS_ASSERT_TEXT((sError == TTL_SUCCESS), xBuff);
        return sError;
    }
    if (TTL_SUCCESS != (sError = TtlReqBackEach(hsTtlDeptHandle, TTL_BAK_DEPT))) {      /* backup dept total file */
		PifLog(MODULE_TTL_TFL_BACKUP, TTL_BAK_DEPT);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(hsTtlDeptHandle));
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(sError));
		sprintf (xBuff, xBuffFmt, TTL_BAK_DEPT, hsTtlDeptHandle, sError);
		NHPOS_ASSERT_TEXT((sError == TTL_SUCCESS), xBuff);
        return sError;
    }

    /* wait for copy completes */
    /* 02/10/01 */
    sError = TtlPLUGetBackupStatus();
    while (sError == TTL_BK_STAT_BUSY) {
        PifSleep(1000);
        sError = TtlPLUGetBackupStatus();
    }

    PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */
	
	//Memory Leak 7.8.05
    //TtlPLUCloseDB();

    if (TTL_SUCCESS != (sError = TtlReqBackEachPlu(CLASS_TTLCURDAY))) {    /*  backup PLUtotal (Daily/Cur) */
		//Memory Leak 7.8.05
        //TtlPLUOpenDB();
		PifLog(MODULE_TTL_TFL_BACKUP, LOG_ERROR_TTL_CODE_01);
		PifLog(MODULE_TTL_TFL_BACKUP, TTL_BAK_PLU);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(sError));
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
        return sError;
    }
    if (TTL_SUCCESS != (sError = TtlReqBackEachPlu(CLASS_TTLSAVDAY))) {   /*  backup PLUtotal (Daily/Cur) */
		//Memory Leak 7.8.05
        //TtlPLUOpenDB();
		PifLog(MODULE_TTL_TFL_BACKUP, LOG_ERROR_TTL_CODE_02);
		PifLog(MODULE_TTL_TFL_BACKUP, TTL_BAK_PLU);
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(sError));
        return sError;
    }
    if (TTL_SUCCESS != (sError = TtlReqBackEachPlu(CLASS_TTLCURPTD))) {  /*  backup PLUtotal (Daily/Cur) */
		//Memory Leak 7.8.05
        //TtlPLUOpenDB();
		PifLog(MODULE_TTL_TFL_BACKUP, LOG_ERROR_TTL_CODE_03);
		PifLog(MODULE_TTL_TFL_BACKUP, TTL_BAK_PLU);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(sError));
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
        return sError;
    }
    if (TTL_SUCCESS != (sError = TtlReqBackEachPlu(CLASS_TTLSAVPTD))) {   /*  backup PLUtotal (Daily/Cur) */
		//Memory Leak 7.8.05
        //TtlPLUOpenDB();
		PifLog(MODULE_TTL_TFL_BACKUP, LOG_ERROR_TTL_CODE_04);
		PifLog(MODULE_TTL_TFL_BACKUP, TTL_BAK_PLU);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(sError));
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
        return sError;
    }
    /* --- Sleep to wait for the finalize of ADOCECLOSE,    Dec/15/2000 --- */
	
	//Memory Leak 7.8.05
	TtlPLUCloseDB();
    TtlPLUOpenDB();

	PifSleep(3000);

    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */

    if (TTL_SUCCESS != (sError = TtlReqBackEach(hsTtlCasHandle, TTL_BAK_CASHIER))) {   /* backup cashier total file */
		PifLog(MODULE_TTL_TFL_BACKUP, TTL_BAK_CASHIER);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(hsTtlCasHandle));
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(sError));
		sprintf (xBuff, xBuffFmt, TTL_BAK_CASHIER, hsTtlCasHandle, sError);
		NHPOS_ASSERT_TEXT((sError == TTL_SUCCESS), xBuff);
        return sError;
    }
    if (TTL_SUCCESS != (sError = TtlReqBackEach(hsTtlIndFinHandle, TTL_BAK_INDFIN))) {   /* backup individual financial, R3.1 */
		PifLog(MODULE_TTL_TFL_BACKUP, TTL_BAK_INDFIN);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(hsTtlIndFinHandle));
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(sError));
		sprintf (xBuff, xBuffFmt, TTL_BAK_INDFIN, hsTtlIndFinHandle, sError);
		NHPOS_ASSERT_TEXT((sError == TTL_SUCCESS), xBuff);
        return sError;
    }

	// Coupon totals and Service Time totals will depend on the provisioning of GenPOS.
	// Not all installations will be using these so do them last.
    if (TTL_SUCCESS != (sError = TtlReqBackEach(hsTtlCpnHandle, TTL_BAK_CPN))) { /* backup coupon total file, R3.0 */
		PifLog(MODULE_TTL_TFL_BACKUP, TTL_BAK_CPN);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(hsTtlCpnHandle));
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(sError));
//        return sError;
    }
    if (TTL_SUCCESS != (sError = TtlReqBackEach(hsTtlSerTimeHandle, TTL_BAK_SERTIME))) {  /* bakup service time file, R3.1 */
		PifLog(MODULE_TTL_TFL_BACKUP, TTL_BAK_SERTIME);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(hsTtlSerTimeHandle));
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(sError));
//        return sError;
    }
    /* ===== New Total Files (Release 3.1) END ===== */

    return TTL_SUCCESS;
}

/*
*============================================================================
**Synopsis:     SHORT  TtlReqBackEach(SHORT hsHandle, UCHAR uchType)
*
*    Input:     hsHandle    - each file handle
*               uchType     - What file would like to read
*
*   Output:     Nothing
*    InOut:
*
** Return:      TTL_SUCCES       Success 
*               TTl_FAIL         Fail
*
** Description  This function receives the Total File.
*
*============================================================================
*/
SHORT   TtlReqBackEach(SHORT hsHandle, UCHAR uchType)
{
    TTLBACKUP_REQ   RequestBuff;
    TTLBACKUP_RES   ResponseBuff;
    USHORT          usMaxLen;
    SHORT           sError;

	if (0 > hsHandle) {
        PifLog(MODULE_TTL_TFL_BACKUP, LOG_ERROR_TTL_FILE_HANDLE); /* ### Mod (2171 for Win32) V1.0 */
        PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(TTL_FILE_OPEN_ERR)); /* ### Mod (2171 for Win32) V1.0 */
		return TTL_FILE_OPEN_ERR;
	}

    RequestBuff.uchFileType = uchType;      /* What file ? */
    RequestBuff.ulOffSet    = 0L;           /* start offset */
    RequestBuff.usCount     = 0;            /* request counter */

    do {
        RequestBuff.usCount ++;             /* counter increment */
        usMaxLen = sizeof(TTLBACKUP_RES);   /* buffer size maximum */
                                            /* REQUEST READ */
        sError = SstReqBackUp(CLI_FCBAKTOTAL, (UCHAR *)&RequestBuff, sizeof(TTLBACKUP_REQ), (UCHAR *)&ResponseBuff, &usMaxLen);

        if (TTL_SUCCESS == sError) {        /* reading success ? */
                                            /* handle exist ? */
            if ((hsHandle < 0) && (0 != ResponseBuff.D.usDataSize)) {
                PifLog(MODULE_TTL_TFL_BACKUP, LOG_ERROR_TTL_FILE_HANDLE); /* ### Mod (2171 for Win32) V1.0 */
                PifLog(MODULE_DATA_VALUE(MODULE_TTL_TFL_BACKUP), uchType); /* ### Mod (2171 for Win32) V1.0 */
                PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(hsHandle)); /* ### Mod (2171 for Win32) V1.0 */
                return (TTL_FAIL);
            }
                                            /* sequence # check */
            if (RequestBuff.usCount != ResponseBuff.F.usCount) {
                if (RequestBuff.usCount == (ResponseBuff.F.usCount - 1)) {
                    RequestBuff.usCount --;
                } else {
                    RequestBuff.ulOffSet = 0L; 
                    RequestBuff.usCount = 0;
                }
                continue;
            }
                                            /* data exist ? */
            if (0 < ResponseBuff.D.usDataSize) {
                                            /* write into file */
                sError = TtlWriteFile(hsHandle, ResponseBuff.F.ulOffSet, ResponseBuff.D.auchData, ResponseBuff.D.usDataSize);

                if (TTL_SUCCESS != sError) {/* writing success ? */    
                    PifLog(MODULE_TTL_TFL_BACKUP, FAULT_ERROR_FILE_SEEK);
					PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(sError));
                    PifAbort(MODULE_TTL_TFL_BACKUP, FAULT_ERROR_FILE_SEEK);
                }
            }

        } else {                            /* reading fail ? */
            PifLog(MODULE_TTL_TFL_BACKUP, LOG_ERROR_TTL_BACKUP_MASTER);
            PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(sError));
        }
                                            /* increment offset */
        RequestBuff.ulOffSet += ResponseBuff.D.usDataSize;

    } while ((TTL_SUCCESS == sError) && (0 == ResponseBuff.F.uchEOF));

    return (sError);
}

/*
*============================================================================
**Synopsis:     SHORT  TtlRespBackup(UCHAR *puchReqBuf, USHORT usSize,
*                                    UCHAR *puchRcvBuf, USHORT *pusRcvLen)
*
*    Input:     puchReqBuf  - request message from O/D terminal
*               usSize      - request message length
*   Output:     puchRcvBuf  - send message to O/D terminal (from me)
*    InOut:     pusRcvLen   - send message length
*
** Return:      TTL_SUCCES       Success 
*               TTL_FAIL         Fail
*
** Description  This function sends total files.                     R3.0
*
*============================================================================
*/
SHORT  TtlRespBackup(UCHAR *puchReqBuf, USHORT usSize, 
                     UCHAR *puchRcvBuf, USHORT *pusRcvLen)
{
    TTLBACKUP_REQ   *pRecv;
    TTLBACKUP_RES   *pSend;
    USHORT          usMaxLen;

    pRecv = (TTLBACKUP_REQ *)puchReqBuf;    /* pointer copy */     
    pSend = (TTLBACKUP_RES *)puchRcvBuf;

    usMaxLen = *pusRcvLen;                  /* max size calculation */
    if (usMaxLen > TTL_BAK_BUFFER) {
        usMaxLen = TTL_BAK_BUFFER;
    }
                                            /* copy from Req. to Resp */
    pSend->F.uchFileType = pRecv->uchFileType;
    pSend->F.ulOffSet    = pRecv->ulOffSet;
    pSend->F.usCount     = pRecv->usCount;
    pSend->F.sFileSubType = 0; /* ### ADD 2172 Rel1.0 (01/25/00) */

    if (sizeof(TTLBACKUP_REQ) != usSize) {  /* request data error */
        pSend->F.sError = TTL_FAIL;
        *pusRcvLen = 0;
		NHPOS_ASSERT_TEXT((sizeof(TTLBACKUP_REQ) == usSize), "TtlRespBackup(): Request data struct size error.");
        PifLog(MODULE_TTL_TFL_BACKUP, LOG_ERROR_TTL_BACKUP_MASTER);
        PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(pSend->F.sError));
        return (pSend->F.sError);
    }

    switch(pRecv->uchFileType) {            /* What file type ? */

    case    TTL_BAK_BASE:                   /* base file, Fin, Hourly */
        *pusRcvLen = TtlResBackEach(pSend, usMaxLen, hsTtlBaseHandle);
        break;

    case    TTL_BAK_DEPT:                   /* dept total file */
        *pusRcvLen = TtlResBackEach(pSend, usMaxLen, hsTtlDeptHandle);
        break;

    /* ===== ### MOD 2172 Rel1.0 (01/25/00) ===== */
    case    TTL_BAK_PLU:                    /* PLU total file */
        pSend->F.sFileSubType = pRecv->sFileSubType; 
        *pusRcvLen = TtlResBackEachPlu(pSend, usMaxLen);
        break;

    case    TTL_BAK_CPN:                    /* coupon total file, R3.0 */
        *pusRcvLen = TtlResBackEach(pSend, usMaxLen, hsTtlCpnHandle);
        break;

    case    TTL_BAK_CASHIER:                /* cashier total file */
        *pusRcvLen = TtlResBackEach(pSend, usMaxLen, hsTtlCasHandle);
        break;

    /* ===== New Totals (Release 3.1) BEGIN ===== */
    case    TTL_BAK_SERTIME:                /* service time file, R3.1 */
        *pusRcvLen = TtlResBackEach(pSend, usMaxLen, hsTtlSerTimeHandle);
        break;

    case    TTL_BAK_INDFIN:                 /* individual financial, R3.1 */
        *pusRcvLen = TtlResBackEach(pSend, usMaxLen, hsTtlIndFinHandle);
        break;
    /* ===== New Totals (Release 3.1) END ===== */

    default:                                /* illegal specfication */
		NHPOS_ASSERT_TEXT(0, "TtlRespBackup(): pRecv->uchFileType illegal file type.");
        pSend->F.sError = TTL_FAIL;
        *pusRcvLen = 0;
        break;
    }
    return (pSend->F.sError);
}

/*
*============================================================================
**Synopsis:     SHORT  TtlResBackEach(TTLBACKUP_RES *pSend,
*                                      USHORT usMaxLen, SHORT hsHandle);
*
*    Input:     usMaxLen    - max buffer length can be read
*    Input:     hsHandle    - each file handle
*   Output:     pSend       - send buffer address
*
** Return:      size of send data
*
** Description  This function reads data from base total file.
*
*============================================================================
*/
USHORT  TtlResBackEach(TTLBACKUP_RES *pSend, USHORT usMaxLen, SHORT hsHandle)
{
    ULONG           ulFileSize;
    USHORT          usFileSize;
    SHORT           sError;
                                        /* case of BASE total file */
	if (hsHandle < 0) {
		char  xBuff[128];
		sprintf(xBuff, "TtlResBackEach(): uchFileType %d, hsHandle = %d", pSend->F.uchFileType, hsHandle);
		NHPOS_ASSERT_TEXT((hsHandle >= 0), xBuff);
	}

    if (TTL_BAK_BASE == pSend->F.uchFileType) {  
        if (hsHandle < 0) {             /* Handle Error */
            pSend->F.sError = TTL_FAIL;
            return (0);
        }
        ulFileSize = (ULONG)TTL_BASE_SIZE;
    /************ check service time file size (4/9/96) **************/
    } else if (TTL_BAK_SERTIME == pSend->F.uchFileType) {
        if (hsHandle < 0) {             /* Handle Error */
            pSend->F.sError = TTL_SUCCESS;
            pSend->F.uchEOF = 1;
            pSend->D.usDataSize = 0;
            return (sizeof(TTLBACKUP_RESFIX) + 2);
        } 
        ulFileSize = (ULONG)(TTL_SERTTL_SIZE * 4);
    /************ check service time file size (4/9/96) **************/
    } else {                            /* case of DEPT,PLU,CAS,WAI file */
        if (hsHandle < 0) {             /* Handle Error */
            pSend->F.sError = TTL_SUCCESS;
            pSend->F.uchEOF = 1;
            pSend->D.usDataSize = 0;
            return (sizeof(TTLBACKUP_RESFIX) + 2);
        } 
                                        /* read first 4 bytes for waiter */
        if ((TTL_BAK_WAITER  == pSend->F.uchFileType) ||
            (TTL_BAK_DEPT     == pSend->F.uchFileType) ||    /* 2172 */
            (TTL_BAK_PLU     == pSend->F.uchFileType) ||    /* R3.0 */
            (TTL_BAK_CASHIER == pSend->F.uchFileType)) {    /* R3.1 */
            sError = TtlReadFile(hsHandle, 0L, &ulFileSize, 4);
        } else {                        /* read first 2 bytes for other */
            sError = TtlReadFile(hsHandle, 0L, &usFileSize, 2);
            ulFileSize = (ULONG)usFileSize;
        }
        if (TTL_SUCCESS != sError) {
			char  xBuff[128];
			sprintf(xBuff, "TtlResBackEach(): uchFileType %d, sError = %d", pSend->F.uchFileType, sError);
			NHPOS_ASSERT_TEXT((TTL_SUCCESS == sError), xBuff);
            pSend->F.sError = TTL_FAIL;
            return (0);
        }
    }
                                        /* How many bytes should be read */
    if ((pSend->F.ulOffSet + usMaxLen) < ulFileSize) {
        pSend->F.uchEOF = 0;            /* still exist data */
        pSend->D.usDataSize = usMaxLen;
    } else {
        pSend->F.uchEOF = 1;            /* all data will send */
        pSend->D.usDataSize = (USHORT)(ulFileSize - pSend->F.ulOffSet);
    }
                                        /* read data from file */
    pSend->F.sError = TtlReadFile(hsHandle, pSend->F.ulOffSet, pSend->D.auchData, pSend->D.usDataSize);

    if (TTL_SUCCESS != pSend->F.sError) {
		char  xBuff[128];
		sprintf(xBuff, "TtlResBackEach(): uchFileType %d, sError = %d, ulOffset %d, ulFileSize %d", pSend->F.uchFileType, pSend->F.sError, pSend->F.ulOffSet, ulFileSize);
		NHPOS_ASSERT_TEXT((TTL_SUCCESS == pSend->F.sError), xBuff);
        return (0);                     /* read error detected */
    }
                                        /* read successful */
    return (sizeof(TTLBACKUP_RESFIX) + 2 + pSend->D.usDataSize);
}
/*===== END OF SOURCE =====*/
