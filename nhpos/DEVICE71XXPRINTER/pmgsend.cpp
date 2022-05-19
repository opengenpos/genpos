/*
**************************************************************************
**
**  Title:      Print data send module
**
**  Categoly:   Print Manager
**
**  Abstruct:   Send spool data to printer
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
;=============================================================================
;     Rev.   |  Date      |  Comment
;-----------------------------------------------------------------------------
;   00.00.00 |            | Initial
;   G0       | May-25-'95 | modify BeginValidation() 
;   G1       | Aug-15-'95 | Modified censor handling in SndBeginValidation().
;            | Aug-17-'95 | Deleted error handling of PifWriteCom() in
;            |            | SndOpenPlaten().
;   G9       | Feb-25-'97 | Modify SndPrintFunctionRJ()
;            |            | 
;   G14      | Feb-17-'99 | A.Mitsui    Modify SndCommError()
;            |            |           
;   Saratoga | Apr-11-'00 | M.Teraki    Modify PmgSendRJ()       <#001#>
;            |            |             eliminate suspended 'ESC v' for 7140
;=============================================================================
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
#include    "pmgif.h"
#include    "pmgcmn.h"
#include    "pmgsend.h"
#include    "printio.h"
#include    "escp.h"
#include    "devpmg.h"

extern  PRTCTRL     pPrtCtrl;
#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
extern  SPOOLCNT    pSpoolCtrlR;
extern  SPOOLCNT    pSpoolCtrlJ;
#endif
#if defined (STATION_SLIP)
extern  SPOOLCNT    pSpoolCtrlS;
#endif
extern  USHORT      hPmgImpRctSem;      /* handle of semaphore          */
extern  USHORT      hPmgImpJnlSem;      /*  (PmgEndImportant)           */
extern  USHORT      hPmgImpSlpSem;      /*                              */
extern  UCHAR       ucFontTypeRJ;       /* font type R/J                */
extern  UCHAR       ucFeedLenRJ;        /* feed length R/J              */

extern  UCHAR       ucFeedLenS;                 /* feed length Slip             */

extern  USHORT      usSaveValRetryCount; /* Save count for PmgSetValWaitCount() */

/**
;========================================================================
;+                                                                      +
;+            S T A T I C    R A M    A S S I G N M E N T               +
;+                                                                      +
;========================================================================
**/
    static  UCHAR   aucSelPrt[] = {ESC, 'c', '0', 0};
    static  UCHAR   aucSelPrtVal[] = { ESC, 'c', '0', 4};
    static  UCHAR   aucRJHome[] = {ESC, '@', ESC, 'c', '0', 3, ESC, '<'};
    static  UCHAR   aucRJHome2[] = {ESC, 'c', '0', 1};
    static  UCHAR   aucRJHome3[] = {ESC, 'd', 0};
#if defined (DEVICE_7158) || defined (DEVICE_7194) || defined (DEVICE_7161) || defined (DEVICE_7196)
    static  UCHAR   aucSlpOpen[] = {0x0c};
#else
    static  UCHAR   aucSlpOpen[] = {ESC, 'q'};
#endif
    static  UCHAR   aucSameOn[] = {ESC, 'c', '0', 3, ESC, 'z', 1};
    static  UCHAR   aucSameOff[] = {ESC, 'z', 0};
#if defined(DEVICE_7158)
    static  UCHAR   aucFontEsc[] = {ESC, '!', 0, ESC, 'c', '1', 0x02, ESC, '3', PMG_SET_DEF_LF};
#else
    static  UCHAR   aucFontEsc[] = {ESC, '!', 0, ESC, '3', PMG_SET_DEF_LF};
#endif

/*
*********************************************************************
*
**Synopsis:     VOID    PmgSend(VOID)
*
**Return:       Non
*
**Description:  Send spool data to printer (receipt & journal & slip)
*
*********************************************************************
*/

VOID THREADENTRY PmgSend(VOID)
{
    while (pPrtCtrl.bDoSend) {
#if defined (STATION_SLIP)
        PmgSendS();
#endif
#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
        PmgSendRJ();
#endif
        PmgWakeUp(ID_RECEIVE);
        PmgSleep(ID_SEND, WAIT_DATA, 0);
    }
    pPrtCtrl.ucStatSend |= SPLST_FINALIZED;
}


#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
/*
*********************************************************************
*
**Synopsis:     VOID    PmgSendRJ(VOID)
*
**Return:       Non
*
**Description:  Send spool data to printer (receipt & journal)
*
*********************************************************************
*/

VOID PmgSendRJ(VOID)
{
    UCHAR   ucStatRct;              /* status of receipt spool      */
    UCHAR   ucStatJnl;              /* status of journal spool      */
    SPOOLDAT    *pSpoolRct;         /* pointer to receipt buffer    */
    SPOOLDAT    *pSpoolJnl;         /* pointer to journal buffer    */


    for (;;) {

        PmgRequestSemRJ();              /* spool read/write semaphore   */
                                        /* get print data from spool    */
        ucStatRct = SndGetSpoolData(PMG_PRT_RECEIPT, &pSpoolRct);
        ucStatJnl = SndGetSpoolData(PMG_PRT_JOURNAL, &pSpoolJnl);

        if ((ucStatRct == SPL_NO_DATA) && (ucStatJnl == SPL_NO_DATA)) {
            PmgReleaseSemRJ();
            return;

        } else if ((ucStatRct == SPL_ERR_RCV) || (ucStatJnl == SPL_ERR_RCV)) {
            pPrtCtrl.ucStatSend |= SPLST_ERROR;
            PmgReleaseSemRJ();                      /*  for receive     */
            PmgWakeUp(ID_RECEIVE);
            PifSleep(50);
            pPrtCtrl.ucStatSend &= (0xff - SPLST_ERROR);

#if !defined (HI_SPEED)
#if defined(DEVICE_7140)                                                   /* A START <#001#> */
        } else if ((UCHAR)(pPrtCtrl.ucSndSeqNo-pPrtCtrl.ucRcvSeqNo) > 0) {
#else                                                                      /* A END <#001#> */
        } else if ((UCHAR)(pPrtCtrl.ucSndSeqNo-pPrtCtrl.ucRcvSeqNo) >= 2) {
#endif                                                                     /* A <#001#> */
            pPrtCtrl.ucStatSend |= SPLST_2BUFF;
            PmgReleaseSemRJ();
            PmgWakeUp(ID_RECEIVE);
            PmgSleep(ID_SEND, WAIT_BUFF, 0);
            pPrtCtrl.ucStatSend &= (0xff - SPLST_2BUFF);
#endif
        } else if ((ucStatRct == SPL_DATA) && (ucStatJnl == SPL_DATA)) {
            PmgReleaseSemRJ();
            SndPrintFunctionRJ(ucStatRct, ucStatJnl, pSpoolRct, pSpoolJnl);

        } else {
            if ((ucStatRct != SPL_NO_DATA) && (ucStatJnl != SPL_NO_DATA)) {
                if (PmgCmpSequence(pSpoolRct->aDataCtrl.ucInputSeqNo,
                pSpoolJnl->aDataCtrl.ucInputSeqNo) < 0) {
                    ucStatJnl = SPL_NO_DATA;
                } else {
                    ucStatRct = SPL_NO_DATA;
                }
            }

            if (ucStatRct == SPL_ERR_IMP) {
                SndNextSpoolPtr(PMG_PRT_RECEIPT, SPBST_SKIP);
                PmgReleaseSemRJ();

            } else if (ucStatJnl == SPL_ERR_IMP) {
                SndNextSpoolPtr(PMG_PRT_JOURNAL, SPBST_SKIP);
                PmgReleaseSemRJ();

            } else if (ucStatRct == SPL_SPC_FNC) {
                PmgReleaseSemRJ();
                if ((pSpoolRct->aDataCtrl.ucDataID == SPBID_PRINT_DATA_RJ_C)
                || (pSpoolRct->aDataCtrl.ucDataID == SPBID_PRINT_DATA_RJ_E)
                || (pPrtCtrl.ucSndSeqNo == pPrtCtrl.ucRcvSeqNo)) {
                    SndSpecialFunction(PMG_PRT_RECEIPT, pSpoolRct);

                } else {
                    pPrtCtrl.ucStatSend |= SPLST_2BUFF;
                    PmgSleep(ID_SEND, WAIT_BUFF, 0);
                    pPrtCtrl.ucStatSend &= (0xff - SPLST_2BUFF);
                }

            } else if (ucStatJnl == SPL_SPC_FNC) {
                PmgReleaseSemRJ();
                if ((pSpoolJnl->aDataCtrl.ucDataID == SPBID_PRINT_DATA_RJ_C)
                || (pSpoolJnl->aDataCtrl.ucDataID == SPBID_PRINT_DATA_RJ_E)
                || (pPrtCtrl.ucSndSeqNo == pPrtCtrl.ucRcvSeqNo)) {
                    SndSpecialFunction(PMG_PRT_JOURNAL, pSpoolJnl);

                } else {
                    pPrtCtrl.ucStatSend |= SPLST_2BUFF;
                    PmgSleep(ID_SEND, WAIT_BUFF, 0);
                    pPrtCtrl.ucStatSend &= (0xff - SPLST_2BUFF);
                }

            } else {
                PmgReleaseSemRJ();
                SndPrintFunctionRJ(ucStatRct, ucStatJnl, pSpoolRct, pSpoolJnl);
            }
        }
    }
}

