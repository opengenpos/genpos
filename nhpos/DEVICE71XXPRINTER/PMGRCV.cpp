/*
**************************************************************************
**
**  Title:      Print status receive module
**
**  Categoly:   Print Manager
**
**  Abstruct:   Receive status from printer
**
*;========================================================================
*
*   PVCS ENTRY
*
*;========================================================================
*
*$Revision$
*$Date$
*$Author$
*$Log$
*
**************************************************************************
*/

/**
;==============================================================================
;     Rev.   |  Date       |  Comment
;------------------------------------------------------------------------------
;   00.00.00 |             | Initial
;            |             |
;   Saratoga | Apr-11-2000 | Changed length of the character '///.../'.
;            |             | printed in recovery sequence.
;==============================================================================
**/

/**
;=============================================================================
;+                                                                           +
;+                  I N C L U D E     F I L E s                              +
;+                                                                           +
;=============================================================================
**/
#include    "stdafx.h"
#include    <string.h>
#include    <ecr.h>
#include    <pif.h>
//#include    <uie.h>
//#include    <paraequ.h>
#include    <tally.h>
#include    <pmg.h>
#include    "devpmg.h"
#include    "pmgif.h"
#include    "pmgcmn.h"
#include    "pmgrcv.h"
#include    "printio.h"
#include    "escp.h"

extern  PRTCTRL     pPrtCtrl;
#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
extern  SPOOLCNT    pSpoolCtrlR;
extern  SPOOLCNT    pSpoolCtrlJ;
#endif
#if defined (STATION_SLIP)
extern  SPOOLCNT    pSpoolCtrlS;
#endif
extern  USHORT      hPmgWaitSem;            /* handle of semaphore          */
                                            /*  (PmgWait)                   */

/**
;========================================================================
;+                                                                      +
;+            S T A T I C    R A M    A S S I G N M E N T               +
;+                                                                      +
;========================================================================
**/
static USHORT   usJournalEnd;       /* counter of journal paper near end    */
static USHORT   usSlipEnd;          /* counter of slip paper near end       */
static UCHAR    aucSelPrt[] = {ESC, 'c', '0', 0};
static UCHAR    aucSameOff[] = {ESC, 'z', 0};

/*
*********************************************************************
*
**Synopsis:     VOID    PmgReceive(VOID)
*
**Return:       Non
*
**Description:  Receie status from priter (receipt & journal & slip)
*
*********************************************************************
*/
VOID THREADENTRY PmgReceive(VOID)
{
    /*--- counter of journal paper near end ---*/
    usJournalEnd = 0;
    usSlipEnd = 0;

    while (pPrtCtrl.bDoReceive) {
#if defined (STATION_SLIP)
        PmgReceiveS();
#endif
#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
        PmgReceiveRJ();
#endif
        PmgSleep(ID_RECEIVE, WAIT_DATA, 0);
    }
    pPrtCtrl.ucStatReceive |= SPLST_FINALIZED;
}


#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
/*
*********************************************************************
*
**Synopsis:     VOID    PmgReceiveRJ(VOID)
*
**Return:       Non
*
**Description:  Receie status from priter (receipt & journal)
*
*********************************************************************
*/

VOID PmgReceiveRJ(VOID)
{
    SHORT           sRc;
    PRT_RESPONCE    aPrtResponce;

    for (;;) {
        /*--- spool read/write semaphore ---*/
        PmgRequestSemRJ();

        switch(RcvChkSpoolData()) {
        case SPL_DATA:
            PmgReleaseSemRJ();

            /*--- read communication port ---*/
            sRc = PmgReadCom(PMG_PRT_RCT_JNL,
                              &aPrtResponce,
                              sizeof(aPrtResponce));

            if (sRc <= PIF_ERROR_COM_ERRORS) {
                /*--- error recovery ---*/
                RcvErrorRecovery(PMG_PRT_RCT_JNL);
                /*-- count error tally ---*/
                pPrtCtrl.pPrtTally->usCommError++;

            } else if ((sRc == 0) || (sRc <= PIF_ERROR_COM_TIMEOUT)) {
                /*--- error recovery ---*/
                RcvErrorRecovery(PMG_PRT_RCT_JNL);
                /*-- count error tally ---*/
                pPrtCtrl.pPrtTally->usTimeOut++;

            } else if (sRc == PIF_ERROR_COM_POWER_FAILURE) {
                /*--- power down recovry ---*/
                RcvPowerRecovery(PMG_PRT_RCT_JNL);

            } else {
                /*--- check responce data ---*/
                RcvChkResponce(PMG_PRT_RCT_JNL, &aPrtResponce);
            }

            PmgWakeUp(ID_WAIT_RJ);              /* wake up user             */
            PmgWakeUp(ID_SEND);                 /* wake up send thread      */
            break;

        case SPL_ERROR:
            /*--- error recovery ---*/
            PmgReleaseSemRJ();
            RcvErrorRecovery(PMG_PRT_RCT_JNL);
            break;

        case SPL_POWER_FAIL:
            /*--- power down recovry ---*/
            PmgReleaseSemRJ();
            RcvPowerRecovery(PMG_PRT_RCT_JNL);
            break;

        case SPL_ERR_IMP:
            /*--- error in important lines ---*/
            PmgReleaseSemRJ();
            break;

        default:
            /*--- wait for print status ---*/
            if (((pSpoolCtrlR.fbModeSpool&SPLMD_WAIT_EMPTY) != 0)
            || ((pSpoolCtrlJ.fbModeSpool&SPLMD_WAIT_EMPTY) != 0)) {
                pSpoolCtrlR.fbModeSpool &= (0xff - SPLMD_WAIT_EMPTY);
                pSpoolCtrlJ.fbModeSpool &= (0xff - SPLMD_WAIT_EMPTY);
                PifReleaseSem(hPmgWaitSem);
            }
            PmgReleaseSemRJ();
            /*--- sleep receive thread ---*/
            return;
/*          break;  */

        }
    }
}
#endif  /* defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION) */


