/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server STUB, Backup Copy Source File                        
* Category    : Client/Server STUB, US Hospitality Model
* Program Name: CSSTBBAK.C                                            
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*          CliReqBackUp()       C   Back Up Copy request from UI
*          SerStartBackUp()     C   Back Up Copy request from super
*          SerChangeInqStat()       Non-Back Up request from super
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial
* Feb-05-93:M.YAMAMOTO : Add KpsCheckPrint()
* Jun-07-93:H.Yamaguchi: Adds ETK backup
* Mar-31-94:K.You      : add flex GC file feature.(mod. CliReqBackUp)                                    
* Nov-30-95:T.Nakahata : R3.1 Initial (add Programmable Report)
* Aug-11-99:M.Teraki   : R3.5 remove WAITER_MODEL
* Feb-14-00:hrkato     : Saratoga
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
#include	<math.h>

#include    "ecr.h"
#include    "pif.h"
#include    "cscas.h"
#include    "csgcs.h"
#include    "csttl.h"
#include    "csop.h"
#include    "nb.h"
#include    "csstbstb.h"
#include    "csstbfcc.h"
#include    "csstbbak.h"
#include    "csserm.h"
#include    "cskp.h"
#include    "csetk.h"
#include    "csprgrpt.h"
#include    "csstubin.h"
#include    "appllog.h"