/*
*******************************************************************************
*
**Synopsis:     VOID SndPrintFunctionRJ(ucStatRct, ucStatJnl,
*                                               pSpoolRct, pSpoolJnl)
*
*               input   UCHAR       ucStatRct;  : receipt status
*               input   UCHAR       ucStatJnl;  : journal status
*               input   SPOOLDAT    *pSpoolRct; : pointer to print buffer
*               input   SPOOLDAT    *pSpoolJnl; : pointer to print buffer
*
**Return:       Non
*
**Description:  print normal data (receipt & journal)
*
*******************************************************************************
*/

VOID SndPrintFunctionRJ(UCHAR ucStatRct, UCHAR ucStatJnl,
                        SPOOLDAT *pSpoolRct, SPOOLDAT *pSpoolJnl)
{
    SHORT       sRc;            /* return status                */


    PmgRequestSemRJ();

    /*--- check & wait both print data ---*/
    if (!((ucStatRct == SPL_DATA) && (ucStatJnl == SPL_DATA))) {
        if (ucStatRct == SPL_DATA) {
            if (pPrtCtrl.ucInputSeqNo
            == pSpoolRct->aDataCtrl.ucInputSeqNo+(UCHAR)1) {
                PmgReleaseSemRJ();
                PmgSleep(ID_SEND, WAIT_JNL_BUFF, PMG_TIM_SPOOL);
                PmgRequestSemRJ();
                ucStatJnl = SndGetSpoolData(PMG_PRT_JOURNAL, &pSpoolJnl);
            }
        } else {
            if (pPrtCtrl.ucInputSeqNo
            == pSpoolJnl->aDataCtrl.ucInputSeqNo+(UCHAR)1) {
                PmgReleaseSemRJ();
                PmgSleep(ID_SEND, WAIT_RCT_BUFF, PMG_TIM_SPOOL);
                PmgRequestSemRJ();
                ucStatRct = SndGetSpoolData(PMG_PRT_RECEIPT, &pSpoolRct);
            }
        }
    }

    /*--- check ESC code in print buffer ---*/
    if ((ucStatRct == SPL_DATA) && (ucStatJnl == SPL_DATA)) {
        if ((SndChkEsc(pSpoolJnl) == TRUE)
        || (SndChkEsc(pSpoolRct) == TRUE)) {
            if (PmgCmpSequence(pSpoolJnl->aDataCtrl.ucInputSeqNo, 
            pSpoolRct->aDataCtrl.ucInputSeqNo) > 0) {
                ucStatJnl = SPL_NO_DATA;
            } else {
                ucStatRct = SPL_NO_DATA;
            }
        }
    }

    /*--- check special function in spool buffer ---*/
    if ((ucStatRct == SPL_DATA) && (ucStatJnl == SPL_DATA)) {
        if (SndChkSpoolDataID(PMG_PRT_RECEIPT,
        pSpoolJnl->aDataCtrl.ucInputSeqNo) == TRUE) {
            ucStatJnl = SPL_NO_DATA;
        } else if (SndChkSpoolDataID(PMG_PRT_JOURNAL,
        pSpoolRct->aDataCtrl.ucInputSeqNo) == TRUE) {
            ucStatRct = SPL_NO_DATA;
        }
    }

    /*--- memory the special ESC data ---*/
/*    if ((ucStatRct == SPL_NO_DATA) && (ucStatJnl == SPL_NO_DATA)) { del 2-25-97 */
        if (ucStatRct == SPL_DATA) {
            PmgMemoryEsc(PMG_PRT_RECEIPT, pSpoolRct->ucPrtData, (UCHAR)(pSpoolRct->aDataCtrl.ucVLI-sizeof(DATACTRL)));

        } else if (ucStatJnl == SPL_DATA) {
            PmgMemoryEsc(PMG_PRT_JOURNAL, pSpoolJnl->ucPrtData, (UCHAR)(pSpoolJnl->aDataCtrl.ucVLI-sizeof(DATACTRL)));

        }
/*    } */

    /*--- increment print tally ---*/
    if (ucStatRct == SPL_DATA) SndPrtTally(PMG_PRT_RECEIPT, pSpoolRct);
    if (ucStatJnl == SPL_DATA) SndPrtTally(PMG_PRT_JOURNAL, pSpoolJnl);

    PmgReleaseSemRJ();
    sRc = PmgSendData(ucStatRct,
                      ucStatJnl,
                      pSpoolRct,
                      pSpoolJnl);
    PmgRequestSemRJ();

    /*--- set pointer to next ---*/
    if (ucStatRct == SPL_DATA) {
        if (sRc == PIF_ERROR_COM_POWER_FAILURE) {
            SndNextSpoolPtr(PMG_PRT_RECEIPT, SPBST_POWERFAIL);

        } else if (sRc < PIF_ERROR_COM_POWER_FAILURE) {
            SndNextSpoolPtr(PMG_PRT_RECEIPT, SPBST_ERROR);

        } else {
            SndNextSpoolPtr(PMG_PRT_RECEIPT, SPBST_EXECUTED);
        }
    }
    if (ucStatJnl == SPL_DATA) {
        if (sRc == PIF_ERROR_COM_POWER_FAILURE) {
            SndNextSpoolPtr(PMG_PRT_JOURNAL, SPBST_POWERFAIL);

        } else if (sRc < PIF_ERROR_COM_POWER_FAILURE) {
            SndNextSpoolPtr(PMG_PRT_JOURNAL, SPBST_ERROR);

        } else {
            SndNextSpoolPtr(PMG_PRT_JOURNAL, SPBST_EXECUTED);
        }
    }

    /*-- count error tally ---*/
    if (sRc < PIF_ERROR_COM_POWER_FAILURE) {
        pPrtCtrl.pPrtTally->usCommError++;
    }

    /*--- wake up receive thread ---*/
    PmgWakeUp(ID_RECEIVE);

    PmgReleaseSemRJ();
    return;
}
#endif  /* defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION) */


