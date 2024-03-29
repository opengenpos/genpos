/*
**************************************************************************
**
**  Title:      Print Manager Interface Routine
**
**  Categoly:   Print Manager
**
**  Abstruct:   Interface Routine of Application and Print Manager
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
;   G0       | Mar-16-'95 | Correspond to PifBeginThread() argument by
;            |            | overlay function.
:            | May-25-'95 | Added PmgSetValWaitCount().
:   G1       | Aug-15-'95 | Changed from TYPE_M825_M820 to PMG_M825_M820
:            |            | in PmgOpenCom().
:   G3       | Sep-14-'95 | Added configuration check in PmgPrint().
:            |            | 
:   Saratoga | Feb-25-'00 | Added PmgReadStatus().
;            | Apr-11-'00 | Added reset command issue in PmgOpenCom() (<#001#>)
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
#include    <memory.h>
#include    <string.h>
#include    <ecr.h>
#include    <rfl.h>
#include    <pif.h>
//#include    <uie.h>
//#include    <paraequ.h>
#include    <termcfg.h>
#include    <tally.h>
#include    <pmg.h>
#include    "devpmg.h"
#include    "pmgcmn.h"
#include    "pmgif.h"
#include    "printio.h"
#include    "escp.h"

extern  UCHAR   ucCodeTable;                /* character code table         */
extern  UCHAR   ucFontTypeRJ;               /* font type                    */
extern  UCHAR   ucFeedLenRJ;                /* feed length R/J              */
extern  UCHAR   ucFeedLenS;                 /* feed length Slip             */

// OPOS receipt printer escape codes for standard
// printer output formatting actions.

typedef struct {
	const CHAR   *pEsc;
	const USHORT usLen;
} PrinterEscCode;

static const PrinterEscCode EscAlignLeft = { "\x1b|lA", 4 };      // left align following text in print line
static const PrinterEscCode EscAlignRight = { "\x1b|rA", 4 };     // right align following text in print line
static const PrinterEscCode EscAlignCenter = { "\x1b|cA", 4 };    // center following text in print line
static const PrinterEscCode EscSizeSnglWH = { "\x1b|1C", 4 };     // change text size to single wide, single high
static const PrinterEscCode EscSizeDblW = { "\x1b|2C", 4 };       // change text size to double wide, single high
static const PrinterEscCode EscSizeDblWH = { "\x1b|4C", 4 };      // change text size to double wide, double high
static const PrinterEscCode EscPaperCut = { "\x1b|75fP", 6 };     // perform paper cut after feeding a couple of lines
static const PrinterEscCode EscFeedlines = { "\x1b|1lF", 5 };     // perform line feed of paper. hard coded to 1 line.
static const PrinterEscCode EscInitPrinter = { "\x1b|3fT", 5 };   // perform printer initialization.

typedef struct {
    UCHAR* pucBuffDest;
    UCHAR* pucBuffSrc;
    UCHAR* pucBuffStart;
    USHORT usBuffLen;
    USHORT usDestLen;
} PmgEditBuff;

static USHORT  PmgEditOposEsc(PmgEditBuff *p);
static UCHAR  PmgStrAdjust(UCHAR *pszDest, USHORT usDestLen, UCHAR *pszSource, UCHAR uchMaxColumn, BOOL fAutoFeed);
static USHORT PmgWideAdjust(USHORT usPrtType, UCHAR *pucDest, UCHAR *pucSrc, SHORT sLen);

/**
;========================================================================
;+                                                                      +
;+            S T A T I C    R A M    A S S I G N M E N T               +
;+                                                                      +
;========================================================================
**/
PRTCTRL     pPrtCtrl;                       /* R&J printer control          */

#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
SPOOLCNT    pSpoolCtrlR;                    /* spool control block of R     */
UCHAR       pucSpoolR[SIZE_SPOOL_R+5];      /* spool buffer of receipt      */
SPOOLCNT    pSpoolCtrlJ;                    /* spool control block of J     */
UCHAR       pucSpoolJ[SIZE_SPOOL_J+5];      /* spool buffer of journal      */
#endif

#if defined (STATION_SLIP)
SPOOLCNT    pSpoolCtrlS;                    /* spool control block of S     */
UCHAR       pucSpoolS[SIZE_SPOOL_S+5];      /* spool bufer of slip          */
#endif

USHORT  usPmgStackSend[SIZE_STACK_SEND/2];    /* stack of snd thread      */
USHORT  usPmgStackReceive[SIZE_STACK_RCV/2];  /* stack of rcv thread      */

USHORT      usSaveValRetryCount;                /* Save count for PmgSetValWaitCount() */

PCALL_BACK      pPmgCallBack;
SERIAL_CONFIG   SerialConf;

PRTTALLY        PrtTallyRJ;
PRTTALLY        PrtTallyS;

/*
*********************************************************************
*
**Synopsis:     VOID    DevPmgInitialize(VOID)
*
**Return:       Non
*
**Description:  Print Manager Start up
*
*********************************************************************
*/