#if defined (STATION_SLIP)
/*
*********************************************************************
*
**Synopsis:     VOID    PmgReceiveS(VOID)
*
**Return:       Non
*
**Description:  Receie status from priter (slip)
*
*********************************************************************
*/

VOID PmgReceiveS(VOID)
{
    SHORT       sRc;
    SPOOLDAT    *pSpoolBuff;
    PRT_RESPONCE    aPrtResponce;

    for (;;) {
        /*--- spool read/write semaphore ---*/
        PmgRequestSemS();

        switch (RcvGetSpoolData(PMG_PRT_SLIP, &pSpoolBuff)) {
        case SPL_DATA:
            PmgReleaseSemS();

            /*--- read communication port ---*/
            sRc = PmgReadCom(PMG_PRT_SLIP,
                             &aPrtResponce,
                             sizeof(aPrtResponce));

            if (sRc <= PIF_ERROR_COM_ERRORS) {
                /*--- error recovery ---*/
                RcvErrorRecovery(PMG_PRT_SLIP);
                /*-- count error tally ---*/
                pPrtCtrl.pPrtTally->usCommError++;

            } else if ((sRc == 0) || (sRc <= PIF_ERROR_COM_TIMEOUT)) {
                /*--- error recovery ---*/
                RcvErrorRecovery(PMG_PRT_SLIP);
                /*-- count error tally ---*/
                pPrtCtrl.pPrtTally->usTimeOut++;

            } else if (sRc == PIF_ERROR_COM_POWER_FAILURE) {
                /*--- power down recovry ---*/
                RcvPowerRecovery(PMG_PRT_SLIP);

            } else {
                /*--- check responce data ---*/
                RcvChkResponce(PMG_PRT_SLIP, &aPrtResponce);
            }

            PmgWakeUp(ID_WAIT_S);               /* wake up user             */
            PmgWakeUp(ID_SEND);                 /* wake up send thread      */
            break;

        case SPL_ERROR:
            /*--- error recovery ---*/
            PmgReleaseSemS();
            RcvErrorRecovery(PMG_PRT_SLIP);
            break;

        case SPL_POWER_FAIL:
            /*--- power down recovry ---*/
            PmgReleaseSemS();
            RcvPowerRecovery(PMG_PRT_SLIP);
            break;

        case SPL_ERR_IMP:
            /*--- error in important lines ---*/
            PmgReleaseSemS();
            break;

        default:
            /*--- wait for print status ---*/
            if ((pSpoolCtrlS.fbModeSpool&SPLMD_WAIT_EMPTY) != 0) {
                pSpoolCtrlS.fbModeSpool &= (0xff - SPLMD_WAIT_EMPTY);
                PifReleaseSem(hPmgWaitSem);
            }
            PmgReleaseSemS();
            /*--- sleep receive thread ---*/
            return;
/*          break;  */

        }
    }
}
#endif /* #if defined (STATION_SLIP) */


/*
*******************************************************************************
*
**Synopsis:     VOID    RcvChkResponce(usPrtType, aPrtResponce)
*               input   USHORT          usPrtType;      : printer ID
*               input   PRT_RESPONCE    *aPrtResponce;  : data buffer
*
**Return:       Non
*
**Description:  Check responce data
*
*******************************************************************************
*/

VOID RcvChkResponce(USHORT usPrtType, PRT_RESPONCE *aPrtResponce)
{
    if ((aPrtResponce->ucPrtStatus & PRT_STAT_ERROR) == 0) {
        RcvErrorRecovery(usPrtType);
        /*-- count error tally ---*/
        if (usPrtType == PMG_PRT_SLIP) {
            pPrtCtrl.pPrtTally->usJamError++;
        } else {
            pPrtCtrl.pPrtTally->usJamError++;
        }

    } else if ((aPrtResponce->ucPrtStatus & PRT_STAT_INIT_END) == 0) {
        PmgRequestSem(usPrtType);
        if (RcvCheckSeqNo(usPrtType, aPrtResponce->ucSeqNo) == FALSE) {
            PmgReleaseSem(usPrtType);
            RcvErrorRecovery(usPrtType);
        } else {
            PmgReleaseSem(usPrtType);
            RcvCommRecovery(usPrtType);
        }

    } else {
        PmgRequestSem(usPrtType);
        if (RcvCheckSeqNo(usPrtType, aPrtResponce->ucSeqNo) == FALSE) {
            PmgReleaseSem(usPrtType);
            RcvErrorRecovery(usPrtType);
            /*-- count error tally ---*/
            if (usPrtType == PMG_PRT_SLIP) {
                pPrtCtrl.pPrtTally->usSeqError++;
            } else {
                pPrtCtrl.pPrtTally->usSeqError++;
            }
        } else {
            /*--- set next receive pointer ---*/
            RcvDelSpoolData(usPrtType, aPrtResponce);
            PmgReleaseSem(usPrtType);

            if (usPrtType == PMG_PRT_SLIP) {
/*-----  no check paper for slip printer  -----*/
/***
                if ((aPrtResponce->ucPrtStatus & PRT_STAT_TAILEND) == 0) {
                    if (usSlipEnd++ >= PMG_SLIP_END) {
                        if (RcvWarningNearEnd(usPrtType) == TRUE) {
                            usSlipEnd = 0;
                        }
                    }
                } else {
                    if (usSlipEnd >= PMG_SLIP_END) {
                        PmgRestartSend(usPrtType);
                    }
                    usSlipEnd = 0;
                }
***/

            } else {
                if ((aPrtResponce->ucPrtStatus
                & (PRT_STAT_J_NEAR_END|PRT_STAT_R_NEAR_END)) != 0) {
                    if (usJournalEnd++ >= PMG_JNL_END) {
                        if (RcvWarningNearEnd(usPrtType) == TRUE) {
                            usJournalEnd = 0;
                        }
                    }
                } else {
                    if (usJournalEnd >= PMG_JNL_END) {
                        PmgRestartSend(usPrtType);
                    }
                    usJournalEnd = 0;
                }
            }
        }
    }
}

                
/*
*******************************************************************************
*
**Synopsis:     VOID    PmgDelSpoolData(usPrtType, aPrtResponce)
*               input   USHORT          usPrtType;      : printer ID
*               input   PPRT_RESPONCE   aPrtResponce;   : data buffer
*
**Return:       Non
*
**Description:  Delete the received data
*
*******************************************************************************
*/