#if defined (STATION_SLIP)
/*
*********************************************************************
*
**Synopsis:     VOID    PmgSendS(VOID)
*
**Return:       Non
*
**Description:  Send spool data to printer (slip)
*
*********************************************************************
*/

VOID PmgSendS(VOID)
{
    UCHAR       ucStatSlp;          /* status of slip spool     */
    SPOOLDAT    *pSpoolSlp;         /* pointer to slip buffer   */


    for (;;) {

        /*--- spool read/write semaphore ---*/
        PmgRequestSemS();

        /*--- get print data from spool ---*/
        ucStatSlp = SndGetSpoolData(PMG_PRT_SLIP, &pSpoolSlp);

        if (ucStatSlp == SPL_NO_DATA) {
            PmgReleaseSemS();
            return;

        } else if (ucStatSlp == SPL_ERR_RCV) {
            /*--- for receive ---*/
            pPrtCtrl.ucStatSend |= SPLST_ERROR;
            PmgReleaseSemS();
            PmgWakeUp(ID_RECEIVE);
            PifSleep(50);
            pPrtCtrl.ucStatSend &= (0xff - SPLST_ERROR);

        } else if (ucStatSlp == SPL_ERR_IMP) {
            SndNextSpoolPtr(PMG_PRT_SLIP, SPBST_SKIP);
            PmgWakeUp(ID_RECEIVE);
            PmgReleaseSemS();

#if !defined (HI_SPEED)
        } else if ((UCHAR)(pPrtCtrl.ucSndSeqNo-pPrtCtrl.ucRcvSeqNo) >= 2) {
            pPrtCtrl.ucStatSend |= SPLST_2BUFF;
            PmgReleaseSemS();
            PmgWakeUp(ID_RECEIVE);
            PmgSleep(ID_SEND, WAIT_BUFF, 0);
            pPrtCtrl.ucStatSend &= (0xff - SPLST_2BUFF);
#endif

        } else if (ucStatSlp == SPL_SPC_FNC) {
            PmgReleaseSemS();
            if (pPrtCtrl.ucSndSeqNo == pPrtCtrl.ucRcvSeqNo) {
                SndSpecialFunction(PMG_PRT_SLIP, pSpoolSlp);

            } else {
                pPrtCtrl.ucStatSend |= SPLST_2BUFF;
                PmgSleep(ID_SEND, WAIT_BUFF, 0);
                pPrtCtrl.ucStatSend &= (0xff - SPLST_2BUFF);
            }

        } else {
            PmgReleaseSemS();
            SndPrintFunctionS(pSpoolSlp);

        }
    }
}

/*
*******************************************************************************
*
**Synopsis:     VOID SndPrintFunctionS(pSpoolSpl)
*
*               input   SPOOLDAT    *pSpoolSpl; : pointer to print buffer
*
**Return:       Non
*
**Description:  print normal data (slip)
*
*******************************************************************************
*/

VOID SndPrintFunctionS(SPOOLDAT *pSpoolSlp)
{
    PRT_SNDMSG  aSendBuff;
    SHORT       usDataLen, usEscLen, sRc;
#if 0
    ESC4BYTE    EscSlipWait  = {ESC, 'f', 0, 0};
    ESC4BYTE    EscSelSensor = {ESC, 'c', '4', 0x30};
#endif
    PRTCTRL     *pPrintCtrl;         /* Printer control block    */
    SPOOLCNT    *pSpoolCtrl;            /* spool control block      */
//    SPOOLDAT    *pSpoolData;


    PmgGetCtrl(PMG_PRT_SLIP, &pPrintCtrl, &pSpoolCtrl);

    PmgRequestSemS();

    /*--- set IDENT = 0 ---*/
    aSendBuff.auchIDENT[0]
        = aSendBuff.auchIDENT[1]
        = aSendBuff.auchIDENT[2]
        = 0;
    pSpoolSlp->aDataCtrl.ucCommSeqNo
        = aSendBuff.ucSeqNo
        = pPrtCtrl.ucSndSeqNo;

    /*--- select slip station ---*/
    usEscLen = 0;
	//US Customs slip printer control change for short forms
	if (!(pSpoolCtrl->fbModeInput & SPLMD_SMALL_VALIDATION)) {
		memcpy(&aSendBuff.aucPrtMsg[usEscLen], aucSelPrt, sizeof(aucSelPrt));
		aSendBuff.aucPrtMsg[usEscLen+3] = PMG_SEL_SLIP;
		usEscLen += sizeof(aucSelPrt);
	}
#if 0
    memcpy(&aSendBuff.aucPrtMsg[usEscLen], &EscSlipWait, sizeof(EscSlipWait));
    usEscLen += sizeof(EscSlipWait);

    memcpy(&aSendBuff.aucPrtMsg[usEscLen], &EscSelSensor, sizeof(EscSelSensor));
    usEscLen += sizeof(EscSelSensor);
#endif
    /*--- copy print data ---*/
    usDataLen = pSpoolSlp->aDataCtrl.ucVLI-sizeof(DATACTRL);
    memcpy(&aSendBuff.aucPrtMsg[usEscLen], &pSpoolSlp->ucPrtData[0], usDataLen);

    /*--- increment print tally ---*/
    SndPrtTally(PMG_PRT_SLIP, pSpoolSlp);

    /*--- memory the special ESC data ---*/
    PmgMemoryEsc(PMG_PRT_SLIP, pSpoolSlp->ucPrtData, usDataLen);

    PmgReleaseSemS();

    sRc = PmgWriteCom((USHORT)PMG_PRT_SLIP, (VOID FAR *)&aSendBuff, (USHORT)(4+usDataLen+usEscLen));

    PmgRequestSemS();

    if (sRc == PIF_ERROR_COM_POWER_FAILURE) {
        SndNextSpoolPtr(PMG_PRT_SLIP, SPBST_POWERFAIL);

    } else if (sRc < PIF_ERROR_COM_POWER_FAILURE) {
        SndNextSpoolPtr(PMG_PRT_SLIP, SPBST_ERROR);
        /*-- count error tally ---*/
        pPrtCtrl.pPrtTally->usCommError++;

    } if (sRc > 0) {
        /*--- if success then increment sequnce number ---*/
        pPrtCtrl.ucSndSeqNo++;
        SndNextSpoolPtr(PMG_PRT_SLIP, SPBST_EXECUTED);
    }

    /*--- wake up receive thread ---*/
    PmgWakeUp(ID_RECEIVE);

    PmgReleaseSemS();
    return;
}
#endif  /* defined (STATION_SLIP) */