/*
*===========================================================================
** Synopsis:    SHORT   CliReqBackUp(VOID);
*
** Return:      STUB_SUCCESS:   back up success
*               other:          error
*
** Description:  This function supports Back Up Copy Function.
*                CliReqBackUp() supports functionality for SER_SPESTS_AC42 status.
*                CliReqBackUp() triggered by Notice Board flag NB_EXECUTEBKCOPY.
*===========================================================================
*/
SHORT   CliReqBackUp(VOID)
{
    SHORT           sError;

	NHPOS_NONASSERT_TEXT("CliReqBackUp(): Begin Backup Copy");

    /* wait for total update is done completely, 06/05/01 */
    while(TTL_SUCCESS != TtlReadWrtChk()) { /* total still exist */
	    CstSleep();
    }
    /**
    if (TTL_SUCCESS != TtlReadWrtChk()) {
        return TTL_FAIL;
    }**/

    if (OP_PERFORM != (sError = OpReqBackUp(0))) {
		PifLog(MODULE_OP_BACKUP, LOG_ERROR_TTL_BACKUP_FILE);
		PifLog(MODULE_ERROR_NO(MODULE_OP_BACKUP), (USHORT)abs(sError));
		{
			char xBuff[128];
			sprintf(xBuff,"  OpReqBackUp(): sError = %d - End Backup Copy", sError);
			NHPOS_ASSERT_TEXT((sError == OP_PERFORM), xBuff);
		}
        return sError;
    }
    if (CAS_PERFORM != (sError = CasSendFile())) {
		PifLog(MODULE_CASHIER, LOG_ERROR_TTL_BACKUP_FILE);
		PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(sError));
		{
			char xBuff[128];
			sprintf(xBuff,"  CasSendFile(): sError = %d - End Backup Copy", sError);
			NHPOS_ASSERT_TEXT((sError == OP_PERFORM), xBuff);
		}
        return sError;
    }
    if (TTL_SUCCESS != (sError = TtlReqBackup())) {
		PifLog(MODULE_TTL_TFL, LOG_ERROR_TTL_BACKUP_FILE);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sError));
		{
			char xBuff[128];
			sprintf(xBuff,"  TtlReqBackup(): sError = %d - End Backup Copy", sError);
			NHPOS_ASSERT_TEXT((sError == OP_PERFORM), xBuff);
		}
        return sError;
    }
    if (GCF_SUCCESS != (sError = GusReqBackUpFH(hsSerTmpFile))) {
		PifLog(MODULE_GCF, LOG_ERROR_TTL_BACKUP_FILE);
		PifLog(MODULE_ERROR_NO(MODULE_GCF), (USHORT)abs(sError));
		{
			char xBuff[128];
			sprintf(xBuff,"  GusReqBackUpFH(): sError = %d - End Backup Copy", sError);
			NHPOS_ASSERT_TEXT((sError == OP_PERFORM), xBuff);
		}
        return sError;
    }
    if (ETK_SUCCESS != (sError = CliReqBackupETK(CLI_FCBAKETK, CLI_BAK_ETK_PARAM_FILE))) {
		PifLog(MODULE_ETK, CLI_BAK_ETK_PARAM_FILE);
		PifLog(MODULE_ERROR_NO(MODULE_ETK), (USHORT)abs(sError));
		{
			char xBuff[128];
			sprintf(xBuff,"  CliReqBackupETK(): sError = %d - End Backup Copy", sError);
			NHPOS_ASSERT_TEXT((sError == OP_PERFORM), xBuff);
		}
        return sError;
    }
    if (ETK_SUCCESS != (sError = CliReqBackupETK(CLI_FCBAKETK, CLI_BAK_ETK_TOTAL_FILE))) {
		PifLog(MODULE_ETK, CLI_BAK_ETK_TOTAL_FILE);
		PifLog(MODULE_ERROR_NO(MODULE_ETK), (USHORT)abs(sError));
		{
			char xBuff[128];
			sprintf(xBuff,"  CliReqBackupETK(CLI_FCBAKETK): sError = %d - End Backup Copy", sError);
			NHPOS_ASSERT_TEXT((sError == OP_PERFORM), xBuff);
		}
        return sError;
    }
    if (STUB_SUCCESS != (sError = CliReqBackupMMM(FILE_PLU, CLI_FCBAKPLU))) {       /* Saratoga */
		PifLog(MODULE_OP_PLU, FILE_PLU);
		PifLog(MODULE_ERROR_NO(MODULE_OP_PLU), (USHORT)abs(sError));
		{
			char xBuff[128];
			sprintf(xBuff,"  CliReqBackupMMM(FILE_PLU): sError = %d - End Backup Copy", sError);
			NHPOS_ASSERT_TEXT((sError == OP_PERFORM), xBuff);
		}
        return(CliConvertErrorMMM(sError));
    }
    if (STUB_SUCCESS != (sError = CliReqBackupMMM(FILE_PLU_INDEX, CLI_FCBAKPLU))) { /* Saratoga */
		PifLog(MODULE_OP_PLU, FILE_PLU_INDEX);
		PifLog(MODULE_ERROR_NO(MODULE_OP_PLU), (USHORT)abs(sError));
		{
			char xBuff[128];
			sprintf(xBuff,"  CliReqBackupMMM(FILE_PLU_INDEX): sError = %d - End Backup Copy", sError);
			NHPOS_ASSERT_TEXT((sError == OP_PERFORM), xBuff);
		}
        return(CliConvertErrorMMM(sError));
    }

    /* === Add New Function - Programmable Report (Release 3.1) BEGIN === */
    if (PROGRPT_SUCCESS != (sError = ProgRptSendFile())) {
		PifLog(MODULE_PROGRPT, LOG_ERROR_TTL_BACKUP_FILE);
		PifLog(MODULE_ERROR_NO(MODULE_PROGRPT), (USHORT)abs(sError));
		{
			char xBuff[128];
			sprintf(xBuff,"  ProgRptSendFile(): sError = %d - End Backup Copy", sError);
			NHPOS_ASSERT_TEXT((sError == OP_PERFORM), xBuff);
		}
        return sError;
    }
    /* === Add New Function - Programmable Report (Release 3.1) END === */
	{
		USHORT          usDataLen = sizeof(CLIREQDATABAK);
		CLIREQDATABAK   ReqData = {0};

		if (STUB_SUCCESS != (sError = SstReqBackUp(CLI_FCBAKFINAL, NULL, 0, (UCHAR *)&ReqData, &usDataLen))) {
			PifLog(MODULE_STB_LOG, 101);
			PifLog(MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)abs(sError));
			{
				char xBuff[128];
				sprintf(xBuff,"  SstReqBackUp(): sError = %d - End Backup Copy", sError);
				NHPOS_ASSERT_TEXT((sError == OP_PERFORM), xBuff);
			}
			return STUB_NOT_MASTER;
		}
		if (sizeof(CLIREQDATABAK) != usDataLen) {
			char xBuff[128];
			sprintf(xBuff,"  SstReqBackUp(): invalid length %d - End Backup Copy", usDataLen);
			NHPOS_ASSERT_TEXT(0, xBuff);
			PifLog(MODULE_STB_LOG, 102);
			return STUB_NOT_MASTER;
		}
	    
		SstChangeInqCurTraNo(ReqData.ausTransNo);   /* write transaction # */
		SstChangeInqPreTraNo(ReqData.ausPreTranNo); /* write transaction # */
		SstChangeInqBcasInf(ReqData.fsBcasInf);     /* write Bcast Inf. */
	}

    if (CLI_IAM_MASTER) {
        SstSetFlag(SER_SPESTS_FINAL_AC42);
    }

	NHPOS_NONASSERT_TEXT("CliReqBackUp(): End Backup Copy");

    return STUB_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerStartBackUp(VOID);