VOID RcvDelSpoolData(USHORT usPrtType, PRT_RESPONCE *aPrtResponce)
{
    USHORT      usRstat, usJstat;
    SPOOLDAT    *pSpoolData;


    if (usPrtType == PMG_PRT_SLIP) {
#if defined (STATION_SLIP)
        /*--- delete slip data ---*/
        while (pSpoolCtrlS.pucSend != pSpoolCtrlS.pucReceive) {
            usRstat = RcvGetSpoolData(PMG_PRT_SLIP, &pSpoolData);
            if (usRstat == SPL_DATA) {
                if (PmgCmpSequence(pSpoolData->aDataCtrl.ucCommSeqNo,
                aPrtResponce->ucSeqNo) != 0) {
                    break;
                }
            }  else {
                if (pSpoolData->aDataCtrl.fbPrtMode == 0) {
                    break;
                }
            }

            pSpoolCtrlS.pucReceive = (UCHAR *)pSpoolData
                                        + pSpoolData->aDataCtrl.ucVLI;

            if (PmgChkCurStat(usPrtType, SPLMD_VALIDATION) == 0) {
                if ((pSpoolData->aDataCtrl.ucDataID != SPBID_PRINT_DATA_C)
                && (pSpoolData->aDataCtrl.ucDataID != SPBID_PRINT_DATA_RJ_C)) {
                    pSpoolCtrlS.pucValidation = pSpoolCtrlS.pucReceive;
                }
            }
        }
#endif /* #if defined (STATION_SLIP) */

    } else {
#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
        /*--- delete receipt data ---*/
        while (pSpoolCtrlR.pucSend != pSpoolCtrlR.pucReceive) {
            usRstat = RcvGetSpoolData(PMG_PRT_RECEIPT, &pSpoolData);
            if (usRstat == SPL_DATA) {
                if (PmgCmpSequence(pSpoolData->aDataCtrl.ucCommSeqNo,
                aPrtResponce->ucSeqNo) != 0) {
                    break;
                }
            }  else {
                if (pSpoolData->aDataCtrl.fbPrtMode == 0) {
                    break;
                }
            }

            pSpoolCtrlR.pucReceive = (UCHAR *)pSpoolData
                                        + pSpoolData->aDataCtrl.ucVLI;

            if (PmgChkCurStat(usPrtType, SPLMD_VALIDATION) == 0) {
                if ((pSpoolData->aDataCtrl.ucDataID != SPBID_PRINT_DATA_C)
                && (pSpoolData->aDataCtrl.ucDataID != SPBID_PRINT_DATA_RJ_C)) {
                    pSpoolCtrlR.pucValidation = pSpoolCtrlR.pucReceive;
                }
            }
        }

        /*--- delete journal data ---*/
        while (pSpoolCtrlJ.pucSend != pSpoolCtrlJ.pucReceive) {
            usJstat = RcvGetSpoolData(PMG_PRT_JOURNAL, &pSpoolData);
            if (usJstat == SPL_DATA) {
                if (PmgCmpSequence(pSpoolData->aDataCtrl.ucCommSeqNo,
                aPrtResponce->ucSeqNo) != 0) {
                    break;
                }
            }  else {
                if (pSpoolData->aDataCtrl.fbPrtMode == 0) {
                    break;
                }
            }

            pSpoolCtrlJ.pucReceive = (UCHAR *)pSpoolData
                                        + pSpoolData->aDataCtrl.ucVLI;

            if (PmgChkCurStat(usPrtType, SPLMD_VALIDATION) == 0) {
                if ((pSpoolData->aDataCtrl.ucDataID != SPBID_PRINT_DATA_C)
                && (pSpoolData->aDataCtrl.ucDataID != SPBID_PRINT_DATA_RJ_C)) {
                    pSpoolCtrlJ.pucValidation = pSpoolCtrlJ.pucReceive;
                }
            }
        }
#endif /* #if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION) */

    }
}


#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
/*
*********************************************************************
*
**Synopsis:     USHORT  RcvChkSpoolData(VOID)
*
**Return:       type of spool data
*
**Description:  Test spool data
*
*********************************************************************
*/