/*
*******************************************************************************
*
**Synopsis:     VOID SndSpecialFunction(usPrtType, pSpoolBuf)
*
*               input   USHORT      usPrtType;  : printer ID
*               input   SPOOLDAT    *pSpoolBuf; : pointer to print buffer
*
**Return:       Non
*
**Description:  Execute special print function
*
*******************************************************************************
*/

VOID SndSpecialFunction(USHORT usPrtType, SPOOLDAT *pSpoolBuf)
{
    SHORT   sRc=1;


    switch (pSpoolBuf->aDataCtrl.ucDataID) {
    case SPBID_BEGIN_VALIDATION:
        sRc = SndBeginValidation(usPrtType);
        break;
    case SPBID_END_VALIDATION:
        SndEndValidation(usPrtType);
        break;
    case SPBID_BEGIN_IMPORTANT:
        SndBeginImportant(usPrtType);
        break;
    case SPBID_END_IMPORTANT:
        SndEndImportant(usPrtType);
        break;
    case SPBID_PRINT_DATA_RJ_C:
    case SPBID_PRINT_DATA_RJ_E:
        sRc = SndPrintRJ(usPrtType, pSpoolBuf);
        break;
    case SPBID_FONT_A:
    case SPBID_FONT_B:
        sRc = SndFontType(usPrtType,
            pSpoolBuf->aDataCtrl.ucDataID,
            pSpoolBuf);
        break;
    default:
        break;
    }

    PmgRequestSem(usPrtType);

    if (sRc == PIF_ERROR_COM_POWER_FAILURE) {
        SndNextSpoolPtr(usPrtType, SPBST_POWERFAIL);

    } else if (sRc < PIF_ERROR_COM_POWER_FAILURE) {
        SndNextSpoolPtr(usPrtType, SPBST_ERROR);
        /*-- count error tally ---*/
        if (usPrtType == PMG_PRT_SLIP) {
            pPrtCtrl.pPrtTally->usCommError++;
        } else {
            pPrtCtrl.pPrtTally->usCommError++;
        }

    } if (sRc > 0) {
        SndNextSpoolPtr(usPrtType, SPBST_EXECUTED);
    }

    /*--- wake up receive thread ---*/
    PmgWakeUp(ID_RECEIVE);

    PmgReleaseSem(usPrtType);
}


#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
/*
*******************************************************************************
*
**Synopsis:     SHORT SndPrintRJ(usPrtType, pSpoolBuff)
*
*               input   USHORT      usPrtType;      : printer ID
*               input   SPOOLDAT    *pSpoolBuff;    : pointer to print buffer
*
*
**Return:       Pif return status
*
**Description:  print R&J normal data
*
*******************************************************************************
*/

SHORT SndPrintRJ(USHORT usPrtType, SPOOLDAT *pSpoolBuf)
{
    SHORT   sRc;            /* return status                */
    USHORT  usSendLen;      /* send data length             */
    PRT_SNDMSG  aSendBuff;


    PmgRequestSemRJ();

    if (PmgChkCurStat(usPrtType, SPLMD_VALIDATION) == 0) {
#if defined (PROVIDED_SAME_PRINT)
        /*--- set receipt & journal print ESC ---*/
        pSpoolCtrlR.fbModeSpool |= SPLMD_RCT_JNL;
        memcpy(&aSendBuff.aucPrtMsg[0], aucSameOn, sizeof(aucSameOn));
        usSendLen = sizeof(aucSameOn);
#else
        usSendLen = 0;
#endif
    } else {
        /*--- no printer selection for validation ---*/
        usSendLen = 0;
    }

    /*--- set print data ---*/
    memcpy(&aSendBuff.aucPrtMsg[usSendLen], &pSpoolBuf->ucPrtData[0],
        pSpoolBuf->aDataCtrl.ucVLI-sizeof(DATACTRL));
    usSendLen += pSpoolBuf->aDataCtrl.ucVLI-sizeof(DATACTRL);

    /*--- set IDENT = 0 ---*/
    aSendBuff.auchIDENT[0]
        = aSendBuff.auchIDENT[1]
        = aSendBuff.auchIDENT[2]
        = 0;
    pSpoolBuf->aDataCtrl.ucCommSeqNo
        = aSendBuff.ucSeqNo
        = pPrtCtrl.ucSndSeqNo;

    /*--- increment print tally ---*/
    SndPrtTally(PMG_PRT_RECEIPT, pSpoolBuf);
#if defined (STATION_JOURNAL)
    SndPrtTally(PMG_PRT_JOURNAL, pSpoolBuf);
#endif

    PmgReleaseSemRJ();

    sRc = PmgWriteCom((USHORT)PMG_PRT_RCT_JNL, (VOID FAR *)&aSendBuff, (USHORT)(4+usSendLen));

    PmgRequestSemRJ();

    /*--- if success then increment sequnce number ---*/
    if (sRc > 0) pPrtCtrl.ucSndSeqNo++;

    PmgReleaseSemRJ();
    return(sRc);
}
#endif /* if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION) */


/*
*******************************************************************************
*
**Synopsis:     SHORT SndBeginValidation(usPrtType)
*
*               input   USHORT  usPrtType;  : printer ID
*
*
**Return:       Pif return status
*
**Description:  Begin of validation
*
*******************************************************************************
*/

SHORT SndBeginValidation(USHORT usPrtType)
{
    UCHAR   pfbStatus;
    SHORT   sRc;
    USHORT  usRetryCount;
    PmgRequestSem(usPrtType);
    usRetryCount = usSaveValRetryCount-1;

    if (usPrtType != PMG_PRT_SLIP) SndOpenPlaten(usPrtType, 3);
    for (;;) {
        do {
            /*--- open platen ---*/
            sRc = SndOpenPlaten(usPrtType, 1);
            if (sRc == PIF_ERROR_COM_POWER_FAILURE) break;
            if (sRc < PIF_ERROR_COM_POWER_FAILURE) {
                SndCommError(usPrtType);
                continue;
            }
            /*--- request insert the paper ---*/
            PmgReleaseSem(usPrtType);
            PmgErrorMsg(DEVICEIO_PRINTER_MSG_VSLPINS_ADR);
            PmgRequestSem(usPrtType);
            /*--- paper check ----*/
            if (usPrtType == PMG_PRT_SLIP) {
#if 1
                sRc = PmgReadStatus(usPrtType,&pfbStatus);
#else
                sRc = EscpControlCom(pPrtCtrl.hPort,
                                    PIF_PIP_GET_STATUS,
                                    (UCHAR FAR *)&pfbStatus,
                                    sizeof(pfbStatus));
#endif
            } else {
#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
                /* --- if no censor (1 line validation) then no check --- */
                if ((pSpoolCtrlR.fbConfig & PMG_VALIDATION) == 0) break;
                if (usSaveValRetryCount != 0) {
                    if (usRetryCount == 0) break;
                    usRetryCount--;
                }    
#endif

#if 1
                sRc = PmgReadStatus(usPrtType,&pfbStatus);
#else
                sRc = EscpControlCom(pPrtCtrl.hPort,
                                    PIF_PIP_GET_STATUS,
                                    (UCHAR FAR *)&pfbStatus,
                                    sizeof(pfbStatus));
#endif
            }
            if (sRc == PIF_ERROR_COM_POWER_FAILURE) break;
            if (sRc < PIF_ERROR_COM_POWER_FAILURE) {
                SndCommError(usPrtType);
                continue;
            }
        } while ((pfbStatus & PRT_STAT_FORM_IN) == 0);
        if (sRc == PIF_ERROR_COM_POWER_FAILURE) break;

        /*--- close platen ---*/
        sRc = SndClosePlaten(usPrtType);
        if (sRc > 0) break;
        if (sRc == PIF_ERROR_COM_POWER_FAILURE) break;

    }

    if (usPrtType == PMG_PRT_SLIP) {
#if defined (STATION_SLIP)
        /*--- enter validation print ---*/
        PmgSetCurStat(PMG_PRT_SLIP, SPLMD_VALIDATION);
        pSpoolCtrlS.pucValidation = pSpoolCtrlS.pucSend;
#endif
    } else {
#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
        /*--- enter validation print ---*/
        PmgSetCurStat(PMG_PRT_RCT_JNL, SPLMD_VALIDATION);
        pSpoolCtrlR.pucValidation = pSpoolCtrlR.pucSend;
        pSpoolCtrlJ.pucValidation = pSpoolCtrlJ.pucSend;
#endif
    }

    PmgReleaseSem(usPrtType);
    return(sRc);
}


