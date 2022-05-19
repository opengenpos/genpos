/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2172     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTAL, Total Update Module
* Category    : TOTAL, NCR 2170 US Hospitality Application
* Program Name: Totaltum.c
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:
*   VOID  TtlTUMShutDown()          - total data backup (blocked operation)
*   VOID  TtlTUMRestoreTtlData()    - total data restore (ditto)
*   VOID  TtlTUMTryToUpdateBackup() - total data backup (asyncronous operation)
*
* --------------------------------------------------------------------------
* Update Histories
*    Data   : Ver.Rev.:   Name    : Description
* Jun-07-00 : 01.00.00: m.teraki  : Saratoga initial
* Jul-04-00 : 01.00.02: m.teraki  : added error/warning processing
*
** GenPOS **
*
* Aug-29-17 : 02.02.02 : R.Chambers : replaced PARAFLEXMEM with RflGetPtdFlagByType(FLEX_PLU_ADR).
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
#include	<stdio.h>

#include <ecr.h>
#include <pif.h>
#include <csttl.h>
#include <rfl.h>
#include <ttl.h>
#include <appllog.h>
#include <csstbstb.h>
#include <uie.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>


/*
*===========================================================================
*   Extern Declarations
*===========================================================================
*/

extern TCHAR FARCONST szTtlPluPtdCurFileName[];   /* Base Total File Name */
extern TCHAR FARCONST szTtlPluPtdSavFileName[];   /* Base Total File Name */

/*
*===========================================================================
*   Local common valiables
*===========================================================================
*/
BOOL bAlreadyLowStorageLogIssued = FALSE;
BOOL bAlreadyNoStorageLogIssued  = FALSE;
BOOL bAlreadyNoMemoryLogIssued   = FALSE;

/*
*============================================================================
**Synopsis:     VOID   TtlTUMShutDown(VOID);
*
*    Input:     NONE
*   Output:     NONE
*    InOut:     NONE
**  Return:     NONE
** Description  Create backup of totaldata and blocks until copy is
*               completed or stopped.
*               Total data file must be closed.
*============================================================================
*/
VOID TtlTUMShutDown(VOID)
{
    SHORT sRet;
    extern PifSemHandle usTtlSemPowerDown;

    sRet = CliIsMasterOrBMaster();
    if (sRet == 0) {
        return;
    }

    PifRequestSem(usTtlSemPowerDown);   /* stop total update thread */

    /* wait for current copy job (if working) */
    sRet = TtlPLUGetBackupStatus();
    while (sRet == TTL_BK_STAT_BUSY) {
        PifSleep(1000);
        sRet = TtlPLUGetBackupStatus();
    }

    /* if any kind of error has occured do nothing anymore */
    if (sRet == TTL_FAIL) return;

    /* last update may canceled, so create newest backup copy */
    TtlPLUCloseDB();
    sRet = TtlPLUStartBackup();
    if (sRet == TTL_BK_DENIED_NOHEAP) {
        if (bAlreadyNoMemoryLogIssued == FALSE) {
			PifLog(MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_CODE_01);
            PifLog(MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_BACKUP_NOMEMORY);
            bAlreadyNoMemoryLogIssued = TRUE;
        }
    }
    else if (sRet == TTL_BK_DENIED_NOSTORAGE) {
        if (bAlreadyNoStorageLogIssued == FALSE) {
			PifLog(MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_CODE_01);
            PifLog(MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_BACKUP_NOSTORAGE);
            bAlreadyNoStorageLogIssued = TRUE;
        }
    }
    else if (sRet == TTL_BK_ACCEPTED_LOWSTORAGE) {
        if (bAlreadyLowStorageLogIssued == FALSE) {
			PifLog(MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_CODE_01);
            PifLog(MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_BACKUP_LOWSTORAGE);
            bAlreadyLowStorageLogIssued = TRUE;
        }
    }
    else if (sRet != TTL_BK_ACCEPTED_CLEARLY) {
        ;
    }

    /* wait for copy completes */
    sRet = TtlPLUGetBackupStatus();
    while (sRet == TTL_BK_STAT_BUSY) {
        PifSleep(1000);
        sRet = TtlPLUGetBackupStatus();
    }
}

/*
*============================================================================
**Synopsis:     VOID   TtlTUMResume(VOID);
*
*    Input:     NONE
*   Output:     NONE
*    InOut:     NONE
**  Return:     NONE
** Description  Called when system is being resumed from suspend mode.
*               open DB on ram.(DB bust be closed in TtlTUMShutDown()).
*============================================================================
*/
VOID TtlTUMResume(VOID)
{
    SHORT sRet;
    extern USHORT usTtlSemPowerDown;

    /* ignore if terminal is master/b-master */
    sRet = CliIsMasterOrBMaster();
    if (sRet == 0) {
        return;
    }

    /* open db */
	
	//Memory Leak 7.8.05
    //sRet = TtlPLUOpenDB();

    if (sRet == TTL_FAIL) {
        TtlAbort(MODULE_TTL_TUM_UPDATE, TTL_FILE_OPEN_ERR);
    }

    PifReleaseSem(usTtlSemPowerDown);   /* stop total update thread */
}