USHORT RcvChkSpoolData(VOID)
{
    SPOOLDAT    *pSpoolBuff;
    USHORT      usStatRct, usStatJnl;


    usStatRct = RcvGetSpoolData(PMG_PRT_RECEIPT, &pSpoolBuff);
    usStatJnl = RcvGetSpoolData(PMG_PRT_JOURNAL, &pSpoolBuff);

    /*--- check data ID in R&J spool buffer ---*/
    if ((usStatRct == SPL_DATA) || (usStatJnl == SPL_DATA)) {
        return(SPL_DATA);
    }

    if ((usStatRct == SPL_POWER_FAIL) || (usStatJnl == SPL_POWER_FAIL)) {
        return(SPL_POWER_FAIL);
    }

    if ((usStatRct == SPL_ERROR) || (usStatJnl == SPL_ERROR)) {
        return(SPL_ERROR);
    }

    if ((usStatRct == SPL_ERR_IMP) || (usStatJnl == SPL_ERR_IMP)) {
        return(SPL_ERR_IMP);
    }

    return(SPL_NO_DATA);
}
#endif /* #if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION) */


/*
*********************************************************************
*
**Synopsis:     USHORT  RcvGetSpoolData(usPrtType, pBuffer)
*               input   USHORT      usPrtType;  : printer ID
*               output  SPOOLDAT    **pBuffer;  : data buffer
*
**Return:       Data type
*
**Description:  Get spool data
*
*********************************************************************
*/

USHORT RcvGetSpoolData(USHORT usPrtType, SPOOLDAT **pBuffer)

{
    SPOOLCNT    *pSpoolCtrl = PmgGetSpoolCtrl(usPrtType);            /* spool control block      */
    SPOOLDAT    *pSpoolData;

    for (;;) {
        *pBuffer = pSpoolData = (SPOOLDAT *)pSpoolCtrl->pucReceive;

        if (pSpoolCtrl->pucReceive == pSpoolCtrl->pucSend) {
            /*--- return if no data ---*/
            return(SPL_NO_DATA);
        }

        if (pSpoolData->aDataCtrl.ucDataID == SPBID_END_SPOOL) {
            /*--- if buffer end then loop the pointer ---*/
            pSpoolCtrl->pucReceive = pSpoolCtrl->pucStart;
            *pBuffer = pSpoolData = (SPOOLDAT *)pSpoolCtrl->pucStart;
        }

        if (pSpoolCtrl->pucReceive == pSpoolCtrl->pucSend) {
            /*--- return if no data ---*/
            return(SPL_NO_DATA);
        }

        switch (pSpoolData->aDataCtrl.fbPrtMode) {
        case 0:
            /*--- return if not executed ---*/
            return(SPL_NO_DATA);
        case SPBST_ERROR:
            /*--- return if error ---*/
            return(SPL_ERROR);
        case SPBST_POWERFAIL:
            /*--- return if power failed ---*/
            return(SPL_POWER_FAIL);
        case SPBST_SKIP:
            /*--- return if error in important lines ---*/
            pSpoolCtrl->pucValidation = pSpoolCtrl->pucReceive += pSpoolData->aDataCtrl.ucVLI;
            return(SPL_ERR_IMP);
        default:
            break;
        }

        switch (pSpoolData->aDataCtrl.ucDataID) {
        case SPBID_PRINT_DATA_C:
        case SPBID_PRINT_DATA_E:
        case SPBID_PRINT_DATA_RJ_C:
        case SPBID_PRINT_DATA_RJ_E:
        case SPBID_FONT_A:
        case SPBID_FONT_B:
            return(SPL_DATA);
        default:
            break;
        }

        pSpoolCtrl->pucReceive += pSpoolData->aDataCtrl.ucVLI;

        if (PmgChkCurStat(usPrtType, SPLMD_VALIDATION) == 0) {
            pSpoolCtrl->pucValidation = pSpoolCtrl->pucReceive;
        }
    }
}


/*
*******************************************************************************
*
**Synopsis:     USHORT  RcvCheckSeqNo(usPrtType, ucSeqNo)
*               input   USHORT      usPrtType;  : printer ID
*               input   UCHAR       ucSeqNo;    : responce sequence number
*
**Return:       TRUE  : good
*               FALSE : sequence number error
*
**Description:  Check responce sequnce number
*
*******************************************************************************
*/

USHORT RcvCheckSeqNo(USHORT usPrtType, UCHAR ucSeqNo)
{
#if defined (HI_SPEED)
    return(TRUE);
#else

    if (PmgCmpSequence(ucSeqNo, pPrtCtrl.ucRcvSeqNo) == 0) {
        pPrtCtrl.ucRcvSeqNo++;
        return(TRUE);

    } else if ((UCHAR)(pPrtCtrl.ucRcvSeqNo - ucSeqNo) == 1) {
        return(TRUE);

    } else {
        return(FALSE);
    }
    usPrtType = usPrtType;
#endif
}


/*
*********************************************************************
*
**Synopsis:     USHORT  RcvWarningNearEnd(usPrtType)
*               input   USHORT      usPrtType;  : printer ID
*
**Return:       TRUE  : display error message
*               FALSE : exist sended message (need read message)
*
**Description:  Display journal paper near end
*
*********************************************************************
*/