/*
*******************************************************************************
*
**Synopsis:     VOID SndEndValidation(usPrtType)
*
*               input   USHORT  usPrtType;  : printer ID
*
*
**Return:       Non
*
**Description:  End of validation
*
*******************************************************************************
*/

VOID SndEndValidation(USHORT usPrtType)
{
    PmgRequestSem(usPrtType);

    /*--- open platen ---*/
    if (usPrtType == PMG_PRT_SLIP) SndOpenPlaten(usPrtType, 1);
    /*--- request remove the paper ---*/
    PmgReleaseSem(usPrtType);
    PmgErrorMsg(DEVICEIO_PRINTER_MSG_VSLPREMV_ADR);
    PmgRequestSem(usPrtType);

    if (usPrtType == PMG_PRT_SLIP) {
        /*--- exit validation print ---*/
        PmgResetCurStat(PMG_PRT_SLIP, SPLMD_VALIDATION);
    } else {
        SndOpenPlaten(usPrtType, 2);
        /*--- exit validation print ---*/
        PmgResetCurStat(PMG_PRT_RCT_JNL, SPLMD_VALIDATION);
    }

    PmgReleaseSem(usPrtType);
}


/*
*******************************************************************************
*
**Synopsis:     VOID SndBeginImportant(usPrtType)
*
*               input   USHORT  usPrtType;  : printer ID
*
**Return:       Non
*
**Description:  Begin of important lines
*
*******************************************************************************
*/

VOID SndBeginImportant(USHORT usPrtType)
{
    PmgRequestSem(usPrtType);
    /*--- enter important lines ---*/
    PmgSetCurStat(usPrtType, SPLMD_IMPORTANT);
    PmgReleaseSem(usPrtType);
}


/*
*******************************************************************************
*
**Synopsis:     VOID SndEndImportant(usPrtType)
*
*               input   USHORT  usPrtType;  : printer ID
*
**Return:       Non
*
**Description:  End of important lines
*
*******************************************************************************
*/

VOID SndEndImportant(USHORT usPrtType)
{
    UCHAR   ucStat;


    PmgRequestSem(usPrtType);
    /*--- exit important lines ---*/
    ucStat = PmgGetCurStat(usPrtType);
    PmgResetCurStat(usPrtType, SPLMD_IMPORTANT|SPLMD_IMP_ERROR);
    ucStat &= SPLMD_IMP_ERROR;
    PmgSetInputStat(usPrtType, ucStat);

    switch (usPrtType) {
        case PMG_PRT_RECEIPT:
            PifReleaseSem(hPmgImpRctSem);
            break;
        case PMG_PRT_JOURNAL:
            PifReleaseSem(hPmgImpJnlSem);
            break;
        case PMG_PRT_SLIP:
            PifReleaseSem(hPmgImpSlpSem);
            break;                      
        default:
            break;
    }
    PmgReleaseSem(usPrtType);
}


/*
*******************************************************************************
*
**Synopsis:     SHORT SndFontType(usPrtType, ucFontType, *pSpoolBuf)
*
*               input   USHORT      usPrtType;  : printer ID
*               input   UCHAR       ucFontType; : font type
*               input   SPOOLDAT    *pSpoolBuff;    : pointer to print buffer
*
**Return:       Pif return status
*
**Description:  set to font type (EPSON thermal printer)
*
*******************************************************************************
*/

SHORT SndFontType(USHORT usPrtType, UCHAR ucFontType, SPOOLDAT *pSpoolBuf)
{
    SHORT       sRc;            /* return status            */
    PRT_SNDMSG  aSendBuff;

    PmgRequestSemRJ();

    /*--- set font data ---*/
    memcpy(&aSendBuff.aucPrtMsg[0], &aucFontEsc, sizeof(aucFontEsc));
    if (ucFontType != SPBID_FONT_B) {
        PmgResetCurStat(usPrtType, SPLMD_FONT_B);
        ucFontTypeRJ = 0;
        ucFeedLenRJ = PMG_SET_DEF_LF;
    } else {
        aSendBuff.aucPrtMsg[2] = PMG_SET_FONT_B;    /* specify BOLD font */
#if defined(DEVICE_7158)
        aSendBuff.aucPrtMsg[9] = PMG_SET_LF;        /* [5] -> [9] */
#else
        aSendBuff.aucPrtMsg[5] = PMG_SET_LF;
#endif
        PmgSetCurStat(usPrtType, SPLMD_FONT_B);
        ucFontTypeRJ = PMG_SET_FONT_B;
        ucFeedLenRJ = PMG_SET_LF;
    }

#if defined(DEVICE_7158)
    /* 7158 printer has slip station and receipt station. */
    /* LF pitch select command requiers station selection */
    if (usPrtType == PMG_PRT_SLIP) {
        aSendBuff.aucPrtMsg[6] = 0x04;
    }
    else { /* in the case of receipt */
        aSendBuff.aucPrtMsg[6] = 0x02;
    }
#endif

    /*--- set IDENT = 0 ---*/
    aSendBuff.auchIDENT[0]
        = aSendBuff.auchIDENT[1]
        = aSendBuff.auchIDENT[2]
        = 0;
    pSpoolBuf->aDataCtrl.ucCommSeqNo
        = aSendBuff.ucSeqNo
        = pPrtCtrl.ucSndSeqNo;

    PmgReleaseSemRJ();

    sRc = PmgWriteCom(PMG_PRT_RCT_JNL, &aSendBuff, 4+sizeof(aucFontEsc));

    PmgRequestSemRJ();

    /*--- if success then increment sequnce number ---*/
    if (sRc > 0) pPrtCtrl.ucSndSeqNo++;

    PmgReleaseSemRJ();
    return(sRc);
}


/*
*********************************************************************
*
**Synopsis:     USHORT  SndGetSpoolData(usPrtType, pBuffer)
*               input   USHORT      usPrtType;  : printer ID
*               output  SPOOLDAT    **pBuffer;  : data buffer
*
**Return:       Data type
*
**Description:  Get spool data
*
*********************************************************************
*/

UCHAR SndGetSpoolData(USHORT usPrtType, SPOOLDAT **pBuffer)