/*
*============================================================================
**Synopsis:     VOID   TtlTUMRestoreTtlData(VOID);
*
*    Input:     NONE
*   Output:     NONE
*    InOut:     NONE
**  Return:     NONE
** Description  Restore total data from backup to work area and  blocks until
*               copy is completed or stopped.
*               Total data file must be closed.
*============================================================================
*/
VOID TtlTUMRestoreTtlData(VOID)
{
    SHORT sRet;

    sRet = CliIsMasterOrBMaster();
    if (sRet == 0) {
        return;
    }

	/* delete ptd file by option, from the upgrade from previous version, 09/05/01 */
	if(RflGetPtdFlagByType(FLEX_PLU_ADR) == 0){
        PifDeleteFile(szTtlPluPtdCurFileName);	/* delete current ptd file on DOC */
        PifDeleteFile(szTtlPluPtdSavFileName);	/* delete saved ptd file on DOC */
	}

    /* last update may canceled, create newest backup */
    sRet = TtlPLUStartRestore();
    if ((sRet == TTL_BK_DENIED_NOHEAP) || (sRet == TTL_BK_DENIED_NOSTORAGE)) {
        TtlAbort(MODULE_TTL_TUM_UPDATE, sRet);
        return;
    }

	NHPOS_NONASSERT_NOTE("==STATE", "==STATE: TtlTUMRestoreTtlData() polling backup status.");
    /* wait for copy completes */
    sRet = TtlPLUGetBackupStatus();
    while (sRet == TTL_BK_STAT_BUSY) {
        PifSleep(1000);
        sRet = TtlPLUGetBackupStatus();
    }
	{
		char xBuff[128];
		sprintf (xBuff, "==STATE: TtlTUMRestoreTtlData() polling complete %d", sRet);
		NHPOS_NONASSERT_NOTE("==STATE", xBuff);
	}
}

/*
*============================================================================
**Synopsis:     VOID   TtlTUMTryToUpdateBackup(VOID);
*
*    Input:     NONE
*   Output:     NONE
*    InOut:     NONE
**  Return:     NONE
** Description  This function tries to update total file in a persistent storage
*               (Usually, total file is used on temporary(volatile) filesystem.)
*============================================================================
*/
VOID TtlTUMTryToUpdateBackup(VOID)
{
    SHORT sRet; /* return value from subproc. */

    /* do nothing if terminal is woring as master/b-master */
    sRet = CliIsMasterOrBMaster();
    if (sRet == 0) {
        return;
    }

    /* if previous copy is still working, do nothing */
    sRet = TTL_FAIL;
    sRet = TtlPLUGetBackupStatus();
    if (sRet != TTL_BK_STAT_READY) {
        return;
    }

    PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */
	sRet = TtlPLUGetGetRecStatus();			/* sequential PLU total access is executed by another thread */
	if (sRet == TRUE) {
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
		return;
	}

    /* close DB here */
	//Memory Leak 7.8.05
    //sRet = TtlPLUCloseDB();

    if (sRet == TTL_FAIL) {
        ;
    }

    /* issue trigger of copy start */
    sRet = TtlPLUStartBackup();
    if (sRet == TTL_BK_DENIED_NOHEAP) {
        if (bAlreadyNoMemoryLogIssued == FALSE) {
			PifLog(MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_CODE_02);
            PifLog(MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_BACKUP_NOMEMORY);
            bAlreadyNoMemoryLogIssued = TRUE;
        }
        UieErrorMsg(LDT_NOMEMORY_ADR, UIE_WITHOUT);
    }
    else if (sRet == TTL_BK_DENIED_NOSTORAGE) {
        if (bAlreadyNoStorageLogIssued == FALSE) {
			PifLog(MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_CODE_02);
            PifLog(MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_BACKUP_NOSTORAGE);
            bAlreadyNoStorageLogIssued = TRUE;
        }
        UieErrorMsg(LDT_NOSTORAGE_ADR, UIE_WITHOUT);
    }
    else if (sRet == TTL_BK_ACCEPTED_LOWSTORAGE) {
        if (bAlreadyLowStorageLogIssued == FALSE) {
			PifLog(MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_CODE_02);
            PifLog(MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_BACKUP_LOWSTORAGE);
            bAlreadyLowStorageLogIssued = TRUE;
        }
        UieErrorMsg(LDT_LOWSTORAGE_ADR, UIE_WITHOUT);
    }
    else if (sRet != TTL_BK_ACCEPTED_CLEARLY) {
        ;
    }

    /* re-open DB */
	//Memory Leak 7.8.05
    //sRet = TtlPLUOpenDB();

    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */

    if (sRet == TTL_FAIL) {
        TtlAbort(MODULE_TTL_TUM_UPDATE, TTL_FILE_OPEN_ERR);
    }
}
#if 0   /* used only for test START */
{
    static bInitialized = FALSE;
    TBC_CMD cmd;
    LONG    lRet;
    if (bInitialized == FALSE) {
        cmd.dwCmdId = TBC_CMDID_INIT;
        lRet = TbcCommand(&cmd);
        DebugPrintf("TbcCommand() initialized dwResult = %d\n", cmd.dwResult);
        bInitialized = TRUE;
    }
    else {
        wchar_t szSrc1[] = L"\\just1m.001";
        wchar_t szDst1[] = L"\\flashdisk\\just1m.001";
        wchar_t szSrc2[] = L"\\just1m.002";
        wchar_t szDst2[] = L"\\flashdisk\\just1m.002";
        wchar_t *aszData[4] = {szSrc1, szDst1, szSrc2, szDst2};
        cmd.dwCmdId = TBC_CMDID_STARTCOPY;
        cmd.dwLen = 4 * sizeof(wchar_t *);
        cmd.pvData = aszData;
        lRet = TbcCommand(&cmd);
        DebugPrintf("TbcCommand() returned %d, dwResult = %d\n", lRet, cmd.dwResult);
    }
}
#endif   /* used only for test END */

/* ===== End of File (TtlBkup.C) ===== */