*
** Return:      STUB_SUCCESS:       back up success
*               STUB_NOT_MASTER:    I amd not as Master
*               STUB_NOT_UPTODATE:  not up to date
*               STUB_TIME_OUT:      fail to back up
*               STUB_BUSY:          not finished broad cast 
*
** Description: This function supports Back Up Copy Function.
*               called from MaintCopyTtl() to check conditions for an AC42 action
*               and to start it going if system state allows.  This function
*               will trigger the broadcast of a NB_EXECUTEBKCOPY message to
*               inform the out of date terminal to request the current version
*               of the database files.  It is up to the out of date terminal to
*               actually request the files.  See function CliReqBackUp() above.
*===========================================================================
*/
SHORT   SerStartBackUp(VOID)
{
    USHORT          fsComStatus;
    SHORT           sError, sGCFError;

    if (CLI_IAM_SATELLITE) {
		PifLog (MODULE_MAINT_LOG_ID, LOG_EVENT_MAINT_BKUP_SATELLITE);
        return STUB_NOT_MASTER;
    }

    fsComStatus = CstComReadStatus();
    if (CLI_IAM_MASTER) {
		SERINQSTATUS    InqData;

        if (0 == (CliNetConfig.fchStatus & CLI_NET_BACKUP)) {
			PifLog (MODULE_MAINT_LOG_ID, LOG_EVENT_MAINT_BKUP_NO_BACKUP);
            return STUB_NO_BACKUP;
        }
        if (0 == (fsComStatus & CLI_STS_M_UPDATE)) {
			PifLog (MODULE_MAINT_LOG_ID, LOG_EVENT_MAINT_BKUP_MAS_NOTUPDATE);
            return STUB_NOT_MASTER;
        } else if (fsComStatus & CLI_STS_BM_UPDATE) {
			PifLog (MODULE_MAINT_LOG_ID, LOG_EVENT_MAINT_BKUP_BM_NOTUPDATE);
            return STUB_NOT_UPTODATE;
        }
        SstReadInqStat(&InqData);
        if (InqData.fsBcasInf & 0x0002) {
			PifLog (MODULE_MAINT_LOG_ID, LOG_EVENT_MAINT_BKUP_INQ_BUSY);
            return STUB_DUR_INQUIRY;
        }
    }
    if (CLI_IAM_BMASTER) {
		SERINQSTATUS    InqData;

        if (0 == (fsComStatus & CLI_STS_BM_UPDATE)) {
			PifLog (MODULE_MAINT_LOG_ID, LOG_EVENT_MAINT_BKUP_BM_NOTUPDATE);
            return STUB_NOT_MASTER;
        } else if (fsComStatus & CLI_STS_M_UPDATE) {
			PifLog (MODULE_MAINT_LOG_ID, LOG_EVENT_MAINT_BKUP_MAS_NOTUPDATE);
            return STUB_NOT_UPTODATE;
        }
        SstReadInqStat(&InqData);
        if (InqData.fsBcasInf & 0x0001) {
			PifLog (MODULE_MAINT_LOG_ID, LOG_EVENT_MAINT_BKUP_INQ_BUSY);
            return STUB_DUR_INQUIRY;
        }
    }

	//  Check to see if the kitchen printer is in the middle of a print operation.
	//  If so, indicated by KpsCheckPrint () returning KPS_PRINTER_BUSY, then we
	//  give the kitchen printer 20 seconds to complete its printing.
    for (fsComStatus = 0; ; ) {
        if (KPS_PERFORM == KpsCheckPrint()) {
            break;
        }
        PifSleep(1000);      /* Delay 1 seconds */
        fsComStatus++ ;
        if ( fsComStatus > 20 ) {
			PifLog (MODULE_MAINT_LOG_ID, LOG_EVENT_MAINT_BKUP_KPS_BUSY);
            return STUB_KPS_BUSY;    /* wait for 20 seconds */
        }
    }

    sGCFError = GusAllLock();
    if ((GCF_SUCCESS != sGCFError) && (GCF_FILE_NOT_FOUND != sGCFError) && (GCF_LOCKED != sGCFError))
	{
		char  xBuff[128];

		sprintf(xBuff, "**WARNING: SerStartBackUp() - GusAllLock(): returned %d", sGCFError);
		NHPOS_ASSERT_TEXT((sGCFError == STUB_SUCCESS), xBuff);
		PifLog (MODULE_MAINT_LOG_ID, LOG_EVENT_MAINT_BKUP_GCF_LOCK_FAIL);
		PifLog (MODULE_ERROR_NO(MODULE_MAINT_LOG_ID), (USHORT) abs(sGCFError));
        return STUB_BUSY;               
    }

    sError = TtlLock();
	NHPOS_ASSERT_TEXT((sError == TTL_SUCCESS), "==NOTE: SerStartBackUp() - TtlLock() already TTL_LOCKED.");

	// Inform SerESRBackUp () in SERVESR.C in Server subsystem that it is
	// time to do an AC42 synchronization action and wait for the action
	// to complete.
    sError = SstSetFlagWaitDone(SER_SPESTS_AC42, husCliWaitDone);
	if (sError != STUB_SUCCESS) {
		char  xBuff[128];

		sprintf(xBuff, "**WARNING: SerStartBackUp() - SstSetFlagWaitDone(): returned %d", sError);
		NHPOS_ASSERT_TEXT((sError == STUB_SUCCESS), xBuff);
	}

    TtlUnlock();

    if (GCF_SUCCESS == sGCFError) {
        GusAllUnLock();
    }

    return sError;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerChangeInqStat(VOID);
*   
** Return:      STUB_SUCCESS:       A/C 85 success
*               STUB_NOT_MASTER:    error
*
** Description: This function supports Non-Back Up Start Function.
*===========================================================================
*/

SHORT   SerForceInqStatus (VOID)
{
    if (CLI_IAM_MASTER)
	{
		NbSetMTOnline();
		NbSetMTUpToDate();
		NbResetBMOnline();
		NbResetBMUpToDate();
    } else if (CLI_IAM_BMASTER)
	{
		NbSetBMOnline();
		NbSetBMUpToDate();
		NbResetMTOnline();
		NbResetMTUpToDate();
	}

	SstChangeInqStat(SER_INQ_M_UPDATE | SER_INQ_BM_UPDATE);

	return STUB_SUCCESS;
}

SHORT   SerChangeInqStat(VOID)
{
    USHORT  fsComStatus = CstComReadStatus();
    SHORT   sRetCode = STUB_NOT_MASTER;

    if (CLI_IAM_MASTER) {
		if (fsComStatus & CLI_STS_M_UPDATE) {
			if (CliNetConfig.fchStatus & CLI_NET_BACKUP) {
				sRetCode = STUB_SUCCESS;
			}
			else
			{
				PifLog (MODULE_MAINT_LOG_ID, LOG_EVENT_MAINT_BKUP_NO_BACKUP);
				sRetCode = STUB_NO_BACKUP;
			}
		}
		else {
			PifLog (MODULE_MAINT_LOG_ID, LOG_EVENT_MAINT_BKUP_MAS_NOTUPDATE);
			sRetCode = STUB_NOT_UPTODATE;
		}
    } else if (CLI_IAM_BMASTER) {
		if (fsComStatus & CLI_STS_BM_UPDATE) {
			sRetCode = STUB_SUCCESS;
		}
		else {
			PifLog (MODULE_MAINT_LOG_ID, LOG_EVENT_MAINT_BKUP_BM_NOTUPDATE);
			sRetCode = STUB_NOT_UPTODATE;
		}
    } else {
		PifLog (MODULE_MAINT_LOG_ID, LOG_EVENT_MAINT_BKUP_SATELLITE);
		NHPOS_ASSERT(!"Neither Master or BMaster");
	}

    if (STUB_SUCCESS == sRetCode) {
        sRetCode = SstSetFlagWaitDone(SER_SPESTS_AC85, husCliWaitDone);
		if (sRetCode != STUB_SUCCESS) {
			char  xBuff[128];

			sprintf(xBuff, "**WARNING: SerChangeInqStat() - SstSetFlagWaitDone(): returned %d", sRetCode);
			NHPOS_ASSERT_TEXT((sRetCode == STUB_SUCCESS), xBuff);
		}
    }
    return sRetCode;
}

/*===== END OF SOURCE =====*/