{
    PRTCTRL     *pPrintCtrl;         /* Printer control block    */
    SPOOLCNT    *pSpoolCtrl;            /* spool control block      */
    SPOOLDAT    *pSpoolData;


    PmgGetCtrl(usPrtType, &pPrintCtrl, &pSpoolCtrl);
    /*--- get next pointer ---*/
    pSpoolData
        = *pBuffer
        = (SPOOLDAT *)(pSpoolCtrl->pucSend);
    if (pSpoolData->aDataCtrl.ucDataID == SPBID_END_SPOOL) {
        /*--- if buffer end then loop the pointer ---*/
        pSpoolData
            = *pBuffer
            = (SPOOLDAT *)(pSpoolCtrl->pucStart);
    }

    if (pSpoolCtrl->pucSend == pSpoolCtrl->pucWrite) {
        /*--- return if no data ---*/
        return(SPL_NO_DATA);
    }
    if ((pPrintCtrl->ucStatReceive & SPLST_ERROR) != 0) {
        /*--- receive thread in error condition ---*/
        return(SPL_ERR_RCV);
    }
    if ((pSpoolCtrl->fbModeSpool & SPLMD_IMP_ERROR) != 0) {
        /*--- error occured in important lines ---*/
        if (pSpoolData->aDataCtrl.ucDataID == SPBID_END_IMPORTANT) {
            return(SPL_SPC_FNC);
        } else {
            return(SPL_ERR_IMP);
        }
    }
    if (pSpoolData->aDataCtrl.ucDataID != SPBID_PRINT_DATA_C) {
        if (pSpoolData->aDataCtrl.ucDataID != SPBID_PRINT_DATA_E) {
            /*-- return if special function ---*/
            return(SPL_SPC_FNC);
        }
    }

    return(SPL_DATA);
}


#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
/*
*******************************************************************************
*
**Synopsis:     PmgSendData(ucStatRct, ucStatJnl, pSpoolRct, pSpoolJnl)
*
*               input   UCHAR       ucStatRct;  : receipt status
*               input   UCHAR       ucStatJnl;  : journal status
*               input   PSPOOLDAT   pSpoolRct;  : pointer to print buffer
*               input   PSPOOLDAT   pSpoolJnl;  : pointer to print buffer
*
**Return:       Data type
*
**Description:  Send print data to printer
*
*******************************************************************************
*/

SHORT PmgSendData(UCHAR ucStatRct, UCHAR ucStatJnl,
                    SPOOLDAT *pSpoolRct, SPOOLDAT *pSpoolJnl)

{
    PRT_SNDMSG  aSendBuff;
    SHORT   usDataLen, usEscLen, sRc;


    usEscLen = 0;

    if ((ucStatRct == SPL_DATA) && (ucStatJnl == SPL_DATA)) {
        /********************************/
        /*--- send receipt & journal ---*/
        /********************************/
        if ((pSpoolCtrlR.fbModeSpool & SPLMD_RCT_JNL) != 0) {
            memcpy(&aSendBuff.aucPrtMsg[usEscLen],
                aucSameOff,
                sizeof(aucSameOff));
            usEscLen += sizeof(aucSameOff);
            pSpoolCtrlR.fbModeSpool &= 0xff - SPLMD_RCT_JNL;
        }
        /*--- set printer selection ---*/
        memcpy(&aSendBuff.aucPrtMsg[usEscLen], aucSelPrt, sizeof(aucSelPrt));
        aSendBuff.aucPrtMsg[usEscLen+3] = (PMG_SEL_RECEIPT | PMG_SEL_JOURNAL);
        usEscLen += sizeof(aucSelPrt);
        /*--- set up receipt data ---*/
        usDataLen = pSpoolRct->aDataCtrl.ucVLI-sizeof(DATACTRL);
        /*--- remove LF in receipt data ---*/
        if (pSpoolRct->ucPrtData[usDataLen-1] == LF) usDataLen--;
        memcpy(&aSendBuff.aucPrtMsg[usEscLen],
            &pSpoolRct->ucPrtData[0],
            usDataLen);

        if (pSpoolCtrlR.ucMaxChar > (UCHAR)usDataLen) {
            memset(&aSendBuff.aucPrtMsg[usEscLen+usDataLen],
                ' ',
                pSpoolCtrlR.ucMaxChar-usDataLen);
        }

        /*--- set up journal data ---*/
        usDataLen = pSpoolJnl->aDataCtrl.ucVLI - sizeof(DATACTRL);
        memcpy(&aSendBuff.aucPrtMsg[usEscLen+pSpoolCtrlR.ucMaxChar],
            &pSpoolJnl->ucPrtData[0],
            usDataLen);
        usDataLen += pSpoolCtrlR.ucMaxChar;

        /*--- set communication sequence number ---*/
        pSpoolRct->aDataCtrl.ucCommSeqNo
            = pSpoolJnl->aDataCtrl.ucCommSeqNo
            = pPrtCtrl.ucSndSeqNo;
    } else {
        if (ucStatRct == SPL_DATA) {
            /**********************/
            /*--- send receipt ---*/
            /**********************/
            if (PmgChkCurStat(PMG_PRT_RECEIPT, SPLMD_VALIDATION) == 0) {
                usEscLen = 0;
#if (NUMBER_OF_STATION > 1)
                memcpy(&aSendBuff.aucPrtMsg[usEscLen], aucSelPrt, sizeof(aucSelPrt));
                usEscLen += sizeof(aucSelPrt);
                aSendBuff.aucPrtMsg[usEscLen-1] = PMG_SEL_RECEIPT;
#endif
            } else {
                /*--- no printer selection for validation ---*/
            }
            usDataLen = pSpoolRct->aDataCtrl.ucVLI-sizeof(DATACTRL);
            memcpy(&aSendBuff.aucPrtMsg[usEscLen],
                &pSpoolRct->ucPrtData[0],
                usDataLen);
            /*--- set communication sequence number ---*/
            pSpoolRct->aDataCtrl.ucCommSeqNo = pPrtCtrl.ucSndSeqNo;

        } else if (ucStatJnl == SPL_DATA) {
            /**********************/
            /*--- send journal ---*/
            /**********************/
            if (PmgChkCurStat(PMG_PRT_JOURNAL, SPLMD_VALIDATION) == 0) {
                usEscLen = 0;
#if (NUMBER_OF_STATION > 1)
                memcpy(&aSendBuff.aucPrtMsg[usEscLen], aucSelPrt, sizeof(aucSelPrt));
                usEscLen += sizeof(aucSelPrt);
                aSendBuff.aucPrtMsg[usEscLen-1] = PMG_SEL_JOURNAL;
#endif
            } else {
                /*--- no printer selection for validation ---*/
            }
            usDataLen = pSpoolJnl->aDataCtrl.ucVLI-sizeof(DATACTRL);
            memcpy(&aSendBuff.aucPrtMsg[usEscLen],
                &pSpoolJnl->ucPrtData[0],
                usDataLen);
            /*--- set communication sequence number ---*/
            pSpoolJnl->aDataCtrl.ucCommSeqNo = pPrtCtrl.ucSndSeqNo;
        }
    }

    /*--- set IDENT = 0 ---*/
    aSendBuff.auchIDENT[0]
        = aSendBuff.auchIDENT[1]
        = aSendBuff.auchIDENT[2]
        = 0;
    /*--- set communication sequence number ---*/
    aSendBuff.ucSeqNo = pPrtCtrl.ucSndSeqNo;
    usDataLen += usEscLen;

    sRc = PmgWriteCom((USHORT)PMG_PRT_RCT_JNL, (VOID FAR *)&aSendBuff, (USHORT)(4+usDataLen));

    /*--- if success then increment sequnce number ---*/
    if (sRc > 0) pPrtCtrl.ucSndSeqNo++;

    return(sRc);
}


