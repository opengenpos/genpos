/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : SERVER module, Execute Special Request Source File                        
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVESR.C                                            
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           SerESRTimeOutBackUp();   C  Time out check during back up
*           SerESRTimeOutMulti();    C  Time Out check during multi. send/rec
*           SerESRTimeOutKps();      C  Time Out check during Kitcin Print
*           SerESRTimeOutBcas();     C  Time Out check during Broadcasting
*           SerESRChgBMStat();       C  Change BM to Out of date
*           SerESRChgMTStat();       C  Change MT to Out of date
*           SerESRBackUpFinal();     C  Finalize of A/C 42
*           SerESRNonBKSys();        C  Request A/C 85
*              SerESRAC85onMT();               on MT
*              SerESRAC85onBM();               on BM
*           SerESRBackUp();          C  Request A/C 42
*              SerESRAC42onMT();               on MT
*              SerESRAC42onBM();               on BM
*           SerESRBMDown(VOID);         Check BM off-line => Out of Date
*           SerESRTimeOutSps();      A  Time Out check during Shared Print
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* Jun-05-93:H.Yamaguchi: Modified Enhance
*          :           :                                    
* Aug-04-00:M.Teraki   : change num. of kps timer (1 -> No.of KP)
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
#include    <memory.h>
#include    <stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <nb.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <appllog.h>
#include    "servmin.h"
#include    <csstbkps.h>        /* V1.0.0.4 */