USHORT RcvWarningNearEnd(USHORT usPrtType)
{
    /*--- request to stop send thread ---*/
    PmgStopSend(usPrtType);

    if (usPrtType == PMG_PRT_SLIP) {
        while (pPrtCtrl.ucSndSeqNo == pPrtCtrl.ucRcvSeqNo) {
            if (((pPrtCtrl.ucStatSend & SPLST_ERROR) != 0)
            || (PmgChkSleep(ID_SEND,
                        WAIT_BUFF
                        |WAIT_DATA
                        |WAIT_ERROR) != 0)) {
                /*--- error display & input key ---*/
                PmgErrorMsg(DEVICEIO_PRINTER_MSG_PAPERCHG_ADR);
                /*--- restart to print ---*/
                PmgRestartSend(usPrtType);

                return(TRUE);
            } else {
                PmgSleep(ID_RECEIVE, WAIT_ERROR, 10);
            }
        }

    } else {
        while (pPrtCtrl.ucSndSeqNo == pPrtCtrl.ucRcvSeqNo) {
            if (((pPrtCtrl.ucStatSend & SPLST_ERROR) != 0)
            || (PmgChkSleep(ID_SEND,
                        WAIT_BUFF
                        |WAIT_DATA
                        |WAIT_ERROR) != 0)) {
                /*--- error display & input key ---*/
                PmgErrorMsg(DEVICEIO_PRINTER_MSG_JNLLOW_ADR);
                /*--- restart to print ---*/
                PmgRestartSend(usPrtType);

                return(TRUE);
            } else {
                PmgSleep(ID_RECEIVE, WAIT_ERROR, 10);
            }
        }
    }

    return(FALSE);
}


/*
*********************************************************************
*
**Synopsis:     VOID    RcvSeqErrRecovery(usPrtType)
*               input   USHORT      usPrtType;  : printer ID
*               input   UCHAR       ucSeqNo;    : responce sequence number
*
**Return:       Non
*
**Description:  Recovery sequence number error
*
*********************************************************************
*/