/*
*******************************************************************************
*
**Synopsis:     SndChkSpoolDataID(usPrtType, ucInputSeqNo)
*
*               input   USHORT  usPrtType;      : printer type
*               input   UCHAR   ucInputSeqNo;   : input sequence number
*
**Return:       TRUE    : exist special function
*               FALSE   : no special function
*
**Description:  Check spool data
*
*******************************************************************************
*/

USHORT SndChkSpoolDataID(USHORT usPrtType, UCHAR ucInputSeqNo)
{
    SPOOLCNT    *pSpoolCnt;
    SPOOLDAT    *pSpoolData;
    USHORT      i;
    

    if (usPrtType == PMG_PRT_RECEIPT) pSpoolCnt = &pSpoolCtrlR;
    else if (usPrtType == PMG_PRT_JOURNAL) pSpoolCnt = &pSpoolCtrlJ;
    else return(FALSE);

    pSpoolData = (SPOOLDAT *)pSpoolCnt->pucSend;
    while ((UCHAR *)pSpoolData != pSpoolCnt->pucWrite) {
        if (pSpoolData->aDataCtrl.ucDataID == SPBID_END_SPOOL) {
            pSpoolData = (SPOOLDAT *)pSpoolCnt->pucStart;
        }
        if ((pSpoolData->aDataCtrl.ucDataID != SPBID_PRINT_DATA_C)
        && (pSpoolData->aDataCtrl.ucDataID != SPBID_PRINT_DATA_E)) {
            break;
        } else {
            for (i=0; i<pSpoolData->aDataCtrl.ucVLI-sizeof(DATACTRL)-1; i++) {
                if (pSpoolData->ucPrtData[i] == ESC) break;
            }
            if (pSpoolData->ucPrtData[i] == ESC) break;
            pSpoolData = (SPOOLDAT *)((UCHAR *)pSpoolData
                            + pSpoolData->aDataCtrl.ucVLI);
        }
    }

    if ((UCHAR *)pSpoolData == pSpoolCnt->pucWrite) {
        return(FALSE);
    }
    if (PmgCmpSequence(ucInputSeqNo, pSpoolData->aDataCtrl.ucInputSeqNo) >= 0) {
        return(TRUE);
    } else {
        return(FALSE);
    }
}
#endif /* #if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION) */


/*
*******************************************************************************
*
**Synopsis:     SndNextSpoolPtr(usPrtType, fbExeMode)
*
*               input   USHORT  usPrtType;      : printer type
*               input   UCHAR   fbExeMode;      : execution mode
*
**Return:       Non
*
**Description:  Set next send pointer
*
*******************************************************************************
*/

VOID SndNextSpoolPtr(USHORT usPrtType, UCHAR fbExeMode)
{
    SPOOLCNT    *pSpoolCtrl = PmgGetSpoolCtrl(usPrtType);
    SPOOLDAT    *pSpoolData;


    if (usPrtType == PMG_PRT_RCT_JNL) {
        usPrtType = PMG_PRT_RECEIPT;
        pSpoolData = (SPOOLDAT *)pSpoolCtrl->pucSend;

        if (pSpoolData->aDataCtrl.ucDataID == SPBID_END_SPOOL) {
            pSpoolData = (SPOOLDAT *)pSpoolCtrl->pucStart;
            pSpoolCtrl->pucSend = pSpoolCtrl->pucStart;
        }

        pSpoolData->aDataCtrl.fbPrtMode = fbExeMode;
        pSpoolCtrl->pucSend += pSpoolData->aDataCtrl.ucVLI;

        usPrtType = PMG_PRT_JOURNAL;
    }

    pSpoolData = (SPOOLDAT *)pSpoolCtrl->pucSend;

    if (pSpoolData->aDataCtrl.ucDataID == SPBID_END_SPOOL) {
        pSpoolData = (SPOOLDAT *)pSpoolCtrl->pucStart;
        pSpoolCtrl->pucSend = pSpoolCtrl->pucStart;
    }

    pSpoolData->aDataCtrl.fbPrtMode = fbExeMode;
    pSpoolCtrl->pucSend += pSpoolData->aDataCtrl.ucVLI;
}


/*
*******************************************************************************
*
**Synopsis:     SHORT   SndOpenPlaten(usPrtType, usType)
*
*               input   USHORT  usPrtType;      : printer type
*               input   USHORT  usType;         : function type
*
**Return:       Pif return status
*
**Description:  Open platen
*
*******************************************************************************
*/

SHORT   SndOpenPlaten(USHORT usPrtType, USHORT usType)
{
    SHORT   sRc;
    UCHAR   aucSelect[20];
    PRT_RESPONCE    pPrtResponce;


    for (;;) {
        if (usPrtType == PMG_PRT_SLIP) {
            /*--- set up printer selection of validation station & return home ---*/
            aucSelect[0]
                = aucSelect[1]
                = aucSelect[2]
                = 0;
            aucSelect[3] = pPrtCtrl.ucSndSeqNo++;
            memcpy(&aucSelect[4], &aucSlpOpen[0], sizeof(aucSlpOpen));
            /*--- send ESC data to printer ---*/
            sRc = EscpWriteCom(pPrtCtrl.hPort, &aucSelect, 4+sizeof(aucSlpOpen));

            if (sRc < 0) return(sRc);

            do {
                sRc = PmgReadCom(usPrtType,
                                &pPrtResponce,
                                sizeof(pPrtResponce));
                if (sRc <= 0) break;
            } while (pPrtResponce.ucSeqNo != (UCHAR)(pPrtCtrl.ucSndSeqNo-1));
            pPrtCtrl.ucRcvSeqNo = pPrtCtrl.ucSndSeqNo;

        } else {
            /*--- set up printer selection of R/J station & return home ---*/
            aucSelect[0]
                = aucSelect[1]
                = aucSelect[2]
                = 0;
            aucSelect[3] = pPrtCtrl.ucSndSeqNo++;

            switch (usType) {
            case 1:
                memcpy(&aucSelect[4], aucRJHome, sizeof(aucRJHome));
                /*--- send return home to printer ---*/
                sRc = EscpWriteCom(pPrtCtrl.hPort, &aucSelect, 4+sizeof(aucRJHome));
                break;
            case 2:
                memcpy(&aucSelect[4], aucRJHome2, sizeof(aucRJHome2));
                /*--- send initialize to printer ---*/
                sRc = EscpWriteCom(pPrtCtrl.hPort, &aucSelect, 4+sizeof(aucRJHome2));
                break;
            default:
                memcpy(&aucSelect[4], aucRJHome3, sizeof(aucRJHome3));
                /*--- send dummy feed to printer ---*/
                sRc = EscpWriteCom(pPrtCtrl.hPort, &aucSelect, 4+sizeof(aucRJHome3));
                break;
            }

            do {
                sRc = PmgReadCom(usPrtType,
                                &pPrtResponce,
                                sizeof(pPrtResponce));
                if (sRc <= 0) break;
            } while (pPrtResponce.ucSeqNo != (UCHAR)(pPrtCtrl.ucSndSeqNo-1));
            pPrtCtrl.ucRcvSeqNo = pPrtCtrl.ucSndSeqNo;

        }
        if (sRc <= 0) break;
        if (((pPrtResponce.ucPrtStatus & PRT_STAT_ERROR) == 0)
        || ((pPrtResponce.ucPrtStatus & PRT_STAT_INIT_END) == 0)) {
            SndCommError(usPrtType);
        } else {
            break;
        }
    }

    return(sRc);
}