VOID DevPmgInitialize( VOID )
{
    /*static  VOID (THREADENTRY *pFuncS)(VOID) = PmgSend;*/
    /*static  VOID (THREADENTRY *pFuncR)(VOID) = PmgReceive;*/
    SYSCONFIG CONST FAR *pSysConfig;            /* system configuration */

    EscpInitCom();
    /**********************************/
    /*--- initialize print manager ---*/
    /**********************************/
    /*--- memory clear ---*/
    memset(&pPrtCtrl, 0, sizeof(pPrtCtrl));
#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
    memset(&pSpoolCtrlR, 0, sizeof(pSpoolCtrlR));
    /*--- clear spool buffer receipt ---*/
    memset(pucSpoolR, 0, SIZE_SPOOL_R);

    memset(&pSpoolCtrlJ, 0, sizeof(pSpoolCtrlJ));
    /*--- clear spool buffer journal ---*/
    memset(pucSpoolJ, 0, SIZE_SPOOL_J);
#endif

#if defined (STATION_SLIP)
    memset(&pSpoolCtrlS, 0, sizeof(pSpoolCtrlS));
    /*--- clear spool buffer slip ---*/
    memset(pucSpoolS, 0, SIZE_SPOOL_S);
#endif

    /*--- set up initial data ---*/
    ucCodeTable = 0;
    ucFontTypeRJ = 0;
    ucFeedLenRJ = PMG_SET_DEF_LF;
#if defined (DEVICE_7158)
    ucFeedLenS  = PMG_SET_DEF_LF_SLIP;
#else
    ucFeedLenS  = PMG_SET_DEF_LF;
#endif
    pPrtCtrl.hPort = pPrtCtrl.sPortNo = -1;

#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
    pSpoolCtrlR.pucStart
        = pSpoolCtrlR.pucSend
        = pSpoolCtrlR.pucReceive
        = pSpoolCtrlR.pucWrite
        = pSpoolCtrlR.pucValidation = pucSpoolR;
    pSpoolCtrlR.pucEnd = pucSpoolR + SIZE_SPOOL_R;
    pSpoolCtrlJ.pucStart
        = pSpoolCtrlJ.pucSend
        = pSpoolCtrlJ.pucReceive
        = pSpoolCtrlJ.pucWrite
        = pSpoolCtrlJ.pucValidation = pucSpoolJ;
    pSpoolCtrlJ.pucEnd = pucSpoolJ + SIZE_SPOOL_J;
#endif

#if defined (STATION_SLIP)
    pSpoolCtrlS.pucStart
        = pSpoolCtrlS.pucSend
        = pSpoolCtrlS.pucReceive
        = pSpoolCtrlS.pucWrite
        = pSpoolCtrlS.pucValidation = pucSpoolS;
    pSpoolCtrlS.pucEnd = pucSpoolS + SIZE_SPOOL_S;
#endif

    pSysConfig = PifSysConfig();

#if defined _WIN32_WCE || defined WIN32
#if defined (DEVICE_2208)
    pPrtCtrl.pPrtTally = &PrtTallyS;
#else
    pPrtCtrl.pPrtTally = &PrtTallyRJ;
#endif //DEVICE_2208
#else
#if defined (DEVICE_2208)
    pPrtCtrl.pPrtTally = (PRTTALLY *)pSysConfig->pTallySlip;
#else
    pPrtCtrl.pPrtTally = (PRTTALLY *)pSysConfig->pTallyRJ;
#endif //DEVICE_2208
#endif //_WIN32_WCE || defined WIN32

    pPrtCtrl.pPrtTally->usMark = 0xffff;
    pPrtCtrl.pPrtTally->usMaxCount = PMG_TALLIES;

    pPmgCallBack = NULL;

    /*--- get control resource ---*/
    PmgCreateSem();
    PmgCreateWait();

#if 0
    /* create thread -> move to PmgOpen*/
    /***********************/
    /*--- create thread ---*/
    /***********************/
    /*--- create R/J thread ---*/
    pPrtCtrl.bDoSend    = TRUE;
    pPrtCtrl.bDoReceive = TRUE;
    PifBeginThread( pFuncS,
                    &usPmgStackSend[sizeof(usPmgStackSend)/2],
                    sizeof(usPmgStackSend),
                    PRIO_PMG_OUT,
                    SEND_THREAD_NAME );
    PifBeginThread( pFuncR,
                    &usPmgStackReceive[sizeof(usPmgStackReceive)/2],
                    sizeof(usPmgStackReceive),
                    PRIO_PMG_IN,
                    RCV_THREAD_NAME );
#endif
    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    DevPmgInitialize(VOID)
*
**Return:       Non
*
**Description:  Print Manager Start up
*
*********************************************************************
*/

VOID DevPmgFinalize( VOID )
{
#if 0
    WORD    wTick, wTime, wTimeout;

    wTick    = 100;     /* 100msec */
    wTimeout = 10000;   /* 10sec */

    pPrtCtrl.bDoReceive = FALSE;
    pPrtCtrl.bDoSend    = FALSE;
    PmgWakeUp(ID_RECEIVE);
    PmgWakeUp(ID_SEND);

    for(wTime = 0; wTime < wTimeout; wTime += wTick) 
    {
        if ((pPrtCtrl.ucStatSend & SPLST_FINALIZED) &&
            (pPrtCtrl.ucStatReceive & SPLST_FINALIZED))
        {
            break;
        }
        PifSleep(wTick);
    }
#endif
    return;
}
/*
*********************************************************************
*
**Synopsis:     BOOL DevPmgOpen( VOID )
*
**Return:       Non
*
**Description:  Print Manager Open Communication
*
*********************************************************************
*/

BOOL DevPmgOpen( VOID )
{

    /****************************/
    /*--- open communication ---*/
    /****************************/

#if defined (DEVICE_2208)
    return(PmgOpenCom(PMG_PRT_SLIP));
#else
    return(PmgOpenCom(PMG_PRT_RCT_JNL));
#endif

}


/*
*******************************************************************************
*
**Synopsis:     VOID    DevPmgClose( VOID )
*
**Return:       Non
*
**Description:  close the communication
*
*******************************************************************************
*/

VOID DevPmgClose(VOID)
{
    SYSCONFIG CONST FAR *pSysConfig;            /* system configuration */
    WORD    wTick, wTime, wTimeout;
    SHORT sDummy = 0;

    /* thread close*/
    wTick    = 100;     /* 100msec */
    wTimeout = 10000;   /* 10sec */

	// close the send thread

    pPrtCtrl.bDoSend = FALSE;
    PmgWakeUp(ID_SEND);
    PmgWakeUp(ID_RECEIVE);
    for(wTime = 0; wTime < wTimeout; wTime += wTick) 
    {
        if (pPrtCtrl.ucStatSend & SPLST_FINALIZED)
        {
            break;
        }
        PifSleep(wTick);
    }

	// close the receive thread

    pPrtCtrl.bDoReceive = FALSE;
    PmgWakeUp(ID_RECEIVE);
    for(wTime = 0; wTime < wTimeout; wTime += wTick) 
    {
        if (pPrtCtrl.ucStatReceive & SPLST_FINALIZED)
        {
            break;
        }
        PifSleep(wTick);
    }

    /*--- close communication ---*/
    EscpCloseCom(sDummy);
    /*PifCloseCom(pPrtCtrl.hPort);*/

    pPrtCtrl.hPort = pPrtCtrl.sPortNo = -1;

    /**************************************/
    /* reset data for next printer open   */
    /**************************************/
    /* clear spool data*/
    memset(&pPrtCtrl, 0, sizeof(pPrtCtrl));
#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
    memset(&pSpoolCtrlR, 0, sizeof(pSpoolCtrlR));
    /*--- clear spool buffer receipt ---*/
    memset(pucSpoolR, 0, SIZE_SPOOL_R);

    memset(&pSpoolCtrlJ, 0, sizeof(pSpoolCtrlJ));
    /*--- clear spool buffer journal ---*/
    memset(pucSpoolJ, 0, SIZE_SPOOL_J);
#endif

#if defined (STATION_SLIP)
    memset(&pSpoolCtrlS, 0, sizeof(pSpoolCtrlS));
    /*--- clear spool buffer slip ---*/
    memset(pucSpoolS, 0, SIZE_SPOOL_S);
#endif

    /*--- set up initial data ---*/
    ucCodeTable = 0;
    ucFontTypeRJ = 0;
    ucFeedLenRJ = PMG_SET_DEF_LF;
#if defined (DEVICE_7158)
    ucFeedLenS  = PMG_SET_DEF_LF_SLIP;
#else
    ucFeedLenS  = PMG_SET_DEF_LF;
#endif
    pPrtCtrl.hPort = pPrtCtrl.sPortNo = -1;

#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
    pSpoolCtrlR.pucStart
        = pSpoolCtrlR.pucSend
        = pSpoolCtrlR.pucReceive
        = pSpoolCtrlR.pucWrite
        = pSpoolCtrlR.pucValidation = pucSpoolR;
    pSpoolCtrlR.pucEnd = pucSpoolR + SIZE_SPOOL_R;
    pSpoolCtrlJ.pucStart
        = pSpoolCtrlJ.pucSend
        = pSpoolCtrlJ.pucReceive
        = pSpoolCtrlJ.pucWrite
        = pSpoolCtrlJ.pucValidation = pucSpoolJ;
    pSpoolCtrlJ.pucEnd = pucSpoolJ + SIZE_SPOOL_J;
#endif

#if defined (STATION_SLIP)
    pSpoolCtrlS.pucStart
        = pSpoolCtrlS.pucSend
        = pSpoolCtrlS.pucReceive
        = pSpoolCtrlS.pucWrite
        = pSpoolCtrlS.pucValidation = pucSpoolS;
    pSpoolCtrlS.pucEnd = pucSpoolS + SIZE_SPOOL_S;
#endif

    pSysConfig = PifSysConfig();

#if defined _WIN32_WCE || defined WIN32
#if defined (DEVICE_2208)
    pPrtCtrl.pPrtTally = &PrtTallyS;
#else
    pPrtCtrl.pPrtTally = &PrtTallyRJ;
#endif //DEVICE_2208
#else
#if defined (DEVICE_2208)
    pPrtCtrl.pPrtTally = (PRTTALLY *)pSysConfig->pTallySlip;
#else
    pPrtCtrl.pPrtTally = (PRTTALLY *)pSysConfig->pTallyRJ;
#endif //DEVICE_2208
#endif //_WIN32_WCE || defined WIN32

    pPrtCtrl.pPrtTally->usMark = 0xffff;
    pPrtCtrl.pPrtTally->usMaxCount = PMG_TALLIES;

    pPmgCallBack = NULL;
    
    return;
}


/*
**************************************************************************
*
**Synopsis:     USHORT  DevPmgPrint(usPrtType, pucBuff, usLen)
*               input   USHORT  usPrtType;  : type of printer
*               input   UCHAR   *pucBuff;   : pointer to print buffer
*               input   USHORT  usLen;      : print data length
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert print data into spool buffer
*
**************************************************************************
*/

USHORT  DevPmgPrint(USHORT usPrtType, UCHAR *pucBuff, USHORT usLen)
{
    BOOL        fDataFlag;

    switch (PmgChkConfig(usPrtType)) {
    case PMG_TRUE:
        break;
    case PMG_IGNORE:
        return(PMG_SUCCESS);
    case PMG_FALSE:
    default:
        return(PMG_ERROR_PROVIDE);
    }

    PmgAsyncControl(usPrtType);

    /* --- not settle by this procedure --- */
    for (;;) {
		SPOOLCNT    *pSpoolCtrl = PmgGetSpoolCtrl(usPrtType);                /* spool control block      */

        if (pSpoolCtrl->ucMaxChar > 0 &&
            pSpoolCtrl->ucMaxChar < MAX_PRINT_CHAR) {
            break;
        }

        PmgErrorMsg(DEVICEIO_PRINTER_MSG_PRTTBL_ADR);
    }

    /* --- not settle by this procedure --- */
    PmgRequestSem(usPrtType);

	fDataFlag = FALSE;
	while (usLen > 0) {
		USHORT   sTmpLen, sTmpBufLen;
        UCHAR       pBuffDest[64] = { 0 };
        PmgEditBuff pBuff = { 0 };

       /*--- edit print buffer ---*/
        pBuff.pucBuffDest = pBuffDest;
        pBuff.pucBuffSrc = pBuff.pucBuffStart = pucBuff;
        pBuff.usBuffLen = usLen;
        if (PmgEditOposEsc(&pBuff) == FALSE) {
            sTmpLen = pBuff.usDestLen;
            sTmpBufLen = pBuff.pucBuffSrc - pucBuff;
			// this is an escape code sequence at the beginning of the buffer.
            /*--- check data flag ---*/
            if (fDataFlag == TRUE) {
                /*--- check 1 line feed code ---*/
                if (*pBuffDest == LF) {
                    fDataFlag = FALSE;

                /*--- check line feed code ---*/
                } else if (strncmp((CHAR *)pBuffDest, "\033d", 2) == 0) {
                    switch (pBuffDest[1]) {
                    case 0:                     /* 0 line feed  */
                        break;
                    case 1:                     /* 1 line feed  */
                        fDataFlag = FALSE;
                        break;
                    default:                    /* n line feed  */
                        pBuffDest[2]--;
                        fDataFlag = FALSE;

                        /*--- insert escape data ---*/
                        PmgInsSpoolData(usPrtType, pBuffDest, sTmpLen, FALSE);
                        break;
                    }
                } else {
                    /*--- insert escape data ---*/
                    PmgInsSpoolData(usPrtType, pBuffDest, sTmpLen, FALSE);
                }
            } else {
                /*--- insert escape data ---*/
                PmgInsSpoolData(usPrtType, pBuffDest, sTmpLen, FALSE);
            }
        }

        /*--- set next pointer ---*/
        pucBuff += sTmpBufLen;
        usLen   -= sTmpBufLen;
    }

    PmgReleaseSem(usPrtType);

    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     VOID    DevPmgWait( VOID )
*
**Return:       non
*
**Description:  Wait for empty the spool buffer
*
**************************************************************************
*/

VOID DevPmgWait( VOID )
{
#if (defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)) && defined (STATION_SLIP)
    PmgWaitRJS();
#else
#if defined (STATION_SLIP)
    PmgWaitS();
#else
    PmgWaitRJ();
#endif /* defined (STATION_SLIP) */
#endif
}

#if (defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)) && defined (STATION_SLIP)
VOID PmgWaitRJS( VOID )
{
    for (;;) {
        PmgRequestSemRJ();
        PmgRequestSemS();

        if ((pSpoolCtrlR.pucWrite == pSpoolCtrlR.pucReceive)
        &&(pSpoolCtrlJ.pucWrite == pSpoolCtrlJ.pucReceive)
        &&(pSpoolCtrlS.pucWrite == pSpoolCtrlS.pucReceive)) {

            pSpoolCtrlR.fbModeSpool &= (0xff - SPLMD_WAIT_EMPTY);
            pSpoolCtrlJ.fbModeSpool &= (0xff - SPLMD_WAIT_EMPTY);
            pSpoolCtrlS.fbModeSpool &= (0xff - SPLMD_WAIT_EMPTY);

            PmgReleaseSemRJ();
            PmgReleaseSemS();
            break;
        }

        pSpoolCtrlR.fbModeSpool |= SPLMD_WAIT_EMPTY;
        pSpoolCtrlJ.fbModeSpool |= SPLMD_WAIT_EMPTY;
        pSpoolCtrlS.fbModeSpool |= SPLMD_WAIT_EMPTY;

        PmgReleaseSemRJ();
        PmgReleaseSemS();

        PmgRequestWaitSem();
    }
}
#endif


#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
VOID PmgWaitRJ( VOID )
{
    for (;;) {
        PmgRequestSemRJ();

        if ((pSpoolCtrlR.pucWrite == pSpoolCtrlR.pucReceive)
        &&(pSpoolCtrlJ.pucWrite == pSpoolCtrlJ.pucReceive)) {

            pSpoolCtrlR.fbModeSpool &= (0xff - SPLMD_WAIT_EMPTY);
            pSpoolCtrlJ.fbModeSpool &= (0xff - SPLMD_WAIT_EMPTY);

            PmgReleaseSemRJ();
            break;
        }

        pSpoolCtrlR.fbModeSpool |= SPLMD_WAIT_EMPTY;
        pSpoolCtrlJ.fbModeSpool |= SPLMD_WAIT_EMPTY;

        PmgReleaseSemRJ();

        PmgRequestWaitSem();
    }
}
#endif /* defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION) */


#if defined (STATION_SLIP)
VOID PmgWaitS( VOID )
{
    for (;;) {
        PmgRequestSemS();

        if (pSpoolCtrlS.pucWrite == pSpoolCtrlS.pucReceive) {

            pSpoolCtrlS.fbModeSpool &= (0xff - SPLMD_WAIT_EMPTY);

            PmgReleaseSemS();
            break;
        }

        pSpoolCtrlS.fbModeSpool |= SPLMD_WAIT_EMPTY;

        PmgReleaseSemS();

        PmgRequestWaitSem();
    }
}
#endif /* defined (STATION_SLIP) */


/*
**************************************************************************
*
**Synopsis:     USHORT  DevPmgBeginSmallValidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Set Small Validation flag and send Station Selection and
*				Ignore paper sensor parameters.
*
*               Added to provide for short slip printing for US Customs.
*               Problem was trailing edge of short slip would pass trailing
*               edge paper sensor thereby causing printing to stop in the
*               middle of validation printing.  The changes to this file
*               provide for the use of a flag in the NCR 7158 printer DLL
*               to not send the printer station selection string.
*               When the NCR 7158 printer receives a station select string,
*               the printer automatically checks for paper under the paper
*				out sensors.  If no paper is under both sensors the printer stops.
*
**************************************************************************
*/

USHORT DevPmgBeginSmallValidation(USHORT usPrtType)
{
	//Select Slip Station ESC,c,0,0x04   Select Ignore paper sensors ESC,c,4,0x00
	UCHAR   EscBytesForSlipPrinter [] = { ESC, 'c', '0', 0x04, ESC, 'c', '4', 0x00 };

    switch (PmgChkConfig(usPrtType)) {
    case PMG_TRUE:
        break;
    case PMG_IGNORE:
        return(PMG_SUCCESS);
    case PMG_FALSE:
    default:
        return(PMG_ERROR_PROVIDE);
    }


    PmgRequestSem(usPrtType);
    if (usPrtType == PMG_PRT_SLIP) {
        /*--- enter small validation print ---*/
        PmgSetInputStat(PMG_PRT_SLIP, SPLMD_SMALL_VALIDATION);
    } else {
        /*--- enter small validation print ---*/
        PmgSetInputStat(PMG_PRT_RCT_JNL, SPLMD_SMALL_VALIDATION);
    }
    PmgReleaseSem(usPrtType);
//Must ReleaseSem before printing to avoid deadlock
    if (usPrtType == PMG_PRT_SLIP) {
		DevPmgPrint (usPrtType, EscBytesForSlipPrinter, sizeof(EscBytesForSlipPrinter));
	}

    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  DevPmgEndSmallValidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Reset Small Validation flag to none
*
*				WARNING:  Station Selection is not changed.(User Must Change)
*
**************************************************************************
*/

USHORT DevPmgEndSmallValidation(USHORT usPrtType)
{
    switch (PmgChkConfig(usPrtType)) {
    case PMG_TRUE:
        break;
    case PMG_IGNORE:
        return(PMG_SUCCESS);
    case PMG_FALSE:
    default:
        return(PMG_ERROR_PROVIDE);
    }

    PmgRequestSem(usPrtType);
    if (usPrtType == PMG_PRT_SLIP) {
        /*--- remove small validation print ---*/
        PmgResetInputStat(PMG_PRT_SLIP, SPLMD_SMALL_VALIDATION);
    } else {
        /*--- remove small validation print ---*/
        PmgResetInputStat(PMG_PRT_RCT_JNL, SPLMD_SMALL_VALIDATION);
    }
    PmgReleaseSem(usPrtType);

    return(PMG_SUCCESS);
}



/*
**************************************************************************
*
**Synopsis:     USHORT  DevPmgBeginValidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert begin of validation into spool buffer
*
**************************************************************************
*/

USHORT DevPmgBeginValidation(USHORT usPrtType)
{
    switch (PmgChkConfig(usPrtType)) {
    case PMG_TRUE:
        break;
    case PMG_IGNORE:
        return(PMG_SUCCESS);
    case PMG_FALSE:
    default:
        return(PMG_ERROR_PROVIDE);
    }

    PmgRequestSem(usPrtType);
    PmgInsSpoolID(usPrtType, SPBID_BEGIN_VALIDATION);
    if (usPrtType == PMG_PRT_SLIP) {
        /*--- enter validation print ---*/
        PmgSetInputStat(PMG_PRT_SLIP, SPLMD_VALIDATION);
    } else {
        /*--- enter validation print ---*/
        PmgSetInputStat(PMG_PRT_RCT_JNL, SPLMD_VALIDATION);
    }
    PmgReleaseSem(usPrtType);

    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  DevPmgEndVlidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert end of validation into spool buffer
*
**************************************************************************
*/

USHORT DevPmgEndValidation(USHORT usPrtType)
{
    switch (PmgChkConfig(usPrtType)) {
    case PMG_TRUE:
        break;
    case PMG_IGNORE:
        return(PMG_SUCCESS);
    case PMG_FALSE:
    default:
        return(PMG_ERROR_PROVIDE);
    }
    PmgAsyncControl(usPrtType);

    PmgRequestSem(usPrtType);
    PmgInsSpoolID(usPrtType, SPBID_END_VALIDATION);
    if (usPrtType == PMG_PRT_SLIP) {
        /*--- exit validation print ---*/
        PmgResetInputStat(PMG_PRT_SLIP, SPLMD_VALIDATION);
    } else {
        /*--- exit validation print ---*/
        PmgResetInputStat(PMG_PRT_RCT_JNL, SPLMD_VALIDATION);
    }
    PmgReleaseSem(usPrtType);

    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  DevPmgFeed(usPrtType, usLine)
*               input   USHORT  usPrtType;  : type of printer
*               input   USHORT  usLine;     : number of lines
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*               PMG_ERROR_COMM      : communication error
*               PMG_ERROR_PRNTER    : printer error
*
**Description:  Feed the desired pinter
*
**************************************************************************
*/

USHORT DevPmgFeed(USHORT usPrtType, USHORT usLine)
{
    SHORT   sRc;                /* return status                */
    UCHAR   fbStatus;           /* printer status               */


    switch (PmgChkConfig(usPrtType)) {
    case PMG_TRUE:
        break;
    case PMG_IGNORE:
        return(PMG_SUCCESS);
    case PMG_FALSE:
    default:
        return(PMG_ERROR_PROVIDE);
    }

    PmgRequestSem(usPrtType);

    sRc = PmgFeed2(usPrtType, usLine, &fbStatus);
    
    /*--- if error then retry 1 times ---*/
    if (sRc < 0) {
        if (sRc == PIF_ERROR_COM_POWER_FAILURE) {
            /*--- re-open communication ---*/
            PmgReOpenPrt(usPrtType);
        }
        /*--- send abort commoand ---*/
        if (PmgResetPrt(usPrtType) == FALSE) sRc = PMG_ERROR_COMM;
        else sRc = 1;
        /*--- retry 1 time ---*/
        if (sRc > 0)
            sRc = PmgFeed2(usPrtType, usLine, &fbStatus);

    } else if ( ((fbStatus & PRT_STAT_ERROR) == 0)
    || ((fbStatus & PRT_STAT_INIT_END) == 0) ) {
        /*--- send abort commoand ---*/
        if (PmgResetPrt(usPrtType) == FALSE) sRc = PMG_ERROR_COMM;
        /*--- retry 1 time ---*/
        if (sRc > 0)
            sRc = PmgFeed2(usPrtType, usLine, &fbStatus);
    }

    PmgReleaseSem(usPrtType);

    if (sRc <= 0) return(PMG_ERROR_COMM);
    if ((fbStatus & PRT_STAT_ERROR) == 0) return(PMG_ERROR_PRINTER);
    if ((fbStatus & PRT_STAT_INIT_END) == 0) return(PMG_ERROR_PRINTER);

    return(PMG_SUCCESS);
}

     
/*
**************************************************************************
*
**Synopsis:     USHORT  DevPmgBeginImportant(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert begin of important lines into spool buffer
*
**************************************************************************
*/

USHORT DevPmgBeginImportant(USHORT usPrtType)
{
    switch (PmgChkConfig(usPrtType)) {
    case PMG_TRUE:
        break;
    case PMG_IGNORE:
        return(PMG_SUCCESS);
    case PMG_FALSE:
    default:
        return(PMG_ERROR_PROVIDE);
    }
    PmgAsyncControl(usPrtType);

    PmgRequestSem(usPrtType);
    PmgInsSpoolID(usPrtType, SPBID_BEGIN_IMPORTANT);
    PmgSetInputStat(usPrtType, SPLMD_IMPORTANT);
    PmgReleaseSem(usPrtType);

    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  DevPmgEndImportant(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert end of important lines into spool buffer
*
**************************************************************************
*/

USHORT DevPmgEndImportant(USHORT usPrtType)
{
    USHORT  usRc;


    switch (PmgChkConfig(usPrtType)) {
    case PMG_TRUE:
        break;
    case PMG_IGNORE:
        return(PMG_SUCCESS);
    case PMG_FALSE:
    default:
        return(PMG_ERROR_PROVIDE);
    }
    PmgAsyncControl(usPrtType);

    PmgRequestSem(usPrtType);
    PmgInsSpoolID(usPrtType, SPBID_END_IMPORTANT);
    PmgReleaseSem(usPrtType);

    switch (usPrtType) {
        case PMG_PRT_RECEIPT:
        case PMG_PRT_RCT_JNL:
            PmgRequestImpSemR();
            break;
        case PMG_PRT_JOURNAL:
            PmgRequestImpSemJ();
            break;
        case PMG_PRT_SLIP:
            PmgRequestImpSemS();
            break;
    }

    PmgRequestSem(usPrtType);
    usRc = PmgGetInputStat(usPrtType);
    PmgResetInputStat(usPrtType, SPLMD_IMPORTANT|SPLMD_IMP_ERROR);
    PmgReleaseSem(usPrtType);

    if ((usRc & SPLMD_IMP_ERROR) == 0) return(PMG_SUCCESS);
    return(PMG_ERROR_IMPORTANT);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  DevPmgGetStatus(usPrtType, pfbStatus)
*               input   USHORT  usPrtType;  : type of printer
*               inout   PUSHORT pfbStatus;  : printer status
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*               PMG_ERROR_COMM      : communication error
*               PMG_ERROR_PRNTER    : printer error
*
**Description:  Get printer status. This function execute immediately.
*
**************************************************************************
*/
USHORT DevPmgGetStatus(USHORT usPrtType, UCHAR *pfbStatus)
{
    SHORT       sRc;
    SPOOLCNT    *pSpoolCtrl = PmgGetSpoolCtrl(usPrtType);
    *pfbStatus = 0;
    if (pPrtCtrl.sPortNo < 0) return(PMG_ERROR_PROVIDE);

    if(usPrtType ==  PMG_PRT_JOURNAL){
#if !defined(STATION_JOURNAL)
        *pfbStatus = 0;
        return(PMG_SUCCESS);
#endif
    }
    if(usPrtType ==  PMG_PRT_SLIP){
#if !defined(STATION_SLIP)
        *pfbStatus = 0;
        return(PMG_SUCCESS);
#endif
    }
    if(usPrtType ==  PMG_PRT_VALIDATION){
#if !defined(STATION_VALIDATION)
        *pfbStatus = 0;
        return(PMG_SUCCESS);
#endif
    }
    switch (usPrtType) {
    case PMG_PRT_JOURNAL:
    case PMG_PRT_SLIP:
    case PMG_PRT_RECEIPT:
    case PMG_PRT_RCT_JNL:
    case PMG_PRT_VALIDATION:
        break;
    default:
        return(PMG_ERROR_PROVIDE);
    }
    DevPmgWait();
    if (pPrtCtrl.hPort < 0) {
        if (PmgOpenCom(usPrtType) == FALSE) return(PMG_ERROR_COMM);
    }

    PmgRequestSem(usPrtType);

    sRc = PmgReadStatus(usPrtType, pfbStatus);


    if ((sRc == PIF_ERROR_COM_POWER_FAILURE)
    || ((sRc > 0)&&((*pfbStatus & PRT_STAT_INIT_END) == 0))) {
        if (pSpoolCtrl->pucWrite == pSpoolCtrl->pucReceive) {
            /*PmgRequestSem(usPrtType);*/

            if (sRc == PIF_ERROR_COM_POWER_FAILURE) {
                /*--- re-open communication ---*/
                PmgReOpenPrt(usPrtType);
            }
            /*--- send abort commoand ---*/
            PmgResetPrt(usPrtType);
            /*PmgReleaseSem(usPrtType);*/
        }

        if (pPrtCtrl.hPort < 0) {
            if (PmgOpenCom(usPrtType) == FALSE){
                PmgReleaseSem(usPrtType);
                return(PMG_ERROR_COMM);
            }
        }
        sRc = PmgReadStatus(usPrtType, pfbStatus);
    }
    PmgReleaseSem(usPrtType);

    if (sRc == PIF_OK) return(PMG_SUCCESS);
    *pfbStatus &=~PRT_STAT_ERROR;
    return(PMG_ERROR_COMM);
}



/*
**************************************************************************
*
**Synopsis:     USHORT  DevPmgPrtConfig(usPrtType, pusColumn, pfbStatus)
*               input   USHORT  usPrtType;  : type of printer
*               inout   USHORT  *pusColumn; : number of clumn
*               inout   USHORT  *pfbStatus; : printer status
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*               PMG_ERROR_COMM      : communication error
*               PMG_ERROR_PRNTER    : printer error
*
**Description:  Get printer status & number of column.
*
**************************************************************************
*/

USHORT DevPmgPrtConfig(USHORT usPrtType, USHORT *pusColumn, UCHAR *pfbStatus)
{
    if (pPrtCtrl.sPortNo < 0) return(PMG_ERROR_PROVIDE);
    if (pPrtCtrl.hPort < 0) {
        if (PmgOpenCom(usPrtType) == FALSE) return(PMG_ERROR_COMM);
    }

    switch (usPrtType) {
#if defined (STATION_SLIP)
    case PMG_PRT_SLIP:
        *pusColumn = pSpoolCtrlS.ucMaxChar;
        *pfbStatus = pSpoolCtrlS.fbConfig;
        break;
#endif

#if defined (STATION_RECEIPT) || defined (STATION_JOURNAL) || defined (STATION_VALIDATION)
    case PMG_PRT_RECEIPT:
    case PMG_PRT_RCT_JNL:
        *pusColumn = pSpoolCtrlR.ucMaxChar;
        *pfbStatus = pSpoolCtrlR.fbConfig;
        break;

    case PMG_PRT_JOURNAL:
        *pusColumn = pSpoolCtrlJ.ucMaxChar;
        *pfbStatus = pSpoolCtrlJ.fbConfig;
        break;
#endif

    default:
        return(PMG_ERROR_PROVIDE);
    }

    return(PMG_SUCCESS);
}


/*
**************************************************************************
*
**Synopsis:     USHORT  DevPmgFont(usPrtType, usFont)
*               input   USHORT  usPrtType;  : type of printer
*               inout   USHORT  usFont;     : type of font
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*               PMG_ERROR_COMM      : communication error
*
**Description:  Set to font type
*
**************************************************************************
*/

USHORT  DevPmgFont( USHORT usPrtType, USHORT usFont)
{
    switch (PmgChkConfig(usPrtType)) {
    case PMG_TRUE:
        break;
    case PMG_IGNORE:
        return(PMG_SUCCESS);
    case PMG_FALSE:
    default:
        return(PMG_ERROR_PROVIDE);
    }

    PmgRequestSem(usPrtType);
    if (usFont != PMG_SET_FONT_B) {
        PmgInsSpoolID(usPrtType, SPBID_FONT_A);
        PmgResetInputStat(usPrtType, SPLMD_FONT_B);
#if defined (STATION_RECEIPT)
        pSpoolCtrlR.ucMaxChar = COLMUN_RECEIPT_A;
#endif
    } else {
        PmgInsSpoolID(usPrtType, SPBID_FONT_B);
        PmgSetInputStat(usPrtType, SPLMD_FONT_B);
#if defined (STATION_RECEIPT)
        pSpoolCtrlR.ucMaxChar = COLMUN_RECEIPT_B;
#endif
    }
    PmgReleaseSem(usPrtType);

    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgEditEsc(pucBuff, usLen, pusOutLen)
*               input   UCHAR   *pucBuff;   : point to print buffer
*               input   USHORT  usLen;      : length of data
*               output  USHORT  *pusOutLen  : output data length
*
**Return:       TRUE    : text data
*               FALSE   : esc control data
*
**Description:  edit esc data
*
**************************************************************************
*/
static bool IsEscCodeType(UCHAR *pucBuff, PrinterEscCode escCode)
{
	return strncmp((char *)pucBuff, escCode.pEsc, escCode.usLen) == 0;
}

static USHORT PmgEditOposEsc(PmgEditBuff *pBuff)
{
    UCHAR* pX, *pY;

    for (pX = pBuff->pucBuffSrc, pY = pBuff->pucBuffDest; pX < pBuff->pucBuffStart + pBuff->usBuffLen; ) {

        /* --- Fix a glitch (03/31/2001)
        To eject slip paper properly, use "Eject Slip Paper"
        command instead of "Reverse Feed".
        --- */
        if ((*pX == LF) || (*pX == 0x0C)) {
            *pY = *pX;
            pX += 1;
            pY += 1;
            break;
        }

        // the printer appears to treat a carriage return
        // as a line feed resulting in a carriage return, line feed
        // sequence printing as a line of text followed by a blank line.
        // this may be a printer setting however testing with a
        // NCR 7197 thermal printer shows this removal of a carriage
        // return is needed.
        if (*pX == CR) {
            pX += 1;
            break;
        }
        else if (*pX != ESC) {
            *pY++ = *pX++;
        } else {
            if (IsEscCodeType(pX, EscPaperCut)) {
                // replace the OPOS paper cut
                // with the NCR 7197 paper cut command.
                // we need to feed a couple of lines before doing the
                // papercut in order to move the last printed line
                // past the knife.
                pX += EscPaperCut.usLen;
                *pY++ = DC4; *pY++ = 6;
                *pY++ = ESC; *pY++ = 'm';
                break;
            }
            else if (IsEscCodeType(pX, EscFeedlines)) {
                // replace the OPOS line feed
                // with the NCR 7197 line feed command.
                pX += EscFeedlines.usLen;
                *pY++ = DC4; *pY++ = 1;
                break;
            }
            else if (IsEscCodeType(pX, EscSizeSnglWH)) {
                // keep the escape character and then replace the OPOS single wide text
                // with the NCR 7197 single wide text command.
                pX += EscSizeSnglWH.usLen;
                *pY++ = ESC; *pY++ = '!'; *pY++ = 0x00;    // first char was GS
                break;
            }
            else if (IsEscCodeType(pX, EscSizeDblW)) {
                // keep the escape character and then replace the OPOS double wide text
                // with the NCR 7197 double wide text command.
                pX += EscSizeDblW.usLen;
                *pY++ = ESC; *pY++ = '!'; *pY++ = 0x20;    // first char was GS
                break;
            }
            else if (IsEscCodeType(pX, EscSizeDblWH)) {
                // keep the escape character and then replace the OPOS double wide double high text
                // with the NCR 7197 double wide double high text command.
                pX += EscSizeDblWH.usLen;
                *pY++ = ESC; *pY++ = '!'; *pY++ = 0x30;    // first char was GS
                break;
            }
            else if (IsEscCodeType(pX, EscAlignLeft)) {
                //#define DO_ALIGN

                                // keep the escape character and then replace the OPOS left align
                                // with the NCR 7197 left align command.
#if defined(DO_ALIGN)
                pX += EscAlignLeft.usLen;
                *pY++ = ESC; *pY++ = 'a'; *pY++ = '0';
#else
                for (USHORT usI = 0; usI < EscAlignLeft.usLen; usI++) {
                    *pY++ = *pX++;
                }
#endif
                break;
            }
            else if (IsEscCodeType(pX, EscAlignRight)) {
                // replace the OPOS right align
                // with the NCR 7197 right align command.
#if defined(DO_ALIGN)
                pX += EscAlignRight.usLen;
                *pY++ = ESC; *pY++ = 'a'; *pY++ = '2';
#else
                for (USHORT usI = 0; usI < EscAlignRight.usLen; usI++) {
                    *pY++ = *pX++;
                }
#endif
                break;
            }
            else if (IsEscCodeType(pX, EscAlignCenter)) {
                // replace the OPOS center align
                // with the NCR 7197 center align command.
#if defined(DO_ALIGN)
                pX += EscAlignCenter.usLen;
                //				pucBuff[i] = ESC; pucBuff[i + 1] = DC4; pucBuff[i + 2] = '1';  // set column command
                *pY++ = ESC; *pY++ = 'a'; *pY++ = '1';
#else
                for (USHORT usI = 0; usI < EscAlignCenter.usLen; usI++) {
                    *pY++ = *pX++;
                }
#endif
                break;
            }
            else if (IsEscCodeType(pX, EscInitPrinter)) {
                // replace the OPOS initialize printer
                // with the NCR 7197 initialize printer command.
                pX += EscInitPrinter.usLen;
                //				pucBuff[i] = ESC; pucBuff[i + 1] = 0x40;
                *pY++ = DLE;
                *pY++ = ESC; *pY++ = 0x16; *pY++ = 0x01;
                break;
            }
        }
    }

    pBuff->pucBuffSrc = pX;
    pBuff->usDestLen = pY - pBuff->pucBuffDest;
    pBuff->pucBuffDest = pY;

    return 0;
}


/*
**************************************************************************
*
**Synopsis:     VOID    PmgInsSpoolID(usPrtType, ucDataID)
*               input   USHORT  usPrtType;  : type of printer
*               input   UCHAR   ucDataID;   : spool data ID
*
**Return:       non
*
**Description:  Insert the print data ID into spool buffer
*
**************************************************************************
*/

#define LOOP    0x10
#define NEXT    0x20

VOID PmgInsSpoolID(USHORT usPrtType, UCHAR ucDataID)
{
    PRTCTRL     *pPrintCtrl;
    SPOOLCNT    *pSpoolCnt;
    DATACTRL    *pSpoolBuff;
    UCHAR       *pSpoolNext;
    USHORT      usStat = FALSE;


    PmgGetCtrl(usPrtType, &pPrintCtrl, &pSpoolCnt);

    while (usStat == FALSE) {
        pSpoolNext = pSpoolCnt->pucWrite + sizeof(DATACTRL)*2;
        if (pSpoolNext > pSpoolCnt->pucEnd) {
            /*--- if bottom of spool buffer ---*/
            if ((pSpoolCnt->pucValidation > pSpoolCnt->pucWrite)
            ||(pSpoolCnt->pucStart+sizeof(DATACTRL)*2 >= pSpoolCnt->pucValidation)) {
                /*--- if no room in spool buffer ---*/
                PmgWakeUp(ID_SEND);
                PmgReleaseSem(usPrtType);
                if (usPrtType == PMG_PRT_SLIP)
                    PmgSleep(ID_WAIT_S, WAIT_BUFF, 0);
                else
                    PmgSleep(ID_WAIT_RJ, WAIT_BUFF, 0);
                PmgRequestSem(usPrtType);
            } else {
                /*--- permit to insert the data ---*/
                usStat = LOOP;
                pSpoolBuff = (DATACTRL *)pSpoolCnt->pucWrite;
            }
        } else {
            /*--- normal insertion ---*/
            if ((pSpoolNext >= pSpoolCnt->pucValidation)
            &&(pSpoolCnt->pucValidation > pSpoolCnt->pucWrite)) {
                /*--- if no room in spool buffer ---*/
                PmgWakeUp(ID_SEND);
                /*--- wait ---*/
                PmgReleaseSem(usPrtType);
                if (usPrtType == PMG_PRT_SLIP)
                    PmgSleep(ID_WAIT_S, WAIT_BUFF, 0);
                else
                    PmgSleep(ID_WAIT_RJ, WAIT_BUFF, 0);
                PmgRequestSem(usPrtType);
            } else {
                /*--- permit to insert the data ---*/
                usStat = NEXT;
                pSpoolBuff = (DATACTRL *)pSpoolCnt->pucWrite;
            }
        }
    }

    if (usStat == LOOP) {
        /*--- insert end of spool block ---*/
        pSpoolBuff->ucVLI = sizeof(DATACTRL);
        pSpoolBuff->ucDataID = SPBID_END_SPOOL;
        pSpoolBuff->ucInputSeqNo = pPrintCtrl->ucInputSeqNo;
        pSpoolBuff->fbPrtMode = 0;
        pSpoolBuff->ucCommSeqNo = 0;

        pSpoolBuff = (DATACTRL *)pSpoolCnt->pucStart;
    }
    /*--- insert special data block ---*/
    pSpoolBuff->ucVLI = sizeof(DATACTRL);
    pSpoolBuff->ucDataID = ucDataID;
    pSpoolBuff->ucInputSeqNo = pPrintCtrl->ucInputSeqNo;
    pSpoolBuff->fbPrtMode = 0;
    pSpoolBuff->ucCommSeqNo = 0;

    /*--- insert end of data block ---*/
    pSpoolBuff++;
    pSpoolBuff->ucVLI = sizeof(DATACTRL);
    pSpoolBuff->ucDataID = SPBID_END_DATA;
    pSpoolBuff->ucInputSeqNo = pPrintCtrl->ucInputSeqNo;
    pSpoolBuff->fbPrtMode = 0;
    pSpoolBuff->ucCommSeqNo = 0;

    pSpoolCnt->pucWrite = (UCHAR *)pSpoolBuff;
    pPrintCtrl->ucInputSeqNo++;

    PmgWakeUp(ID_SEND);

}


/*
**************************************************************************
*
**Synopsis:     VOID    PmgInsSpoolData(usPrtType, pucData, usLen, fFlag)
*               input   USHORT  usPrtType;  : type of printer
*               input   UCHAR   pucData;    : spool data
*               input   USHORT  usLen;      : data length
*               input   BOOL    fFlag;      : continue flag
*
**Return:       non
*
**Description:  Insert the print data into spool buffer
*
**************************************************************************
*/

VOID PmgInsSpoolData(USHORT usPrtType, UCHAR *pucData, USHORT usLen, BOOL fFlag)
{
    PRTCTRL     *pPrintCtrl;
    SPOOLCNT    *pSpoolCnt;
    SPOOLDAT    *pSpoolBuff;
    USHORT      usStat = FALSE;


    PmgGetCtrl(usPrtType, &pPrintCtrl, &pSpoolCnt);

    while (usStat == FALSE) {
		UCHAR  *pSpoolNext = pSpoolCnt->pucWrite + sizeof(DATACTRL)*2 + usLen;    /* point to next spool buffer   */

        if (pSpoolNext > pSpoolCnt->pucEnd) {
            if ((pSpoolCnt->pucValidation > pSpoolCnt->pucWrite)
            ||(pSpoolCnt->pucStart+sizeof(DATACTRL)*2+usLen >= pSpoolCnt->pucValidation)) {
                /*--- if no room in spool buffer ---*/
                PmgWakeUp(ID_SEND);
                /*--- wait ---*/
                PmgReleaseSem(usPrtType);
                if (usPrtType == PMG_PRT_SLIP)
                    PmgSleep(ID_WAIT_S, WAIT_BUFF, 0);
                else
                    PmgSleep(ID_WAIT_RJ, WAIT_BUFF, 0);
                PmgRequestSem(usPrtType);
            } else {
                usStat = LOOP;
                pSpoolBuff = (SPOOLDAT *)pSpoolCnt->pucWrite;
            }
        } else {
            if ((pSpoolNext >= pSpoolCnt->pucValidation)
            &&(pSpoolCnt->pucValidation > pSpoolCnt->pucWrite)) {
                /*--- if no room in spool buffer ---*/
                PmgWakeUp(ID_SEND);
                /*--- wait ---*/
                PmgReleaseSem(usPrtType);
                if (usPrtType == PMG_PRT_SLIP)
                    PmgSleep(ID_WAIT_S, WAIT_BUFF, 0);
                else
                    PmgSleep(ID_WAIT_RJ, WAIT_BUFF, 0);
                PmgRequestSem(usPrtType);
            } else {
                usStat = NEXT;
                pSpoolBuff = (SPOOLDAT *)pSpoolCnt->pucWrite;
            }
        }
    }

    if (usStat == LOOP) {
        /*--- insert end of spool block ---*/
        pSpoolBuff->aDataCtrl.ucVLI = sizeof(DATACTRL);
        pSpoolBuff->aDataCtrl.ucDataID = SPBID_END_SPOOL;
        pSpoolBuff->aDataCtrl.ucInputSeqNo = pPrintCtrl->ucInputSeqNo;
        pSpoolBuff->aDataCtrl.fbPrtMode = 0;
        pSpoolBuff = (SPOOLDAT *)pSpoolCnt->pucStart;
    }
    /*--- insert data block ---*/
    pSpoolBuff->aDataCtrl.ucVLI = (UCHAR)(usLen + sizeof(DATACTRL));
    pSpoolBuff->aDataCtrl.ucInputSeqNo = pPrintCtrl->ucInputSeqNo;
    pSpoolBuff->aDataCtrl.fbPrtMode = 0;
    pSpoolBuff->aDataCtrl.ucCommSeqNo = 0;
    if (usPrtType == PMG_PRT_RCT_JNL) {
        if (fFlag == TRUE){
            /*--- continue to the next ---*/
            pSpoolBuff->aDataCtrl.ucDataID = SPBID_PRINT_DATA_RJ_C;
        } else {
            /*--- end of data --*/
            pSpoolBuff->aDataCtrl.ucDataID = SPBID_PRINT_DATA_RJ_E;
        }
    } else {
        if (fFlag == TRUE){
            /*--- continue to the next ---*/
            pSpoolBuff->aDataCtrl.ucDataID = SPBID_PRINT_DATA_C;
        } else {
            /*--- end of data --*/
            pSpoolBuff->aDataCtrl.ucDataID = SPBID_PRINT_DATA_E;
        }
    }

    memcpy(&pSpoolBuff->ucPrtData, pucData, usLen);

    /*--- insert end of data block ---*/
    pSpoolBuff = (SPOOLDAT *)((UCHAR *)pSpoolBuff + usLen + sizeof(DATACTRL));
    pSpoolBuff->aDataCtrl.ucVLI = sizeof(DATACTRL);
    pSpoolBuff->aDataCtrl.ucDataID = SPBID_END_DATA;
    pSpoolBuff->aDataCtrl.ucInputSeqNo = pPrintCtrl->ucInputSeqNo;
    pSpoolBuff->aDataCtrl.fbPrtMode = 0;
    pSpoolBuff->aDataCtrl.ucCommSeqNo = 0;

    pSpoolCnt->pucWrite = (UCHAR *)pSpoolBuff;
    pPrintCtrl->ucInputSeqNo++;

    PmgWakeUp(ID_SEND);
}


/*
**************************************************************************
*
**Synopsis:     SHORT   PmgWideAdjust(usPrtType, pucDest, pucSrc, sLen, psRetLen1)
*               input   USHORT  usPrtType;  : type of printer
*               input   UCHAR   *pucDest;   : destination address
*               input   UCHAR   *pucSrc;    : source address
*               input   SHORT   sLen;       : source data length
*
**Return:       destination data length
*
**Description:  Adjust width
*
**************************************************************************
*/

static USHORT PmgWideAdjust(USHORT usPrtType, UCHAR *pucDest, UCHAR *pucSrc, SHORT sLen)
{
    SHORT   sRetLen;
    USHORT  fWidth;
    UCHAR   ucFontType;
    UCHAR   ucDW, ucDHDW, ucNHNW;


    sRetLen = 0;
    fWidth = TRUE;

    if (usPrtType == PMG_PRT_SLIP) {
        ucDW = PMG_ESC_CHARX2;
        ucDHDW = PMG_ESC_CHARX4;
        ucNHNW = 0;
    } else {
        if ((PmgGetInputStat(usPrtType) & SPLMD_FONT_B) != 0)
            ucFontType = PMG_SET_FONT_B;
        else
            ucFontType = 0;

        ucDW = ucFontType | (UCHAR)PMG_ESC_CHARX2;
        ucDHDW = ucFontType | (UCHAR)PMG_ESC_CHARX4;
        ucNHNW = ucFontType;
    }

    while (sLen > 0) {
        while (sLen > 0) {
            if (*pucSrc == PMG_CHARX2) {
                fWidth  = FALSE;
                pucSrc++;                           sLen--;
                *pucDest++ = ESC;       sRetLen++;
                *pucDest++ = '!';       sRetLen++;
                *pucDest++ = ucDW;      sRetLen++;
                *pucDest++ = *pucSrc++; sRetLen++;  sLen--;
                break;
            } else if (*pucSrc == PMG_CHARX4) {
                fWidth  = FALSE;
                pucSrc++;                           sLen--;
                *pucDest++ = ESC;       sRetLen++;
                *pucDest++ = '!';       sRetLen++;
                *pucDest++ = ucDHDW;    sRetLen++;
                *pucDest++ = *pucSrc++; sRetLen++;  sLen--;
                break;
            }
            *pucDest++ = *pucSrc++;     sRetLen++;  sLen--;
        }
        while (sLen > 0) {
            if ((*pucSrc != PMG_CHARX2)&&(*pucSrc != PMG_CHARX4)) {
                fWidth  = TRUE;
                *pucDest++ = ESC;       sRetLen++;
                *pucDest++ = '!';       sRetLen++;
                *pucDest++ = ucNHNW;    sRetLen++;
                *pucDest++ = *pucSrc++; sRetLen++;  sLen--;
                break;
            }
            pucSrc++;                               sLen--;
            *pucDest++ = *pucSrc++;     sRetLen++;  sLen--;
        }
    }
    if (fWidth == FALSE) {
        *pucDest++ = ESC;               sRetLen++;
        *pucDest++ = '!';               sRetLen++;
        *pucDest++ = ucNHNW;            sRetLen++;
    }

    *pucDest++ = LF;                    sRetLen++;

    return(sRetLen);
}


/*
*********************************************************************
*
**Synopsis:     USHORT  PmgChkConfig(usPrtType)
*               input   USHORT  usPrtType;  : printer type
*
**Return:       PMG_TRUE    : provide
*               PMG_FALSE   : not provide
*               PMG_IGNORE  : ignore
*
**Description:  Check printer configuration
*
*********************************************************************
*/

USHORT PmgChkConfig(USHORT usPrtType)
{
    if (pPrtCtrl.sPortNo < 0) return(PMG_FALSE);

    if(usPrtType == PMG_PRT_JOURNAL){
#if !defined(STATION_JOURNAL)
        return(PMG_IGNORE);
#endif
    }
    if(usPrtType == PMG_PRT_SLIP){
#if !defined(STATION_SLIP)
        return(PMG_IGNORE);
#endif
    }

    switch (usPrtType) {
    case PMG_PRT_JOURNAL:
    case PMG_PRT_SLIP:
    case PMG_PRT_RECEIPT:
    case PMG_PRT_RCT_JNL:
        if (pPrtCtrl.hPort >= 0) return(PMG_TRUE);
        while (PmgOpenCom(usPrtType) == FALSE) {
            /*--- error display & input key ---*/
            PmgErrorMsg(DEVICEIO_PRINTER_MSG_PRTTBL_ADR);
        }
        return(PMG_TRUE);
    default:
        return(PMG_FALSE);
    }
}


/*
*********************************************************************
*
**Synopsis:     USHORT  PmgAsyncControl(usPrtType)
*               input   USHORT  usPrtType;  : printer type
*
**Return:       PMG_TRUE    : provide
*               PMG_FALSE   : not provide
*
**Description:  Async Control at R/J with Slip
*
*********************************************************************
*/

USHORT PmgAsyncControl(USHORT usPrtType)
{
    switch (usPrtType) {
    case PMG_PRT_SLIP:
        PmgWaitRJ();
        return(PMG_TRUE);
    case PMG_PRT_JOURNAL:
    case PMG_PRT_RECEIPT:
    case PMG_PRT_RCT_JNL:
#if defined (STATION_SLIP)
        PmgWaitS();
#endif
        return(PMG_TRUE);
    default:
        return(PMG_FALSE);
    }
}


/*
*********************************************************************
*
**Synopsis:     SHORT   PmgOpenCom(usPrtType)
*               input   USHORT  usPrtType;  : printer type
*
**Return:       TRUE    : provide
*               FALSE   : not provide
*
**Description:  Open printer port
*
*********************************************************************
*/

SHORT PmgOpenCom(USHORT usPrtType)
{
	PROTOCOL    pProtocol = { 0 };                  /* communication protocol   */

    if (!PmgGetSerialConfig(&SerialConf))
    {
        return FALSE;
    }
    pPrtCtrl.sPortNo = (SHORT)SerialConf.uchComPort;
    pPrtCtrl.ucSysPara = DEVICE_PRINTER;

    /****************************/
    /*--- open communication ---*/
    /****************************/
    pProtocol.fPip             = SerialConf.usPip;
    pProtocol.uchComByteFormat = SerialConf.uchComByteFormat;
    pProtocol.ulComBaud = SerialConf.ulComBaud;
        
    pPrtCtrl.hPort = EscpOpenCom(pPrtCtrl.sPortNo , &pProtocol);

	if (pPrtCtrl.hPort < 0)
	{
		return FALSE;
	}

    /****************************************/
    /*--- set up Column                  ---*/
    /****************************************/
#if defined (DEVICE_2208)
    pSpoolCtrlS.ucMaxChar   = COLMUN_SLIP;
    pSpoolCtrlS.ucValidChar = COLMUN_SLIP;

#endif
#if defined (DEVICE_7140)
    pSpoolCtrlR.ucMaxChar   = COLMUN_RECEIPT_A;
    pSpoolCtrlJ.ucMaxChar   = COLMUN_JOURNAL;
    pSpoolCtrlR.ucValidChar = pSpoolCtrlJ.ucValidChar
                            = COLMUN_VALIDATION;
    pSpoolCtrlR.fbConfig    = pSpoolCtrlJ.fbConfig
                            = PMG_VALIDATION;

#endif
#if defined (DEVICE_7158)
    pSpoolCtrlR.ucMaxChar = COLMUN_RECEIPT_A;
    pSpoolCtrlJ.ucMaxChar = pSpoolCtrlR.ucValidChar
                          = pSpoolCtrlJ.ucValidChar
                          = 0;                        /*** 0 column   ***/
    pSpoolCtrlS.ucMaxChar   = COLMUN_SLIP;
    pSpoolCtrlS.ucValidChar = COLMUN_SLIP;

#endif
#if defined (DEVICE_7194) || defined (DEVICE_7161) || defined (DEVICE_7196)
    pSpoolCtrlR.ucMaxChar = COLMUN_RECEIPT_A;
    pSpoolCtrlJ.ucMaxChar = pSpoolCtrlR.ucValidChar
                          = pSpoolCtrlJ.ucValidChar
                          = 0;                        /*** 0 column   ***/
#endif

    /***********************/
    /*--- create thread ---*/
    /***********************/
    /*--- create R/J thread ---*/
    pPrtCtrl.bDoSend    = TRUE;
    pPrtCtrl.bDoReceive = TRUE;
    PifBeginThread(PmgSend, 0, 0, PRIO_PMG_OUT, SEND_THREAD_NAME );
    PifBeginThread(PmgReceive, 0, 0, PRIO_PMG_IN, RCV_THREAD_NAME );

    /* issue printer reset command */
    if (PmgResetPrt(PMG_PRT_RCT_JNL) == FALSE)                              /* A START <#001#> */
    {
        EscpCloseCom(pPrtCtrl.hPort);
        pPrtCtrl.hPort = -1;
    }                                                                       /* A END <#001#> */

    /*--- set return value ---*/
    if (pPrtCtrl.hPort >= 0)    return(TRUE);
    else                        return(FALSE);
    usPrtType = usPrtType;

}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgFeed2(usPrtType, usLine, pfbStatus)
*               input   USHORT  usPrtType;  : type of printer
*               input   USHORT  usLine;     : number of lines
*               output  UCHAR   *pfbStatus  : responce status of printer
*
**Return:       Pif return status
*
**Description:  Feed the desired pinter
*
**************************************************************************
*/

USHORT PmgFeed2(USHORT usPrtType, USHORT usLine, UCHAR *pfbStatus)
{
    PRT_SNDMSG      aFeedData;
    USHORT          usOffset;
    SHORT           sRc;
    PRT_RESPONCE    aPrtResponce;
    
    ESC4BYTE    EscStation   = {ESC, 'c', '0', 1};    /* select stesion */
    ESC3BYTE    EscSamePrint = {ESC, 'z', 0};         /* same print */
    ESC3BYTE    EscFeed      = {ESC, 'd', 0};         /* feed */
    ESC3BYTE    EscFeedLen  = {ESC, '3', PMG_SET_DEF_LF};        /* set feed length */

    
    aFeedData.ucSeqNo = pPrtCtrl.ucSndSeqNo++;
    
    usOffset = 0;
    
#if (NUMBER_OF_STATION > 1)
    /*--- select  stesion ---*/
    if (usPrtType == PMG_PRT_SLIP) {
        EscStation.uchData = PMG_SEL_SLIP;
#if defined (STATION_VALIDATION)
    } else if ((pSpoolCtrlR.fbModeSpool & SPLMD_VALIDATION) != 0) {
        EscStation.uchData = PMG_SEL_VALIDATION;
#endif
    } else if (usPrtType == PMG_PRT_RCT_JNL) {
        EscStation.uchData = PMG_SEL_JOURNAL + PMG_SEL_RECEIPT;
    } else if (usPrtType == PMG_PRT_RECEIPT) {
        EscStation.uchData = PMG_SEL_RECEIPT;
    } else if (usPrtType == PMG_PRT_JOURNAL) {
        EscStation.uchData = PMG_SEL_JOURNAL;
    }
    memcpy(&aFeedData.aucPrtMsg[usOffset], &EscStation, sizeof(EscStation));
    usOffset += sizeof(EscStation);
#endif

#if defined (PROVIDED_SAME_PRINT)
    if (usPrtType == PMG_PRT_RCT_JNL) {
        EscSamePrint.uchData = TRUE;
    } else {
        EscSamePrint.uchData = FALSE;
    }
    memcpy(&aFeedData.aucPrtMsg[usOffset], &EscSamePrint, sizeof(EscSamePrint));
    usOffset += sizeof(EscSamePrint);
#endif
    
    EscFeed.uchData = (UCHAR)usLine;                /* feed len*/
    memcpy(&aFeedData.aucPrtMsg[usOffset], &EscFeed, sizeof(EscFeed));
    usOffset += sizeof(EscFeed);

    pPrtCtrl.pPrtTally->usFeedLine += usLine;

    usOffset += (USHORT)OFFSET(PRT_SNDMSG, aucPrtMsg[0]);

    sRc = PmgWriteCom(usPrtType, &aFeedData, usOffset);

    /*--- if error then return with error code ---*/
    if (sRc <= 0) return(sRc);

    do {
        sRc = EscpReadCom(pPrtCtrl.hPort,
                            &aPrtResponce,
                            sizeof(aPrtResponce));
        if (sRc <= 0) break;

    } while (aPrtResponce.ucSeqNo != pPrtCtrl.ucRcvSeqNo);

    pPrtCtrl.ucRcvSeqNo++;
    
    if (sRc <= 0) return(sRc);

    *pfbStatus = aPrtResponce.ucPrtStatus;
    return(sRc);
}

/*
**************************************************************************
*
**Synopsis:     VOID    PmgSetValWaitCount(USHORT usRetryCount)
*               input   USHORT  usRetryCount;   : retry counter
*                                   = 0:    wait etertally
*                                   = X:    retry count 
**Return:       none
*
**Description:  This function set retry counter for checking validation sheet.
*               If retry over this counter then pmg send messege to printer
*               whether sheet exist or not. 
*
**************************************************************************
*/

VOID DevPmgSetValWaitCount(USHORT usRetryCount)
{
    usSaveValRetryCount = usRetryCount;
}   

/*
**************************************************************************
*
**Synopsis:     VOID  DevPmgSetCallBack(PCALL_BACK pCallBack)
*               input   PCALL_BACK pCallBack;   : Call Back
*
**Return:       none
*
**Description:  
*
**************************************************************************
*/

VOID DevPmgSetCallBack(PCALL_BACK pCallBack)
{
    pPmgCallBack = pCallBack;
}   

/**fh
;=============================================================
;
;   function : Adjust string with MaxColumn
;
;   Format   : UCHAR  RflStrAdjust(UCHAR *, USHORT, UCHAR *,  UCHAR, BOOL);       
;   input    : UCHAR  *pszDest,         -Destination buffer pointer
;              USHORT usDestLen,        -Destination buffer length
;              UCHAR  *pszSource,       -Source string pointer Length
;              UCHAR  uchMaxColumn,     -Max column
;              BOOL   fAutoFeed,        -Auto line feed flag
;
;   return   : UCHAR  uchWrtLen         -Written string's length (not include '\0')
;
;   output   : UCHAR  *pszDest           -Formated String
;
;   synopis  : This function formats string with MaxColumn 
;
;
;==============================================================
fh**/
static UCHAR  PmgStrAdjust(UCHAR *pszDest,  USHORT usDestLen, UCHAR *pszSource, UCHAR uchMaxColumn, BOOL fAutoFeed)
{
    UCHAR *puchRead;         /* Source buffer read pointer */
    UCHAR *puchWrite;        /* Destination buffer write pointer */
    UCHAR uchColPos;         /* Current Column position */
    UCHAR *puchWork;         /* Work Area */
    UCHAR auchTabString[80]; /* Tab stirng buffer  */
    UCHAR *puchTabWrt;       /* Tab stirng buffer write pointer */
   
    /* --- initialize ---*/
    memset(pszDest, ' ', usDestLen);
    puchRead   = pszSource;
    puchWrite  = pszDest;
    uchColPos  = 0;


    while ( (*puchRead != '\0')                         /* until string end */
          && (puchWrite < (pszDest+usDestLen - 1)) ) {  /* until destination buffer end */

		/*---------------------------------*\
			   --- Tab management ---
		\*---------------------------------*/
        if (*puchRead == '\t') {
			UCHAR uchTabChar = 0;        /* Tab string character counter */
			UCHAR uchWrtCount = 0;       /* Tab string write counter */
        
            /* --- initialize --- */
            memset(auchTabString, ' ', sizeof(auchTabString));
            uchTabChar = 0;                          /* Num. of written character */
            puchRead++;                              /* for character '\t' */
            puchTabWrt  = auchTabString;

            while ( (*puchRead != '\t') && (*puchRead != '\n') && (*puchRead != '\0') ) {
                *puchTabWrt++ = *puchRead++;
                uchTabChar++;
            }

            if (uchTabChar + uchColPos > uchMaxColumn) {
                if (fAutoFeed == RFL_FEED_ON) {
                    *puchWrite++ = '\n';
                    puchWrite += uchMaxColumn - 1;
                } else {
                    puchWrite += uchMaxColumn - uchColPos - 1;
                }
            } else {
                puchWrite += uchMaxColumn - uchColPos - 1;
            }

            /* --- Check write point --- */
            if ( puchWrite >=  (pszDest+usDestLen) )  {
                pszDest[usDestLen - 1] = '\0';   /* Change Dec-26-95 */
                return((UCHAR)(usDestLen - 1)); /* return written length Chg Dec-26-95 */
            }

            uchWrtCount = 0;
            puchTabWrt--;                       /* point the last char */ 

            /* --Write TabStringbufferdata to destination buffer -- */
            while ((auchTabString <= puchTabWrt) && (uchWrtCount < uchMaxColumn)) {
                *puchWrite-- = *puchTabWrt--;
                uchWrtCount++;
            }

            puchWrite += uchWrtCount + (UCHAR)1;    

		/*---------------------------------*\
		  --- Carrige return management ---
		\*---------------------------------*/
        } else if (*puchRead == '\n') {
                                 
            *puchWrite++ = *puchRead++;        /* write data, increment read & write point */
            uchColPos = 0;                     /* column position initialize */
		/*---------------------------------*\
		  --- other character management ---
		\*---------------------------------*/
        } else {

            /* when cureent column is max - 1column and going to write double wide character */                  
            if ( uchColPos == (uchMaxColumn - (UCHAR)1)   
                  && (UCHAR)*puchRead == (UCHAR)(0x12) ) {  /* 0x12: DOUBLE WIDE CHARACTER, RFL_DOUBLE */

                if (fAutoFeed == RFL_FEED_ON) {

                    *puchWrite++ = '\n';
                    
                    /* --- Check write point --- */
                    if ( puchWrite >=  (pszDest+usDestLen) )  {
                        *(pszDest + (usDestLen - 1)) = '\0';
                        return( (UCHAR)(usDestLen - 1) );
                    }

                    /* -- write data, increment read & write point -- */
                    *puchWrite++ = *puchRead++;

                    /* --- Check write point --- */
                    if ( puchWrite >=  (pszDest+usDestLen) )  {
                        *(pszDest + (usDestLen - 1)) = '\0';
                        return( (UCHAR)(usDestLen - 1) );
                    }
                    uchColPos = 1;
                } else {
                    puchRead++;                 /* write single wide character */
                }
            }

            *puchWrite++ = *puchRead++;         /* write data, increment read & write point */
            uchColPos++;                        /* increment column posit */

            /* -- Check current column position -- */
            if (uchColPos >= uchMaxColumn) {

                if (fAutoFeed == RFL_FEED_ON) {
                    *puchWrite++ = '\n';
                    uchColPos = 0;              /* initialize column position */
                } else {
                    /* -- move Readpoint(puchRead) to next '\t','\n' or '\0' -- */
                    /* - get '\t'  point - */
                    puchWork  = (unsigned char *)strchr((const char *)puchRead, '\t');       

                    /* - move readpoint to next '\n' - */
                    puchRead  = (unsigned char *)strchr((const char *)puchRead, '\n');

                    if ( (puchRead==NULL) && (puchWork!=NULL) ) {
                        puchRead = puchWork;
                    } else if ( puchRead == puchWork ) {
                        /* - move readpoint to string end - */
                        puchRead = (unsigned char *)strchr((const char *)pszDest, '\0');     
                    } else if ( (puchRead > puchWork) && (puchWork != NULL) ) {
                        /* - move readpoint to next '\t' - */
                        puchRead = puchWork;
                    }
                }
            }
        }
    }

    if (*puchWrite == '\n') {
        puchWrite--;                            /* delete last return */
    }

    *puchWrite = '\0';                          /* write null */

    return((UCHAR)(puchWrite - pszDest));       /* return written length */

}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgReadStatus(usPrtType, usLine, pfbStatus)
*               input   USHORT  usPrtType;  : type of printer
*               output  UCHAR   *pfbStatus  : responce status of printer
*
**Return:       Pif return status
*
**Description:  Get Paper Sensor Status
*
**************************************************************************
*/
USHORT PmgReadStatus(USHORT usPrtType, UCHAR *pfbStatus)
{
    UCHAR   aucSend[4];/*+sizeof(Escv)];*/
    SHORT           sRc;
    PRT_RESPONCE    aPrtResponce;
    SHORT       hPort;
   
    memset(aucSend,0,sizeof(aucSend));
    aucSend[3] = pPrtCtrl.ucSndSeqNo++;

    sRc = PmgWriteCom(usPrtType, &aucSend, sizeof(aucSend));
    /*--- if error then return with error code ---*/
    if (sRc > 0){

        do {
            sRc = EscpReadCom(pPrtCtrl.hPort,
                                &aPrtResponce,
                                sizeof(aPrtResponce));
            if (sRc <= 0) break;

        } while (aPrtResponce.ucSeqNo != pPrtCtrl.ucRcvSeqNo);

        pPrtCtrl.ucRcvSeqNo++;
    }

     hPort = pPrtCtrl.hPort;
     EscpControlCom(hPort,
                        PIF_PIP_GET_STATUS,
                        (UCHAR FAR *)pfbStatus,
                        sizeof(pfbStatus));
     if(sRc > 0)    return PIF_OK;
     return sRc;
}
HANDLE  DevGetPrtHandle()
{
    if(pPrtCtrl.hPort < 0){
        return INVALID_HANDLE_VALUE;
    }else{
        return (HANDLE)pPrtCtrl.hPort;
    }
}
BOOL    DevCheckPrtHandle(SHORT handle)
{
    if((handle != pPrtCtrl.hPort)||(handle < 0)){
        return FALSE; 
    }else{
        return TRUE;
    }
}
/***    End of PMGIF.C    ***/