/*
*===========================================================================
** Synopsis:    VOID    SerESRTimeOutBackUp(VOID);
*
** Return:      none.
*
** Description: This function checks time out during back up copy.
*===========================================================================
*/
VOID    SerESRTimeOutBackUp(VOID)
{
    if (SER_IAM_SATELLITE) {
        return;
    }

    if (SER_STBACKUP == usSerStatus) {
        if (SERV_TIMER_EXPIRED == SerTimerRead(CLI_MDC_BACKUP)) {
			NHPOS_NONASSERT_TEXT("SERV_TIMER_EXPIRED == SerTimerRead(CLI_MDC_BACKUP)");
            SerChangeStatus(SER_STNORMAL);
            sSerExeError = STUB_TIME_OUT;
            SstResetFlag(SER_SPESTS_AC42);
			if (usSerCliSem != PIF_SEM_INVALID_HANDLE)
				PifReleaseSem(usSerCliSem);
			usSerCliSem = PIF_SEM_INVALID_HANDLE;       // reset this temporary semaphore variable waiting for next time it is used
            SerSendNtbStart();
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerESRTimeOutMulti(VOID);
*
** Return:      none.
*
** Description:
*   This function checks time out during multi. sending/ receeiving.
*===========================================================================
*/
VOID    SerESRTimeOutMulti(VOID)
{
    if (SER_IAM_SATELLITE) {
        return;
    }
    if ((SER_STMULSND == usSerStatus) || (SER_STMULRCV == usSerStatus)) {
        if (SERV_TIMER_EXPIRED == SerTimerRead(CLI_MDC_MULTI)) {
			NHPOS_NONASSERT_TEXT("SERV_TIMER_EXPIRED == SerTimerRead(CLI_MDC_MULTI)");
            SerChangeStatus(SER_STNORMAL);
            SerChekGCFUnLock();
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerESRTimeOutKps(VOID);
*
** Return:      none.
*
** Description: This function checks time out during Kitchin Printer
*===========================================================================
*/
VOID    SerESRTimeOutKps(VOID)  /* changed V1.0.0.4 */
{
    UCHAR uchKpNo;

    if (SER_IAM_SATELLITE) {
        return;
    }

    for (uchKpNo = 0; uchKpNo < SER_MAX_KP; uchKpNo++) {
        /* if least one timer has expired, stop all timer */
        if (SERV_TIMER_EXPIRED == SerKpsTimerRead(uchKpNo)) {
            SerChekKpsUnLock();              /* Kitcin Printer UnLock */
            SerKpsTimerStopAll();            /* Kps timer stop */
            break;
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerESRTimeOutBcas(VOID);
*
** Return:      none.
*
** Description: This function checks time out during Broadcasting
*===========================================================================
*/
VOID    SerESRTimeOutBcas(VOID)
{
    USHORT  fsSpeStatus;

    if (SER_IAM_SATELLITE) {
        return;
    }

    SstReadFlag(&fsSpeStatus);
    if (fsSpeStatus & SER_SPESTS_BEGIN_BCAS) {
        SerBcasTimerStart();
        SstResetFlag(SER_SPESTS_BEGIN_BCAS);
    } else if (fsSpeStatus & SER_SPESTS_BCAS) {
        if (SERV_TIMER_EXPIRED == SerBcasTimerRead()) {
            sSerExeError = STUB_SUCCESS;
            SstResetFlag(SER_SPESTS_BCAS);
			if (usSerCliSem != PIF_SEM_INVALID_HANDLE)
				PifReleaseSem(usSerCliSem);      // allows SstSetFlagWaitDone() to complete
			usSerCliSem = PIF_SEM_INVALID_HANDLE;       // reset this temporary semaphore variable waiting for next time it is used
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerESRChgBMStat(VOID);
*
** Return:      none.
*
** Description: 
*  This function executes special request to make BM Out Of Date.
*===========================================================================
*/
VOID    SerESRChgBMStat(VOID)
{
    USHORT  fsSpeStatus;
    SHORT   sError = STUB_ILLEGAL;

    if (SER_IAM_SATELLITE) {
        return;
    }

    SstReadFlag(&fsSpeStatus);
    if (fsSpeStatus & SER_SPESTS_BMOD) {
        if ((SER_STNORMAL == usSerStatus) || ((SER_STINQUIRY == usSerStatus) && 
             (fsSpeStatus & SER_SPESTS_INQTIMEOUT))) {
            if (SER_IAM_MASTER && (SerComReadStatus() & SER_COMSTS_M_UPDATE)) {
                SstChangeInqStat(SER_INQ_M_UPDATE);
                PifLog(MODULE_SER_LOG, LOG_ERROR_SER_STUB_BMOD);
                SerSendNtbStart();          /* start on-line information */
                SerChangeStatus(SER_STNORMAL);
                sError = STUB_SUCCESS;
            } else {
                sError = STUB_ILLEGAL;
			}
            sSerExeError = sError;  // Set status before releasing the semaphore
            SstResetFlag(SER_SPESTS_BMOD);
			if (usSerCliSem != PIF_SEM_INVALID_HANDLE)
				PifReleaseSem(usSerCliSem);             // if the semaphore has been initialized then release it
			usSerCliSem = PIF_SEM_INVALID_HANDLE;       // reset this temporary semaphore variable waiting for next time it is used
       }
    }
}
    
/*
*===========================================================================
** Synopsis:    VOID    SerESRChgMTStat(VOID);
*
** Return:      none.
*
** Description: 
*  This function executes special request to make MT Out Of Date.
*===========================================================================
*/
VOID    SerESRChgMTStat(VOID)
{
    USHORT  fsSpeStatus;
    SHORT   sError = STUB_ILLEGAL;

    if (SER_IAM_SATELLITE) {
        return;
    }

    SstReadFlag(&fsSpeStatus);
    if (fsSpeStatus & SER_SPESTS_MTOD) {
        if ((SER_STNORMAL == usSerStatus) || ((SER_STINQUIRY == usSerStatus) && (fsSpeStatus & SER_SPESTS_INQTIMEOUT))) {
            if (SER_IAM_BMASTER && (SerComReadStatus() & SER_COMSTS_BM_UPDATE)) {
                SstChangeInqStat(SER_INQ_BM_UPDATE);
                PifLog(MODULE_SER_LOG, LOG_ERROR_SER_STUB_MTOD);
                SerSendNtbStart();          /* start on-line information */
                SerChangeStatus(SER_STNORMAL);
                sError = STUB_SUCCESS;
            } else {
                sError = STUB_NOT_UPTODATE;
			}
            sSerExeError = sError;  // Set status before releasing the semaphore
            SstResetFlag(SER_SPESTS_MTOD);
			if (usSerCliSem != PIF_SEM_INVALID_HANDLE)
				PifReleaseSem(usSerCliSem);             // if the semaphore has been initialized then release it
			usSerCliSem = PIF_SEM_INVALID_HANDLE;       // reset this temporary semaphore variable waiting for next time it is used
        }
    }
}
    
/*
*===========================================================================
** Synopsis:    VOID    SerESRBackUpFinal(VOID);
*
** Return:      none.
*
** Description: This function executes special request to finalize back up.
*===========================================================================
*/
VOID    SerESRBackUpFinal(VOID)
{
    USHORT          fsSpeStatus;
    DATE_TIME       NewPifDate;
    CLIINQDATE      NewInqDate;

    if (SER_IAM_SATELLITE) {
        return;
    }

    SstReadFlag(&fsSpeStatus);
    if (fsSpeStatus & SER_SPESTS_FINAL_AC42) {
        PifGetDateTime(&NewPifDate);
        SerConvertInqDate(&NewPifDate, &NewInqDate);
        SstChangeInqStat(SER_INQ_M_UPDATE | SER_INQ_BM_UPDATE);
        SstChangeInqDate(&NewInqDate);
        SerSendInqDate();
        SstResetFlag(SER_SPESTS_FINAL_AC42);
    }
}   

/*
*===========================================================================
** Synopsis:    VOID    SerESRNonBKSys(VOID);
*
** Return:      none.
*
** Description: This function executes special request to do A/C 85.
*===========================================================================
*/
VOID    SerESRNonBKSys(VOID)
{
    USHORT  fsSpeStatus;

    if (SER_IAM_SATELLITE) {
        return;
    }

    SstReadFlag(&fsSpeStatus);
    if (fsSpeStatus & SER_SPESTS_AC85) {
        if (SER_IAM_MASTER) {
            SerESRAC85onMT();
        } else {
            SerESRAC85onBM();
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerESRAC85onMT(VOID);
*
** Return:      none.
*
** Description: This function executes special request to do A/C 85 on MT.
                We will allow an AC85 under the following conditions:
                    Backup Master Terminal is off-line
                    Backup Master Terminal is not up to date
                In all cases, the server status in usSerStatus must be either
                SER_STINQUIRY or SER_STNORMAL state otherwise we will assume
                the server state is in transition hence we should wait.
                The main server thread will call this function repeatedly until
                the special request execution of SER_SPESTS_AC85 is reset.
*===========================================================================
*/
VOID    SerESRAC85onMT(VOID)
{
    SHORT           sError;
    USHORT          fsComStatus, fsSpeFlag;
    SERINQSTATUS    InqData;

    if (SER_STINQUIRY == usSerStatus) {
        SstReadFlag(&fsSpeFlag);
        SstReadInqStat(&InqData);
        if (fsSpeFlag & SER_SPESTS_INQTIMEOUT) {
            if (InqData.usStatus & SER_INQ_M_UPDATE) {
                sError = STUB_SUCCESS;
            } else if ((0 == InqData.usStatus) &&
                       (0 == InqData.CurDate.usDay)) {
                sError = STUB_SUCCESS;
            } else {
				NHPOS_ASSERT(!" SerESRAC85onMT() InqData.usStatus not proper for AC85");
                sError = STUB_ILLEGAL;
            }
        } else {
			NHPOS_ASSERT(!" SerESRAC85onMT() sError = STUB_DUR_INQUIRY");
            sError = STUB_DUR_INQUIRY;
        }
    } else if (SER_STNORMAL == usSerStatus) {
        fsComStatus = SerComReadStatus();
        if ((fsComStatus & SER_COMSTS_M_UPDATE) &&
            (fsComStatus & SER_COMSTS_BM_UPDATE)) {
            sError = STUB_SUCCESS;
        } else if (fsComStatus & SER_COMSTS_BM_OFFLINE) {
            sError = STUB_SUCCESS;
        } else if (! (fsComStatus & SER_COMSTS_BM_UPDATE) ) {
            sError = STUB_SUCCESS;
        } else {
			NHPOS_ASSERT(!" SerESRAC85onMT() fsComStatus not proper for AC85");
            sError = STUB_ILLEGAL;
        }
    } else {
			NHPOS_ASSERT(!" SerESRAC85onMT() usSerStatus not appropriate");
            return ;
    }

    sSerExeError = sError;
    if (STUB_SUCCESS == sError) {
        SstChangeInqStat(SER_INQ_M_UPDATE);
        PifLog(MODULE_SER_LOG, LOG_ERROR_SER_AC85_BMOD);
        SerSendNtbStart();
        SerChangeStatus(SER_STNORMAL);
    }
    SstResetFlag(SER_SPESTS_AC85);
	if (usSerCliSem != PIF_SEM_INVALID_HANDLE)
		PifReleaseSem(usSerCliSem);
	usSerCliSem = PIF_SEM_INVALID_HANDLE;       // reset this temporary semaphore variable waiting for next time it is used
}

/*
*===========================================================================
** Synopsis:    VOID    SerESRAC85onBM(VOID);
*
** Return:      none.
*
** Description: This function executes special request to do A/C 85 on BM.
                We will allow an AC85 under the following conditions:
                    Master Terminal is off-line
                    Master Terminal is not up to date
                In all cases, the server status in usSerStatus must be either
                SER_STINQUIRY or SER_STNORMAL state otherwise we will assume
                the server state is in transition hence we should wait.
                The main server thread will call this function repeatedly until
                the special request execution of SER_SPESTS_AC85 is reset.
*===========================================================================
*/
VOID    SerESRAC85onBM(VOID)
{
    SHORT           sError;
    USHORT          fsComStatus, fsSpeFlag;
    SERINQSTATUS    InqData;

    if (SER_STINQUIRY == usSerStatus) {
        SstReadFlag(&fsSpeFlag);
        SstReadInqStat(&InqData);
        if (fsSpeFlag & SER_SPESTS_INQTIMEOUT) {
            if (InqData.usStatus & SER_INQ_BM_UPDATE) {
                sError = STUB_SUCCESS;
            } else if ((0 == InqData.usStatus) && (0 == InqData.CurDate.usDay)) {
                sError = STUB_SUCCESS;
            } else {
				char  xBuff[128];
				sprintf_s (xBuff, 128, "**NOTE: InqData.usStatus 0x%x not proper for AC85", InqData.usStatus);
				NHPOS_ASSERT_TEXT((InqData.usStatus & SER_INQ_BM_UPDATE), xBuff);
                sError = STUB_ILLEGAL;
            }
        } else {
			NHPOS_ASSERT_TEXT(0, "    SerESRAC85onBM()  sError = STUB_DUR_INQUIRY");
            sError = STUB_DUR_INQUIRY;
        }
    } else if (SER_STNORMAL == usSerStatus) {
        fsComStatus = SerComReadStatus();
        if ((fsComStatus & SER_COMSTS_BM_UPDATE) && (fsComStatus & SER_COMSTS_M_UPDATE)) {
            sError = STUB_SUCCESS;
        } else if (fsComStatus & SER_COMSTS_M_OFFLINE) {
            sError = STUB_SUCCESS;
        } else if (fsComStatus & SER_COMSTS_BM_OFFLINE) {
            sError = STUB_SUCCESS;
        } else if (! (fsComStatus & SER_COMSTS_M_UPDATE)) {
            sError = STUB_SUCCESS;
        } else {
			char  xBuff[128];
			sprintf_s (xBuff, 128, "**NOTE: fsComStatus 0x%x not proper for AC85", fsComStatus);
			NHPOS_ASSERT_TEXT(0, xBuff);
            sError = STUB_ILLEGAL;
        }
    } else {
		NHPOS_ASSERT(!" SerESRAC85onBM() usSerStatus not appropriate");
        sError = STUB_ILLEGAL;
    }

    sSerExeError = sError;
    if (STUB_SUCCESS == sError) {
        SstChangeInqStat(SER_INQ_BM_UPDATE);
        PifLog(MODULE_SER_LOG, LOG_ERROR_SER_AC85_MTOD);
        SerSendNtbStart();
        SerChangeStatus(SER_STNORMAL);
    }
    SstResetFlag(SER_SPESTS_AC85);
	if (usSerCliSem != PIF_SEM_INVALID_HANDLE)
		PifReleaseSem(usSerCliSem);
	usSerCliSem = PIF_SEM_INVALID_HANDLE;       // reset this temporary semaphore variable waiting for next time it is used
}

/*
*===========================================================================
** Synopsis:    VOID    SerESRBackUp(VOID);
*
** Return:      none.
*
** Description: This function executes special request to do A/C 42.
*===========================================================================
*/
VOID    SerESRBackUp(VOID)
{
    USHORT  fsSpeStatus;

    if (SER_IAM_SATELLITE) {
        return;
    }

    if (SER_STBACKUP != usSerStatus) {
        SstReadFlag(&fsSpeStatus);
        if (fsSpeStatus & SER_SPESTS_AC42) {
            if (SER_IAM_MASTER) {
                SerESRAC42onMT();
            } else {
                SerESRAC42onBM();
            }
        }
    }
}


/*
*===========================================================================
** Synopsis:    VOID    SerESRAC42onMT(VOID);
*
** Return:      none.
*
** Description: This function executes special request to do A/C 42 on MT.
*===========================================================================
*/
VOID    SerESRAC42onMT(VOID)
{
    USHORT  fsComStatus = 0;
    SHORT   sError = STUB_SUCCESS;

    sSerExeError = STUB_SUCCESS;  // default result code to success for SerESRAC42onMT()

    fsComStatus = SerComReadStatus();
    if (SER_STNORMAL == usSerStatus) {
        if (0 == (fsComStatus & SER_COMSTS_M_UPDATE)) {
			PifLog (MODULE_SER_LOG, LOG_EVENT_SER_AC42_M_NOTUPDATE);
            sError = STUB_NOT_MASTER;
        }
        if (fsComStatus & SER_COMSTS_BM_UPDATE) {
			PifLog (MODULE_SER_LOG, LOG_EVENT_SER_AC42_BM_NOTUPDATE);
            sError = STUB_NOT_UPTODATE;
        }
        if (fsComStatus & SER_COMSTS_BM_OFFLINE) {
			PifLog (MODULE_SER_LOG, LOG_EVENT_SER_AC42_BM_OFFLINE);
            sError = STUB_BM_DOWN;
        }
    } else if (SER_STINQUIRY == usSerStatus) {
		PifLog (MODULE_SER_LOG, LOG_EVENT_SER_AC42_M_DUR_INQUIRY);
        sError = STUB_DUR_INQUIRY;
    } else {
		PifLog (MODULE_SER_LOG, LOG_EVENT_SER_AC42_M_ILLEGAL);
		PifLog (MODULE_DATA_VALUE(MODULE_SER_LOG), usSerStatus);
        sError = STUB_ILLEGAL;
    }

	{
		char  xBuff[128];

		sprintf_s (xBuff, 128, "==NOTE: SerESRAC42onMT(): usSerStatus = %d, sError = %d, fsComStatus=0x%x", usSerStatus, sError, fsComStatus);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

    if (STUB_SUCCESS == sError) {
        SerResp.usPrevMsgHLen = 0;
        SerChangeStatus(SER_STBACKUP);
        NbWriteInfFlag(NB_MTBAKCOPY_FLAG, NB_EXECUTEBKCOPY | NB_STOPALLFUN);
    } else {
        sSerExeError = sError;
        SstResetFlag(SER_SPESTS_AC42);
		if (usSerCliSem != PIF_SEM_INVALID_HANDLE)
			PifReleaseSem(usSerCliSem);
		usSerCliSem = PIF_SEM_INVALID_HANDLE;       // reset this temporary semaphore variable waiting for next time it is used
    }    
}

/*
*===========================================================================
** Synopsis:    VOID    SerESRAC42onBM(VOID);
*
** Return:      none.
*
** Description: This function executes special request to do A/C 42 on BM.
*===========================================================================
*/
VOID    SerESRAC42onBM(VOID)
{
    USHORT  fsComStatus = 0;
    SHORT   sError = STUB_SUCCESS;

    sSerExeError = STUB_SUCCESS;  // default result code to success for SerESRAC42onBM()

    fsComStatus = SerComReadStatus();
	if (SER_STNORMAL == usSerStatus) {
        if (0 == (fsComStatus & SER_COMSTS_BM_UPDATE)) {
			PifLog (MODULE_SER_LOG, LOG_EVENT_SER_AC42_BM_NOTUPDATE);
            sError = STUB_NOT_MASTER;
        }
        if (fsComStatus & SER_COMSTS_M_UPDATE) {
			PifLog (MODULE_SER_LOG, LOG_EVENT_SER_AC42_M_NOTUPDATE);
            sError = STUB_NOT_UPTODATE;
        }
        if (fsComStatus & SER_COMSTS_M_OFFLINE) {
			PifLog (MODULE_SER_LOG, LOG_EVENT_SER_AC42_M_OFFLINE);
            sError = STUB_M_DOWN;
        }
    } else if (SER_STINQUIRY == usSerStatus) {
		PifLog (MODULE_SER_LOG, LOG_EVENT_SER_AC42_BM_DUR_INQUIRY);
        sError = STUB_DUR_INQUIRY;
    } else {
		PifLog (MODULE_SER_LOG, LOG_EVENT_SER_AC42_BM_ILLEGAL);
		PifLog (MODULE_DATA_VALUE(MODULE_SER_LOG), usSerStatus);
        sError = STUB_ILLEGAL;
    }

	{
		char  xBuff[128];

		sprintf_s (xBuff, 128, "SerESRAC42onBM(): usSerStatus = %d, sError = %d, fsComStatus = 0x%x", usSerStatus, sError, fsComStatus);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

    if (STUB_SUCCESS == sError) {
        SerResp.usPrevMsgHLen = 0;
        SerChangeStatus(SER_STBACKUP);
        NbWriteInfFlag(NB_BMBAKCOPY_FLAG, NB_EXECUTEBKCOPY | NB_STOPALLFUN);
    } else {
        sSerExeError = sError;
        SstResetFlag(SER_SPESTS_AC42);
		if (usSerCliSem != PIF_SEM_INVALID_HANDLE)
			PifReleaseSem(usSerCliSem);
		usSerCliSem = PIF_SEM_INVALID_HANDLE;       // reset this temporary semaphore variable waiting for next time it is used
    }    
}

/*
*===========================================================================
** Synopsis:    VOID    SerESRBMDown(VOID);
*
** Return:      none.
*
** Description: This function makes BM Out of Date when BM is off-line.
*===========================================================================
*/
VOID    SerESRBMDown(VOID)
{
    USHORT  fsComStatus;

    if (SER_IAM_MASTER) {
        if ((SERV_SUCCESS == SerResp.sError) &&
            (SER_STNORMAL == usSerStatus) &&
            (CLI_TGT_BMASTER != SerRcvBuf.auchFaddr[CLI_POS_UA])) {
            fsComStatus = SerComReadStatus();
            if ((fsComStatus & SER_COMSTS_M_UPDATE) &&
                (fsComStatus & SER_COMSTS_BM_UPDATE) &&
                (fsComStatus & SER_COMSTS_BM_OFFLINE)) {

                SstChangeInqStat(SER_INQ_M_UPDATE);
                PifLog(MODULE_SER_LOG, LOG_ERROR_SER_OFFLINE_BMOD);
                SerSendNtbStart();
            }
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerESRTimeOutSps(VOID);
*
** Return:      none.
*
** Description: This function checks time out during Shared Printer
*===========================================================================
*/
VOID    SerESRTimeOutSps(VOID)
{
    if (SERV_TIMER_EXPIRED == SerSpsTimerRead()) {
        SerClearShrTermLock() ;        /* Unlock Shared printer */
        SerSpsTimerStop();             /* Stop timer */
    }
}

/*===== END OF SOURCE =====*/