/*
*******************************************************************************
*
**Synopsis:     SHORT   SndClosePlaten(usPrtType)
*
*               input   USHORT  usPrtType;      : printer type
*
**Return:       Pif return status
*
**Description:  Close platen
*
*******************************************************************************
*/

SHORT SndClosePlaten(USHORT usPrtType)
{
    SHORT   sRc;
    UCHAR   aucSelect[4+sizeof(aucSelPrtVal)];
    PRT_RESPONCE    pPrtResponce;


    if (usPrtType == PMG_PRT_SLIP) {
        return(1);

    } else {
        /*--- send printer selection of validation ---*/
        memcpy(&aucSelect[4], aucSelPrtVal, sizeof(aucSelPrtVal));
        aucSelect[0]
            = aucSelect[1]
            = aucSelect[2]
            = 0;
        aucSelect[3] = pPrtCtrl.ucSndSeqNo++;
/*      aucSelect[4+3] = PMG_SEL_VALIDATION;    */
        sRc = EscpWriteCom(pPrtCtrl.hPort, &aucSelect, sizeof(aucSelect));
        if (sRc < 0) return(sRc);
        do {
            sRc = PmgReadCom(usPrtType,
                             &pPrtResponce,
                             sizeof(pPrtResponce));
            if (sRc <= 0) break;
        } while (pPrtResponce.ucSeqNo != (UCHAR)(pPrtCtrl.ucSndSeqNo-1));
        pPrtCtrl.ucRcvSeqNo = pPrtCtrl.ucSndSeqNo;
    }

    return(sRc);
}


/*
*******************************************************************************
*
**Synopsis:     SndCommError(usPrtType)
*
*               input   USHORT  usPrtType;      : printer type
*
**Return:       Non
*
**Description:  display the pritnter error
*       
**Caution!!! This function must request semaphore usPrtType before called.(99-2-17)
*******************************************************************************
*/

VOID SndCommError(USHORT usPrtType)
{

    PmgReleaseSem(usPrtType);   /* ----- modify 99-2-17 TAR#98428 BugFix ----- */

    if (usPrtType == PMG_PRT_SLIP) {
        /*--- error display & input key ---*/
        PmgErrorMsg(DEVICEIO_PRINTER_MSG_PRTTBL_ADR);

    } else {
        /*--- error display & input key ---*/
        PmgErrorMsg(DEVICEIO_PRINTER_MSG_PRTTBL_ADR);
    }

    PmgRequestSem(usPrtType);   /* ----- modify 99-2-17 TAR#98428 BugFix ----- */

    return;
}


/*
*******************************************************************************
*
**Synopsis:     USHORT SndChkEsc(pSpoolData)
*
*               input   SPOOLDAT    *pSpoolData;    : data of spool
*
**Return:       TRUE  : ESC data
*               FALSE : ascii data
*
**Description:  check ESC data in print buffer
*
*******************************************************************************
*/

USHORT SndChkEsc(SPOOLDAT *pSpoolData)
{
    UCHAR   i;

    for (i=0; i<pSpoolData->aDataCtrl.ucVLI-(UCHAR)sizeof(DATACTRL); i++) {
        if (pSpoolData->ucPrtData[i] == ESC) return(TRUE);
    }

    return(FALSE);
}


/*
*******************************************************************************
*
**Synopsis:     VOID SndPrtTally(usPrtType, pSpoolData)
*
*               input   USHORT      usPrtType;      : printer type
*                       SPOOLDAT    *pSpoolData;    : data of spool
*
**Return:       non
*
**Description:  increment printer tally
*
*******************************************************************************
*/

VOID SndPrtTally(USHORT usPrtType, SPOOLDAT *pSpoolData)
{
    USHORT          i, fFlag=0;
    PRTTALLY FAR    *pTally;


    for (i=0; i<(pSpoolData->aDataCtrl.ucVLI-sizeof(DATACTRL)); i++) {
        if (pSpoolData->ucPrtData[i] == ESC) {
            if ((pSpoolData->ucPrtData[i+1] == 't')     /* select code table     */
            || (pSpoolData->ucPrtData[i+1] == 'd')      /* feed                  */
            || (pSpoolData->ucPrtData[i+1] == 'h')      /* feed direction        */
            || (pSpoolData->ucPrtData[i+1] == '3')      /* make size of slow feed*/
            || (pSpoolData->ucPrtData[i+1] == 'J')      /* slow feed             */
            || (pSpoolData->ucPrtData[i+1] == '<')      /* return home           */
            || (pSpoolData->ucPrtData[i+1] == 'i')      /* full cut              */
            || (pSpoolData->ucPrtData[i+1] == 'm')      /* partial cut           */
            || (pSpoolData->ucPrtData[i+1] == 'o')      /* electro stamp         */
#if defined (DEVICE_7158) || defined (DEVICE_7194) || defined (DEVICE_7161) || defined (DEVICE_7196)
            || (pSpoolData->ucPrtData[i+1] == 'e')      /* reverse feed          */
#endif
            || (pSpoolData->ucPrtData[i+1] == 'q')
            ) {   /* open platen           */
                fFlag = 1;
                break;
            }
        }
    }

    if (usPrtType == PMG_PRT_SLIP) {
        if (fFlag == 0) {
            if (pSpoolData->aDataCtrl.ucDataID == SPBID_PRINT_DATA_E) {
                pPrtCtrl.pPrtTally->ulSlipLine++;
            }
            return;
        }
        pTally = pPrtCtrl.pPrtTally;


    } else if (usPrtType == PMG_PRT_JOURNAL) {
        if (fFlag == 0) {
            if ((pSpoolData->aDataCtrl.ucDataID == SPBID_PRINT_DATA_E)
            || (pSpoolData->aDataCtrl.ucDataID == SPBID_PRINT_DATA_RJ_E)) {
                pPrtCtrl.pPrtTally->ulJnlLine++;
            }
            return;
        }
        pTally = pPrtCtrl.pPrtTally;

    } else {
        if (fFlag == 0) {
            if ((pSpoolData->aDataCtrl.ucDataID == SPBID_PRINT_DATA_E)
            || (pSpoolData->aDataCtrl.ucDataID == SPBID_PRINT_DATA_RJ_E)) {
                pPrtCtrl.pPrtTally->ulRctLine++;
            }
            return;
        }
        pTally = pPrtCtrl.pPrtTally;
    }

    switch (pSpoolData->ucPrtData[i+1]) {
    case 'd':                               /* feed                     */
        pTally->usFeedLine += pSpoolData->ucPrtData[i+2];
        break;
    case 'i':                               /* full cut                 */
    case 'm':                               /* partial cut              */
        pTally->usCut++;
        break;
    case 'o':                               /* electro stamp            */
        pTally->usStamp++;
        break;
    default:
        break;
    }
}


/***    End of PMGSEND.C    ***/