VOID RcvSeqErrRecovery(USHORT usPrtType, UCHAR ucSeqNo)
{
    USHORT  usRct, usJnl;
    USHORT  usIsEjectError;

    /*--- request to stop send thread ---*/
    PmgStopSend(usPrtType);

    if (PmgCmpSequence(ucSeqNo, (UCHAR)(pPrtCtrl.ucSndSeqNo-1)) == 0) {
        if (usPrtType == PMG_PRT_SLIP) {
            /*--- recovery slip printer ---*/
            usRct = RcvSpoolRecovery(PMG_PRT_SLIP, &usIsEjectError);

        } else {
            /*--- recovery receipt pinter ---*/
            usRct = RcvSpoolRecovery(PMG_PRT_RECEIPT, &usIsEjectError);

            /*--- recovery journal printer ---*/
            usJnl = RcvSpoolRecovery(PMG_PRT_JOURNAL, &usIsEjectError);

        }

        /*--- restart to print ---*/
        PmgRestartSend(usPrtType);
    }

    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    RcvCommRecovery(usPrtType)
*               input   USHORT      usPrtType;  : printer ID
*
**Return:       Non
*
**Description:  Recovery communication error
*
*********************************************************************
*/

VOID RcvCommRecovery(USHORT usPrtType)
{
    USHORT  usRct, usJnl;
    USHORT  usIsEjectError;

    /*--- request to stop send thread ---*/
    PmgStopSend(usPrtType);

    if (usPrtType == PMG_PRT_SLIP) {

        for (;;) {
            if ((pPrtCtrl.ucStatSend & SPLST_ERROR) != 0) break;
            if (PmgChkSleep(ID_SEND,
                            WAIT_BUFF
                            |WAIT_DATA
                            |WAIT_ERROR) != 0) break;
            PifSleep(50);
        }

        /*--- recovery slip printer ---*/
        usRct = RcvSpoolRecovery(PMG_PRT_SLIP, &usIsEjectError);

    } else {

        for (;;) {
            if ((pPrtCtrl.ucStatSend & SPLST_ERROR) != 0) break;
            if (PmgChkSleep(ID_SEND,
                            WAIT_BUFF
                            |WAIT_DATA
                            |WAIT_ERROR) != 0) break;
            PifSleep(50);
        }

        /*--- recovery receipt pinter ---*/
        usRct = RcvSpoolRecovery(PMG_PRT_RECEIPT, &usIsEjectError);

        /*--- recovery journal printer ---*/
        usJnl = RcvSpoolRecovery(PMG_PRT_JOURNAL, &usIsEjectError);

    }

    /*--- send abort commoand ---*/
    while(PmgResetPrt(usPrtType) == FALSE) {

        /*--- error display & input key ---*/
        PmgErrorMsg(DEVICEIO_PRINTER_MSG_PRTTBL_ADR);
    }

    /*--- restart to print ---*/
    PmgRestartSend(usPrtType);

    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    RcvErrorRecovery(usPrtType)
*               input   USHORT      usPrtType;  : printer ID
*
**Return:       Non
*
**Description:  Recovery printer error
*
*********************************************************************
*/

VOID RcvErrorRecovery(USHORT usPrtType)
{
    USHORT  usRct, usJnl;
    SHORT   sRc, i;
    USHORT  usErrorMsg;
    USHORT  usIsEjectError;

    /*--- request to stop send thread ---*/
    PmgStopSend(usPrtType);

    if (usPrtType == PMG_PRT_SLIP) {

        for (;;) {
            if ((pPrtCtrl.ucStatSend & SPLST_ERROR) != 0) break;
            if (PmgChkSleep(ID_SEND,
                            WAIT_BUFF
                            |WAIT_DATA
                            |WAIT_ERROR) != 0) break;
            PifSleep(50);
        }

        /*--- recovery slip printer ---*/
        usRct = RcvSpoolRecovery(PMG_PRT_SLIP, &usIsEjectError);

        /*--- reset printer (open platen) ---*/
        sRc = PmgResetPrt(usPrtType);

        usErrorMsg = DEVICEIO_PRINTER_MSG_PAPERCHG_ADR;

    } else {

        for (;;) {
            if ((pPrtCtrl.ucStatSend & SPLST_ERROR) != 0) break;
            if (PmgChkSleep(ID_SEND,
                            WAIT_BUFF
                            |WAIT_DATA
                            |WAIT_ERROR) != 0) break;
            PifSleep(50);
        }

        /*--- recovery receipt pinter ---*/
        usRct = RcvSpoolRecovery(PMG_PRT_RECEIPT, &usIsEjectError);

        /*--- recovery journal printer ---*/
        usJnl = RcvSpoolRecovery(PMG_PRT_JOURNAL, &usIsEjectError);

        usErrorMsg = DEVICEIO_PRINTER_MSG_PRTTBL_ADR;
    }

    if ( ! usIsEjectError )
    {
        /*--- error display & input key ---*/
        PmgErrorMsg(DEVICEIO_PRINTER_MSG_PRTTBL_ADR);

        /*--- send abort commoand ---*/
        for (i=0; i<3; i++) {
            sRc = PmgResetPrt(usPrtType);
            if (sRc == TRUE) break;
        }

        if (PmgChkCurStat(usPrtType, SPLMD_VALIDATION) == 0) {
            /*--- '////.../' print ---*/
            for (;;) {
                sRc = RcvErrorPrint(usPrtType, usRct, usJnl);
                if (sRc > 0) break;
                if (sRc == PIF_ERROR_COM_POWER_FAILURE) PmgReOpenPrt(usPrtType);

                if ((sRc == PIF_ERROR_COM_TIMEOUT) && (usPrtType == PMG_PRT_SLIP))
                {
                    sRc = PmgResetPrt(usPrtType);
                }

                /*--- error display & input key ---*/
                PmgErrorMsg(usErrorMsg);

                /*--- send abort commoand ---*/
                for (i=0; i<3; i++) {
                    sRc = PmgResetPrt(usPrtType);
                    if (sRc == TRUE) break;
                }
            }
        }
    }
    /*--- restart to print ---*/
    PmgRestartSend(usPrtType);
}


/*
*********************************************************************
*
**Synopsis:     VOID    RcvPowerRecovery(usPrtType)
*               input   USHORT      usPrtType;  : printer ID
*
**Return:       Non
*
**Description:  Power down recovery
*
*********************************************************************
*/

VOID RcvPowerRecovery(USHORT usPrtType)
{
    USHORT  usRct, usJnl;
    SHORT   sRc, i;
    USHORT  usIsEjectError;

    /*--- request to stop send thread ---*/
    PmgStopSend(usPrtType);

    if (usPrtType == PMG_PRT_SLIP) {

        for (;;) {
            if ((pPrtCtrl.ucStatSend & SPLST_ERROR) != 0) break;
            if (PmgChkSleep(ID_SEND,
                            WAIT_BUFF
                            |WAIT_DATA
                            |WAIT_ERROR) != 0) break;
            PifSleep(50);
        }

        PmgRequestSem(usPrtType);

        /*--- recovery slip printer ---*/
        usRct = RcvSpoolRecovery(PMG_PRT_SLIP, &usIsEjectError);

    } else {

        for (;;) {
            if ((pPrtCtrl.ucStatSend & SPLST_ERROR) != 0) break;
            if (PmgChkSleep(ID_SEND,
                            WAIT_BUFF
                            |WAIT_DATA
                            |WAIT_ERROR) != 0) break;
            PifSleep(50);
        }

        PmgRequestSem(usPrtType);

        /*--- recovery receipt pinter ---*/
        usRct = RcvSpoolRecovery(PMG_PRT_RECEIPT, &usIsEjectError);

        /*--- recovery journal printer ---*/
        usJnl = RcvSpoolRecovery(PMG_PRT_JOURNAL, &usIsEjectError);

    }
    /*--- re-open communication ---*/
    PmgReOpenPrt(usPrtType);

    /*--- send abort commoand ---*/
    for (i=0; i<3; i++) {
        sRc = PmgResetPrt(usPrtType);
        if (sRc == TRUE) break;
    }

    if (PmgChkCurStat(usPrtType, SPLMD_VALIDATION) == 0) {
        /*--- '////.../' print ---*/
        for (;;) {
            sRc = RcvErrorPrint(usPrtType, usRct, usJnl);
            if (sRc > 0) break;

            if (sRc == PIF_ERROR_COM_POWER_FAILURE) {
                PmgReOpenPrt(usPrtType);

            } else {
                /*--- error display & input key ---*/
                PmgErrorMsg(DEVICEIO_PRINTER_MSG_PRTTBL_ADR);
            }

            /*--- send abort commoand ---*/
            for (i=0; i<3; i++) {
                sRc = PmgResetPrt(usPrtType);
                if (sRc == TRUE) break;
            }
        }
    }
    /*--- restart to print ---*/
    PmgRestartSend(usPrtType);

    PmgReleaseSem(usPrtType);
}


/*
*********************************************************************
*
**Synopsis:     VOID    RcvSpoolRecovery(usPrtType)
*               input   USHORT      usPrtType;  : printer ID
*
**Return:       TRUE  : need recovry
*               FALSE : not need recovry
*
**Description:  Recovery spool data
*
*********************************************************************
*/

USHORT RcvSpoolRecovery(USHORT usPrtType, USHORT* pusIsEject)
{
    if ((PmgGetCurStat(usPrtType) & SPLMD_IMPORTANT) != 0) {
        /*--- recovery pointer & check "///../" print ---*/
        RcvRecvPrtPoint(usPrtType, pusIsEject);
        /*--- set error in important line ---*/
        PmgSetCurStat(usPrtType, SPLMD_IMP_ERROR);
        return(FALSE);

    } else {
        /*--- recovery pointer & check "///../" print ---*/
        return(RcvRecvPrtPoint(usPrtType, pusIsEject));

    }
}


/*
*********************************************************************
*
**Synopsis:     USHORT  RcvRecvPrtPoint(usPrtType)
*               input   USHORT      usPrtType;  : printer ID
*
**Return:       TRUE  : need recovery
*               FALSE : not need recovery
*
**Description:  Recovery pointer & check "///../" print
*
*********************************************************************
*/

USHORT RcvRecvPrtPoint(USHORT usPrtType, USHORT* pusIsEject)
{
    SPOOLCNT    *pSpoolCtrl = PmgGetSpoolCtrl(usPrtType);
    SPOOLDAT    *pNext;
    USHORT      usRc;

#if ! defined (USE_2170_ORIGINAL)

    /* --- Fix a glitch (05/16/2001)
        Slip print
    --- */

    *pusIsEject = FALSE;

    if ( usPrtType == PMG_PRT_SLIP )
    {
        /* --- there is no spool data to recover --- */

        if ( pSpoolCtrl->pucSend == pSpoolCtrl->pucValidation )
        {
            *pusIsEject = TRUE;
            return FALSE;
        }

        /* --- determine "///" line should be printed or not --- */

        pNext = (SPOOLDAT *)pSpoolCtrl->pucReceive;
        usRc = ( pNext->aDataCtrl.fbPrtMode == SPBST_EXECUTED ) ? TRUE : FALSE;

        /* --- reset printing status of the failed spool data --- */

        for ( pNext = (SPOOLDAT *)pSpoolCtrl->pucValidation;
              pSpoolCtrl->pucSend != (UCHAR *)pNext;
              pNext = (SPOOLDAT *)((UCHAR *)pNext + pNext->aDataCtrl.ucVLI ))
        {
            if ( pNext->aDataCtrl.ucDataID == SPBID_END_SPOOL )
            {
                pNext = (SPOOLDAT *)pSpoolCtrl->pucStart;
            }
            if ( pNext->aDataCtrl.ucDataID == SPBID_END_DATA )
            {
                break;
            }
            if (( pNext->ucPrtData[0] == 0x0C ) ||
                (( pNext->ucPrtData[0] == 0x1B ) &&
                 ( pNext->ucPrtData[1] == 0x65 )))
            {
                *pusIsEject = TRUE;
                usRc = FALSE;
                break;
            }

            pNext->aDataCtrl.fbPrtMode   = 0;
            pNext->aDataCtrl.ucCommSeqNo = 0;
        }

        if ( *pusIsEject )
        {
            /* --- restore send pointer and sequence # --- */

            pSpoolCtrl->pucReceive =
                pSpoolCtrl->pucValidation = pSpoolCtrl->pucSend;

            pPrtCtrl.ucSndSeqNo = pPrtCtrl.ucRcvSeqNo;
        }
        else
        {
            /* --- restore send pointer and sequence # --- */

            pSpoolCtrl->pucSend =
                pSpoolCtrl->pucReceive = pSpoolCtrl->pucValidation;

            pPrtCtrl.ucSndSeqNo = pPrtCtrl.ucRcvSeqNo;
        }
    }
    else
    {
        /* --- receipt, journal or validation print --- */
#endif
        if (pSpoolCtrl->pucSend == pSpoolCtrl->pucValidation) {
            return(FALSE);
        }

        pNext = (SPOOLDAT *)pSpoolCtrl->pucReceive;
        if (pNext->aDataCtrl.fbPrtMode == SPBST_EXECUTED) usRc = TRUE;
        else usRc = FALSE;

        for (pNext = (SPOOLDAT *)pSpoolCtrl->pucValidation;
        pSpoolCtrl->pucSend != (UCHAR *)pNext;
        pNext = (SPOOLDAT *)((UCHAR *)pNext + pNext->aDataCtrl.ucVLI)) {

            if (pNext->aDataCtrl.ucDataID == SPBID_END_SPOOL) {
                pNext = (SPOOLDAT *)pSpoolCtrl->pucStart;
            }
            if (pNext->aDataCtrl.ucDataID == SPBID_END_DATA) {
                break;
            }

            /*--- reset data condition ---*/
            pNext->aDataCtrl.fbPrtMode = 0;
            pNext->aDataCtrl.ucCommSeqNo = 0;
        }
        /*--- restore the send pointer ---*/
        pSpoolCtrl->pucSend = pSpoolCtrl->pucReceive = pSpoolCtrl->pucValidation;

        /*--- restore the communication sequence number ---*/
        pPrtCtrl.ucSndSeqNo = pPrtCtrl.ucRcvSeqNo;
#if ! defined (USE_2170_ORIGINAL)
    }
#endif
    return(usRc);
}


/*
*******************************************************************************
*
**Synopsis:     SHORT   RcvErrorPrint(usPrtType, usRct, usJnl)
*               input   USHORT      usPrtType;  : printer ID
*               input   USHORT      usRct;      : status of receipt or slip
*               input   USHORT      usJnl;      : ststus of journal
*
**Return:       Pif return status
*
**Description:  Recovery to print "///../" of printer
*
*******************************************************************************
*/

SHORT RcvErrorPrint(USHORT usPrtType, USHORT usRct, USHORT usJnl)
{
    USHORT          i;
    SHORT           sRc;
    PRT_SNDMSG      aucRecovery;
    PRT_RESPONCE    aPrtResponce;
#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
    UCHAR           ucSelPrt;
    DATACTRL        *pDataCtrl;
    USHORT          len, usOffset;
#endif


    if (usPrtType == PMG_PRT_SLIP) {
        if (usRct == FALSE) return(1);
        /*--- make '////...//' for slip printer ---*/
#if defined (STATION_SLIP)
        aucRecovery.auchIDENT[0]
            = aucRecovery.auchIDENT[1]
            = aucRecovery.auchIDENT[2]
            = 0;
        aucRecovery.ucSeqNo = pPrtCtrl.ucSndSeqNo++;

        len = 0;
        aucRecovery.aucPrtMsg[len] = 0x1b;  len++;  /* select station ESC */
        aucRecovery.aucPrtMsg[len] = 'c';   len++;  /* c */
        aucRecovery.aucPrtMsg[len] = '0';   len++;  /* 0 */
        aucRecovery.aucPrtMsg[len] = 0x04;  len++;  /* slip */
        for (i = 0; i < pSpoolCtrlS.ucMaxChar; i++) {
            aucRecovery.aucPrtMsg[len] = '/'; len++;
        }
        aucRecovery.aucPrtMsg[len] = LF;    len++;
        sRc = EscpWriteCom((USHORT)pPrtCtrl.hPort,
                          (VOID FAR *)&aucRecovery,
                          (USHORT)(4 + len));

#endif
    } else {
        if ((usRct == FALSE)&&(usJnl == FALSE)) return(1);

#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
        /*--- make '////...//' for R/J printer ---*/
        len = ucSelPrt = 0;
        if (usRct == TRUE) {
            ucSelPrt |= PMG_SEL_RECEIPT;
            len += pSpoolCtrlR.ucMaxChar;
            /*--- cont printer tally ---*/
            pPrtCtrl.pPrtTally->ulRctLine++;
        }

        if (usJnl == TRUE) {
            ucSelPrt |= PMG_SEL_JOURNAL;
            len += pSpoolCtrlJ.ucMaxChar;
            /*--- cont printer tally ---*/
            pPrtCtrl.pPrtTally->ulJnlLine++;

        } else if (usRct == TRUE) {
            pDataCtrl = (DATACTRL *)pSpoolCtrlR.pucSend;
            if (pDataCtrl->ucDataID == SPBID_END_SPOOL) {
                pDataCtrl = (DATACTRL *)pSpoolCtrlR.pucStart;
            }
            if ((pDataCtrl->ucDataID == SPBID_PRINT_DATA_RJ_C)
            || (pDataCtrl->ucDataID == SPBID_PRINT_DATA_RJ_E)) {
                ucSelPrt |= PMG_SEL_JOURNAL;
                len += pSpoolCtrlJ.ucMaxChar;
                /*--- cont printer tally ---*/
                pPrtCtrl.pPrtTally->ulJnlLine++;
            }
        }
        aucRecovery.auchIDENT[0]
            = aucRecovery.auchIDENT[1]
            = aucRecovery.auchIDENT[2]
            = 0;
        aucRecovery.ucSeqNo = pPrtCtrl.ucSndSeqNo++;

        usOffset = 0;
#if (NUMBER_OF_STATION > 1)
        /*--- set printer selection ---*/
        memcpy(&aucRecovery.aucPrtMsg[usOffset], &aucSelPrt, sizeof(aucSelPrt));
        usOffset += sizeof(aucSelPrt);
        aucRecovery.aucPrtMsg[usOffset-1] = ucSelPrt;
#endif
#if defined (PROVIDED_SAME_PRINT)
        /*--- set to print separately ---*/
        memcpy(&aucRecovery.aucPrtMsg[usOffset], &aucSameOff, sizeof(aucSameOff));
        usOffset += sizeof(aucSameOff);
#endif
        /* fill the character '/' into buffer to transmit */
        /* for (i=usOffset; i<len; i++) { */
        for (i = usOffset; i < len + usOffset; i++) {
            aucRecovery.aucPrtMsg[i] = '/';
        }
        aucRecovery.aucPrtMsg[i++] = LF;

        sRc = EscpWriteCom((USHORT)pPrtCtrl.hPort,
                          (VOID FAR *)&aucRecovery,
                          (USHORT)(4+i));

#endif /* defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION) */
    }

    if (sRc <= PIF_ERROR_COM_ERRORS) {
        /*-- count error tally ---*/
        if (usPrtType == PMG_PRT_SLIP) {
            pPrtCtrl.pPrtTally->usCommError++;
        } else {
            pPrtCtrl.pPrtTally->usCommError++;
        }
        return(sRc);
    }

    if (usPrtType == PMG_PRT_SLIP) {
        do {
            sRc = PmgReadCom(usPrtType,
                             &aPrtResponce,
                             sizeof(aPrtResponce));
            if (sRc < 0) break;
        } while ((aPrtResponce.ucSeqNo != (UCHAR)(pPrtCtrl.ucSndSeqNo-1))
                || (sRc == 0));
        pPrtCtrl.ucRcvSeqNo = pPrtCtrl.ucSndSeqNo;

    } else {
        do {
            sRc = PmgReadCom(usPrtType,
                             &aPrtResponce,
                             sizeof(aPrtResponce));
            if (sRc < 0) break;
        } while ((aPrtResponce.ucSeqNo != (UCHAR)(pPrtCtrl.ucSndSeqNo-1))
                || (sRc == 0));
        pPrtCtrl.ucRcvSeqNo = pPrtCtrl.ucSndSeqNo;
    }

    if (sRc > 0) {
        if (((aPrtResponce.ucPrtStatus & PRT_STAT_ERROR) == 0)
        || ((aPrtResponce.ucPrtStatus & PRT_STAT_INIT_END) == 0)) {
            sRc = PIF_ERROR_COM_ERRORS;
        }
    }

    if (sRc <= PIF_ERROR_COM_ERRORS) {
        /*-- count error tally ---*/
        if (usPrtType == PMG_PRT_SLIP) {
            pPrtCtrl.pPrtTally->usCommError++;
        } else {
            pPrtCtrl.pPrtTally->usCommError++;
        }
    }
    return(sRc);
}
