/*
**************************************************************************
**
**  Title:      Print Manager Common Routine
**
**  Categoly:   Print Manager
**
**  Abstruct:   Common Routine of Print Manager
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
      Rev.   |  Date      |  Comment
;-----------------------------------------------------------------------------
;   00.00.00 |            | Initial
;            | 2000-04-11 | Added Code Page initialization <#001#>
;            | 2000-04-11 | Modified Initialization command sequence <#002#>
;            | 2000-04-12 | Modified 7458 Station select command seq. <#003#>
;     V1.0.5 | 2000-10-01 | Code Page for 7158/7194 changed to 852 <#004#>
;            |            | (euro symbol was required)
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
#include    <tally.h>
#include    <pmg.h>
#include    "devpmg.h"
#include    "pmgif.h"
#include    "pmgcmn.h"
#include    "printio.h"
#include    "ESCP.H"
#include    "SCF.H"

extern  PRTCTRL     pPrtCtrl;
extern  SPOOLCNT    pSpoolCtrlR;
extern  SPOOLCNT    pSpoolCtrlJ;
extern  SPOOLCNT    pSpoolCtrlS;
extern  PCALL_BACK  pPmgCallBack;
extern  SERIAL_CONFIG   SerialConf;

/**
;=============================================================================
;+                                                                           +
;+                                  M A C R O s                              +
;+                                                                           +
;=============================================================================
**/

#ifndef _WINDEF_
#define max(a, b)       ((a) > (b) ? (a) : (b))     /* which is max value ? */
#endif

/**
;========================================================================
;+                                                                      +
;+            S T A T I C    R A M    A S S I G N M E N T               +
;+                                                                      +
;========================================================================
**/
USHORT      hSemaphoreRJ;               /* semaphore ID of R/J      */
USHORT      hSemaphoreS;                /* semaphore ID of slip     */
USHORT      hSemaphoreCmn;              /* semaphore ID of Common   */

USHORT      hPmgWaitSem;                /* handle of semaphore      */
                                        /*  (PmgWait)               */
USHORT      hPmgImpRctSem;              /* handle of semaphore      */
USHORT      hPmgImpJnlSem;              /*  (PmgEndImportant)       */
USHORT      hPmgImpSlpSem;              /*                          */

PMGSEM      PmgSem[ID_MAX+1];           /* semaphore control        */

UCHAR       ucCodeTable;                /* character code table slip    */
UCHAR       ucFontTypeRJ;                 /* font type                    */
UCHAR       ucFeedLenRJ;                /* feed length R/J              */
UCHAR       ucFeedLenS;                 /* feed length Slip             */

CHAR        aszFileMode[]               /* open file mode */
                ="ro";
/*
*********************************************************************
*
**Synopsis:     VOID    PmgCreateSem(VOID)
*
**Return:       Non
*
**Description:  Create Semaphore
*
*********************************************************************
*/

VOID PmgCreateSem(VOID)
{
    hSemaphoreCmn = PifCreateSem(1);
    hSemaphoreRJ = PifCreateSem(1);
    hSemaphoreS = PifCreateSem(1);

    hPmgWaitSem = PifCreateSem(0);          /* for PmgWait()            */

    hPmgImpRctSem = PifCreateSem(0);        /* for PmgEndImportant()    */
    hPmgImpJnlSem = PifCreateSem(0);
    hPmgImpSlpSem = PifCreateSem(0);

    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgCreateWait(VOID)
*
**Return:       Non
*
**Description:  Create wait
*
*********************************************************************
*/

VOID PmgCreateWait(VOID)
{
    SHORT   i;


    for (i=0; i <= ID_MAX; i++) {

        PmgSem[i].hSemaphore = PifCreateSem(0);
        PmgSem[i].fbWaitFlag = 0;
        PmgSem[i].chCount = 0;

    }

    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgRequestSem(usPrtID)
*               input   USHORT  usPrtID;    : printer ID
*
**Return:       Non
*
**Description:  Request Semaphore
*
*********************************************************************
*/

VOID PmgRequestSem(USHORT usPrtID)
{
    if (usPrtID == PMG_PRT_SLIP) {
        PifRequestSem(hSemaphoreS);
    } else {
        PifRequestSem(hSemaphoreRJ);
    }

    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgReleaseSem(usPrtID)
*               input   USHORT  usPrtID;    : printer ID
*
**Return:       Non
*
**Description:  Release Semaphore
*
*********************************************************************
*/

VOID PmgReleaseSem(USHORT usPrtID)
{
    if (usPrtID == PMG_PRT_SLIP) {
        PifReleaseSem(hSemaphoreS);
    } else {
        PifReleaseSem(hSemaphoreRJ);
    }

    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgRequestSemRJ(VOID)
**Synopsis:     VOID    PmgReleaseSemRJ(VOID)
*
**Return:       Non
*
**Description:  Request Semaphore
**Description:  Release Semaphore
*
*********************************************************************
*/

VOID PmgRequestSemRJ(VOID)
{
    PifRequestSem(hSemaphoreRJ);

    return;
}

VOID PmgReleaseSemRJ(VOID)
{
    PifReleaseSem(hSemaphoreRJ);

    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgRequestSemS(VOID)
**Synopsis:     VOID    PmgReleaseSemS(VOID)
*
**Return:       Non
*
**Description:  Request Semaphore
**Description:  Release Semaphore
*
*********************************************************************
*/

VOID PmgRequestSemS(VOID)
{
    PifRequestSem(hSemaphoreS);

    return;
}

VOID PmgReleaseSemS(VOID)
{
    PifReleaseSem(hSemaphoreS);

    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgRequestSemCmn(VOID)
**Synopsis:     VOID    PmgReleaseSemCmn(VOID)
*
**Return:       Non
*
**Description:  Request Semaphore
**Description:  Release Semaphore
*
*********************************************************************
*/

VOID PmgRequestSemCmn(VOID)
{
    PifRequestSem(hSemaphoreCmn);

    return;
}

VOID PmgReleaseSemCmn(VOID)
{
    PifReleaseSem(hSemaphoreCmn);

    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgRequestWaitSem(VOID)
**Synopsis:     VOID    PmgReleaseWaitSem(VOID)
*
**Return:       Non
*
**Description:  Request Semaphore
**Description:  Release Semaphore
*
*********************************************************************
*/

VOID PmgRequestWaitSem(VOID)
{
    PifRequestSem(hPmgWaitSem);

    return;
}

VOID PmgReleaseWaitSem(VOID)
{
    PifReleaseSem(hPmgWaitSem);

    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgRequestImpSemR(VOID)
**Synopsis:     VOID    PmgReleaseImpSemR(VOID)
*
**Return:       Non
*
**Description:  Request Semaphore
**Description:  Release Semaphore
*
*********************************************************************
*/

VOID PmgRequestImpSemR(VOID)
{
    PifRequestSem(hPmgImpRctSem);

    return;
}

VOID PmgReleaseImpSemR(VOID)
{
    PifReleaseSem(hPmgImpRctSem);

    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgRequestImpSemJ(VOID)
**Synopsis:     VOID    PmgReleaseImpSemJ(VOID)
*
**Return:       Non
*
**Description:  Request Semaphore
**Description:  Release Semaphore
*
*********************************************************************
*/

VOID PmgRequestImpSemJ(VOID)
{
    PifRequestSem(hPmgImpJnlSem);

    return;
}

VOID PmgReleaseImpSemJ(VOID)
{
    PifReleaseSem(hPmgImpJnlSem);

    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgRequestImpSemS(VOID)
**Synopsis:     VOID    PmgReleaseImpSemS(VOID)
*
**Return:       Non
*
**Description:  Request Semaphore
**Description:  Release Semaphore
*
*********************************************************************
*/

VOID PmgRequestImpSemS(VOID)
{
    PifRequestSem(hPmgImpSlpSem);

    return;
}

VOID PmgReleaseImpSemS(VOID)
{
    PifReleaseSem(hPmgImpSlpSem);

    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgSleep(usUserID, fbType, usTimer)
*               input   USHORT  usUserID;   : user ID
*               input   UCHAR   fbType;     : wait type
*               input   USHORT  usTimer;    : timer value
*
**Return:       Non
*
**Description:  sleep
*
*********************************************************************
*/

VOID PmgSleep(USHORT usUserID, UCHAR fbType, USHORT usTimer)
{
    if (usUserID > ID_MAX) return;

    PmgRequestSemCmn();

    PmgSem[usUserID].fbWaitFlag |= fbType;

    if (usTimer != 0) {

        PmgReleaseSemCmn();
        PifSleep(usTimer);

    } else {

        if ((PmgSem[usUserID].fbWaitFlag & WAIT_WAKEUP) == 0) {
            PmgSem[usUserID].chCount++;
            PmgReleaseSemCmn();

            PifRequestSem(PmgSem[usUserID].hSemaphore);

        } else {
            PmgSem[usUserID].fbWaitFlag = 0;
            PmgReleaseSemCmn();

        }
    }
    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgWakeUp(usUserID)
*               input   USHORT  usUserID;   : user ID
*
**Return:       Non
*
**Description:  wake up
*
*********************************************************************
*/

VOID PmgWakeUp(USHORT usUserID)
{
    if (usUserID > ID_MAX) return;

    PmgRequestSemCmn();

    if (PmgSem[usUserID].chCount == 0) {
        PmgSem[usUserID].fbWaitFlag = WAIT_WAKEUP;

    } else {
        PmgSem[usUserID].fbWaitFlag = 0;

    }

    while (PmgSem[usUserID].chCount > 0) {
        PifReleaseSem(PmgSem[usUserID].hSemaphore);
        PmgSem[usUserID].chCount--;
    }

    PmgReleaseSemCmn();
    return;
}


/*
*********************************************************************
*
**Synopsis:     SHORT   PmgChkSleep(usUserID, fbType)
*               input   USHORT  usUserID;   : user ID
*               input   UCHAR   fbType;     : wait type
*
**Return:       select pattern
*
**Description:  check sleep status
*
*********************************************************************
*/

USHORT PmgChkSleep(USHORT usUserID, UCHAR fbType)
{
    USHORT  usRc;


    if (usUserID > ID_MAX) return(0);

    PmgRequestSemCmn();

    usRc = PmgSem[usUserID].fbWaitFlag & fbType;

    PmgReleaseSemCmn();

    return(usRc);
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgSetCurStat(usPrtType, fbMode)
*               input   USHORT  usPrtType;  : printer type
*               input   UCHAR   fbMode;     : mode of spool buffer
*
**Return:       Non
*
**Description:  Set printer execution mode
*
*********************************************************************
*/

VOID PmgSetCurStat(USHORT usPrtType, UCHAR fbMode)
{
    switch (usPrtType) {
    case PMG_PRT_SLIP:
#if defined (STATION_SLIP)
        pSpoolCtrlS.fbModeSpool |= fbMode;
#endif
        break;
    case PMG_PRT_RECEIPT:
    case PMG_PRT_RCT_JNL:
        pSpoolCtrlR.fbModeSpool |= fbMode;
        break;
    case PMG_PRT_JOURNAL:
        pSpoolCtrlJ.fbModeSpool |= fbMode;
        break;
    default:
        break;
    }

    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgResetCurStat(usPrtType, fbMode)
*               input   USHORT  usPrtType;  : printer type
*               input   UCHAR   fbMode;     : mode of spool buffer
*
**Return:       Non
*
**Description:  Reset printer execution mode
*
*********************************************************************
*/

VOID PmgResetCurStat(USHORT usPrtType, UCHAR fbMode)
{
    switch (usPrtType) {
    case PMG_PRT_SLIP:
#if defined (STATION_SLIP)
        pSpoolCtrlS.fbModeSpool &= (0xff - fbMode);
#endif
        break;
    case PMG_PRT_RECEIPT:
    case PMG_PRT_RCT_JNL:
        pSpoolCtrlR.fbModeSpool &= (0xff - fbMode);
        break;
    case PMG_PRT_JOURNAL:
        pSpoolCtrlJ.fbModeSpool &= (0xff - fbMode);
        break;
    default:
        break;
    }

    return;
}


/*
*********************************************************************
*
**Synopsis:     UCHAR   PmgGetCurStat(usPrtType)
*               input   USHORT  usPrtType;  : printer type
*
**Return:       mode of spool buffer
*
**Description:  Get printer execution mode
*
*********************************************************************
*/

UCHAR PmgGetCurStat(USHORT usPrtType)
{
    UCHAR   fbMode;


    switch (usPrtType) {
    case PMG_PRT_SLIP:
#if defined (STATION_SLIP)
        fbMode = pSpoolCtrlS.fbModeSpool;
#endif
        break;
    case PMG_PRT_RECEIPT:
    case PMG_PRT_RCT_JNL:
        fbMode = pSpoolCtrlR.fbModeSpool;
        break;
    case PMG_PRT_JOURNAL:
        fbMode = pSpoolCtrlJ.fbModeSpool;
        break;
    default:
        fbMode = 0;
        break;
    }

    return(fbMode);
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgChkCurStat(usPrtType, fbMode)
*               input   USHORT  usPrtType;  : printer type
*               input   UCHAR   fbMode;     : mode of spool buffer
*
**Return:       Non
*
**Description:  Test printer execution mode
*
*********************************************************************
*/

UCHAR PmgChkCurStat(USHORT usPrtType, UCHAR fbMode)
{
    switch (usPrtType) {
    case PMG_PRT_SLIP:
#if defined (STATION_SLIP)
        fbMode &= pSpoolCtrlS.fbModeSpool;
#endif
        break;
    case PMG_PRT_RECEIPT:
    case PMG_PRT_RCT_JNL:
        fbMode &= pSpoolCtrlR.fbModeSpool;
        break;
    case PMG_PRT_JOURNAL:
        fbMode &= pSpoolCtrlJ.fbModeSpool;
        break;
    default:
        fbMode = 0;
        break;
    }

    return(fbMode);
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgSetInputStat(usPrtType, fbMode)
*               input   USHORT  usPrtType;  : printer type
*               input   UCHAR   fbMode;     : mode of spool buffer
*
**Return:       Non
*
**Description:  Set input data mode
*
*********************************************************************
*/

VOID PmgSetInputStat(USHORT usPrtType, UCHAR fbMode)
{
    switch (usPrtType) {
    case PMG_PRT_SLIP:
#if defined (STATION_SLIP)
        pSpoolCtrlS.fbModeInput |= fbMode;
#endif
        break;
    case PMG_PRT_RECEIPT:
    case PMG_PRT_RCT_JNL:
        pSpoolCtrlR.fbModeInput |= fbMode;
        break;
    case PMG_PRT_JOURNAL:
        pSpoolCtrlJ.fbModeInput |= fbMode;
        break;
    default:
        break;
    }

    return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgResetInputStat(usPrtType, fbMode)
*               input   USHORT  usPrtType;  : printer type
*               input   UCHAR   fbMode;     : mode of spool buffer
*
**Return:       Non
*
**Description:  Reset input data mode
*
*********************************************************************
*/

VOID PmgResetInputStat(USHORT usPrtType, UCHAR fbMode)
{
    switch (usPrtType) {
    case PMG_PRT_SLIP:
#if defined (STATION_SLIP)
        pSpoolCtrlS.fbModeInput &= (0xff - fbMode);
#endif
        break;
    case PMG_PRT_RECEIPT:
    case PMG_PRT_RCT_JNL:
        pSpoolCtrlR.fbModeInput &= (0xff - fbMode);
        break;
    case PMG_PRT_JOURNAL:
        pSpoolCtrlJ.fbModeInput &= (0xff - fbMode);
        break;
    default:
        break;
    }

    return;
}


/*
*********************************************************************
*
**Synopsis:     UCHAR   PmgGetInputStat(usPrtType)
*               input   USHORT  usPrtType;  : printer type
*
**Return:       fbMode;     : mode of spool buffer
*
**Description:  Get input data mode
*
*********************************************************************
*/

UCHAR PmgGetInputStat(USHORT usPrtType)
{
    UCHAR   fbMode;


    switch (usPrtType) {
    case PMG_PRT_SLIP:
#if defined (STATION_SLIP)
        fbMode = pSpoolCtrlS.fbModeInput;
#endif
        break;
    case PMG_PRT_RECEIPT:
    case PMG_PRT_RCT_JNL:
        fbMode = pSpoolCtrlR.fbModeInput;
        break;
    case PMG_PRT_JOURNAL:
        fbMode = pSpoolCtrlJ.fbModeInput;
        break;
    default:
        fbMode = 0;
        break;
    }

    return(fbMode);
}


/*
*********************************************************************
*
**Synopsis:     SHORT   PmgCmpSequence(ucSeqNoA, ucSeqNoB)
*               input   UCHAR   ucSeqNoA;   : sequence number - A
*               input   UCHAR   ucSeqNoB;   : sequence number - B
*
**Return:       A > B   : 1
*               A = B   : 0
*               A < B   : -1
*
**Description:  Compare sequence number
*
*********************************************************************
*/

SHORT PmgCmpSequence(UCHAR ucSeqNoA, UCHAR ucSeqNoB)
{
    UCHAR   ucCmp;

    ucCmp = ucSeqNoA - ucSeqNoB;

    if (ucCmp == 0) return(0);
    if (ucCmp <= 0x80) return(1);
    return(-1);
}


/*
*******************************************************************************
*
**Synopsis:     VOID    PmgGetCtrl(usPrtType, pPrtCtrl, pSpoolCtrl)
*               input   USHORT      usPrtType;      : printer type
*               output  PRTCTRL     **pPrtCtrl;     : printer control block
*               output  SPOOLCNT    **pSpoolCtrl;   : spool control block
*
**Return:       non
*
**Description:  get printer control data
*
*******************************************************************************
*/


SPOOLCNT *PmgGetSpoolCtrl(USHORT usPrtType)
{
	SPOOLCNT *pSpoolCtrl = NULL;

	if (usPrtType == PMG_PRT_SLIP) {
#if defined (STATION_SLIP)
		pSpoolCtrl = &pSpoolCtrlS;
#endif
	}
	else if (usPrtType == PMG_PRT_JOURNAL) {
		pSpoolCtrl = &pSpoolCtrlJ;
	}
	else {
		pSpoolCtrl = &pSpoolCtrlR;
	}

	return pSpoolCtrl;
}

VOID    PmgGetCtrl(USHORT usPrtType, PRTCTRL **pPrtCtrlDest, SPOOLCNT **pSpoolCtrl)
{
    *pPrtCtrlDest = &pPrtCtrl;
	*pSpoolCtrl = PmgGetSpoolCtrl(usPrtType);
	NHPOS_ASSERT(*pSpoolCtrl);

    return;
}


/*
*******************************************************************************
*
**Synopsis:     VOID    PmgStopSend(usPrtType)
*               input   USHORT      usPrtType;      : printer type
*
**Return:       Non
*
**Description:  Request to stop send thread
*
*******************************************************************************
*/

VOID    PmgStopSend(USHORT usPrtType)
{
    pPrtCtrl.ucStatReceive |= SPLST_ERROR;
    return;
    
    usPrtType = usPrtType;
}


/*
*******************************************************************************
*
**Synopsis:     VOID    PmgRestartSend(usPrtType)
*               input   USHORT      usPrtType;      : printer type
*
**Return:       Non
*
**Description:  Request to restart send thread
*
*******************************************************************************
*/

VOID    PmgRestartSend(USHORT usPrtType)
{
    pPrtCtrl.ucStatReceive &= (0xff - SPLST_ERROR);
    PmgWakeUp(ID_SEND);

    return;
    
    usPrtType = usPrtType;
}


/*
*******************************************************************************
*
**Synopsis:     VOID    PmgMemoryEsc(usPrtType, pSpoolData)
*               input   USHORT      usPrtType;      : printer type
*               input   UCHAR       *pSpoolData;    : print data
*
**Return:       Non
*
**Description:  open the communication
*
*******************************************************************************
*/

VOID PmgMemoryEsc(USHORT usPrtType, UCHAR *pSpoolData, USHORT usLen)
{
    USHORT i, usEscLen; 

    for (i = 0; i < usLen ; i += usEscLen) {

        usEscLen = 1;
        
        if (pSpoolData[i] == ESC) {

            switch (pSpoolData[i+1]) {
            case 't':                       /*--- character code table ---*/
                usEscLen = 3;
                if (i+usEscLen > usLen) return;
                ucCodeTable  = pSpoolData[i+2];
                break;
            case '!':                       /*--- font type ---*/
                usEscLen = 3;
                if (i+usEscLen > usLen) return;
                if (usPrtType != PMG_PRT_SLIP) ucFontTypeRJ = pSpoolData[i+2];
                break;
            case '3':                       /*--- font type ---*/
                usEscLen = 3;
                if (i+usEscLen > usLen) return;
                if (usPrtType == PMG_PRT_SLIP) ucFeedLenS  = pSpoolData[i+2];
                else                           ucFeedLenRJ = pSpoolData[i+2];
                break;
            case '2':                       /*--- font type ---*/
                usEscLen = 3;
                if (i+usEscLen > usLen) return;
                if (usPrtType == PMG_PRT_SLIP) ucFeedLenS  = pSpoolData[i+2];
                else                           ucFeedLenRJ = pSpoolData[i+2];
                break;
            default:
                break;
            }
        }
        
    }
    
    return;
}


/*
*******************************************************************************
*
**Synopsis:     VOID    PmgReOpenPrt(usPrtType)
*               input   USHORT      usPrtType;      : printer type
*
**Return:       Non
*
**Description:  open the communication
*
*******************************************************************************
*/

VOID PmgReOpenPrt(USHORT usPrtType)
{
    PROTOCOL    pProtocol;

    PmgGetSerialConfig(&SerialConf);

    pPrtCtrl.sPortNo = (SHORT)SerialConf.uchComPort;

    /*--- close communication ---*/
    EscpCloseCom(pPrtCtrl.hPort);

     /* set port number and flow control */
        
    pProtocol.fPip             = SerialConf.usPip;
    pProtocol.uchComByteFormat = SerialConf.uchComByteFormat;
    pProtocol.ulComBaud = SerialConf.ulComBaud;
        
    pPrtCtrl.hPort = EscpOpenCom(pPrtCtrl.sPortNo , &pProtocol);
    
    return;
    
    usPrtType = usPrtType;
}


/*
*******************************************************************************
*
**Synopsis:     USHORT  PmgResetPrt(usPrtType)
*               input   USHORT      usPrtType;      : printer type
*
**Return:       TRUE
*               FALSE
*
**Description:  Send reset command to printer
*
*******************************************************************************
*/

USHORT  PmgResetPrt(USHORT usPrtType)
{
    SHORT   sRc;
    USHORT  hPort;
    PRT_SNDMSG      aResetData;
    PRT_RESPONCE    pPrtResponce;
    USHORT          usOffset;

    ESC2BYTE    EscReset = {ESC, '@'};               /* initialize printer */

#if defined(DEVICE_7140) || defined (DEVICE_7161)                           /* M START <#001#> */
    /* ESC3BYTE    EscCharTable = {ESC, 't', 0}; */     /* set character code table */
    ESC3BYTE    EscCharTable = {ESC, 't', 2};        /* set character code table */
#elif defined(DEVICE_7158) || defined(DEVICE_7194)                          /* A <#004#> */
    ESC3BYTE    EscCharTable = {ESC, 't', 6};                               /* A <#004#> */
#else
    ESC3BYTE    EscCharTable = {ESC, 't', 1};        /* set character code table */
#endif                                                                      /* M END <#001#> */

    ESC3BYTE    EscFontType = {ESC, '!', 0};         /* set font type */

#if defined (DEVICE_7158)                                                   /* M START <#003#> */
    ESC4BYTE    EscStation  = {ESC, 'c', '1', 1};    /* select stesion */
#else /* DEVICE_7158 */
    ESC4BYTE    EscStation  = {ESC, 'c', '0', 1};    /* select stesion */
#endif /* !DEVICE_7158 */                                                   /* M END <#003#> */

    ESC3BYTE    EscFeedLen  = {ESC, '3', PMG_SET_DEF_LF};        /* set feed length */

#if defined (DEVICE_7158)
    ESC4BYTE    EscSlipWait  = {ESC, 'f', 0, 0};
    ESC4BYTE    EscSelSensor = {ESC, 'c', '4', 0x30};
#endif

    if (pPrtCtrl.hPort < 0) return(FALSE);

    /*
     * 1st initialization stage (Added <#002#>)
     */
    {
#if defined (DEVICE_7158)                                                   /* M START <#003#> */
        CHAR  DleEnq[] = "\x10\x05\x01\x10\x05\x02\x10\x05\x03";    /* initialize printer (as if powered on) */
#else
#if defined (DEVICE_7194)
        CHAR  DleEnq[] = "\x10\x05\x01\x10\x05\x02";    /* initialize printer (as if powered on) */
#else
        CHAR  DleEnq[] = "\x10\x05\x02";    /* initialize printer (as if powered on) */
#endif
#endif
        SHORT sLength;                      /* length of command buffer to transmit */

        /* issue the realtime command 'Recover and clear buffers' */
        sLength = strlen(DleEnq);
        sRc = PifWriteCom(pPrtCtrl.hPort, DleEnq, sLength);
        if (sRc != sLength) {
            return FALSE;
        }

        /* The issued command causes printer busy roughly 1 second */
#if defined (DEVICE_7158) || defined (DEVICE_7194)
        PifSleep(600);
#else
        PifSleep(1200);
#endif
    }

    /*
     * 2nd initialization stage
     */
    usOffset = 0;
    
    /*--- set printer reset command ---*/
    memcpy(&aResetData.aucPrtMsg[usOffset], &EscReset, sizeof(EscReset));
    usOffset += sizeof(EscReset);
    
    /*--- set character code table ---*/
    /* EscCharTable.uchData = ucCodeTable; */                               /* D <#001#> */
    memcpy(&aResetData.aucPrtMsg[usOffset], &EscCharTable, sizeof(EscCharTable));
    usOffset += sizeof(EscCharTable);

#if defined (PROVIDED_FONT_TYPE_SELECTION)
    /*--- set character code table ---*/
    EscFontType.uchData = ucFontTypeRJ;
    memcpy(&aResetData.aucPrtMsg[usOffset], &EscFontType, sizeof(EscFontType));
    usOffset += sizeof(EscFontType);
#endif

#if defined (STATION_RECEIPT)
#if (NUMBER_OF_STATION > 1)
    /*--- select R stesion ---*/
    EscStation.uchData = PMG_SEL_RECEIPT;
    memcpy(&aResetData.aucPrtMsg[usOffset], &EscStation, sizeof(EscStation));
    usOffset += sizeof(EscStation);
#endif
    /*--- set character code table ---*/
    EscFeedLen.uchData = ucFeedLenRJ;
    memcpy(&aResetData.aucPrtMsg[usOffset], &EscFeedLen, sizeof(EscFeedLen));
    usOffset += sizeof(EscFeedLen);
#endif
    
#if defined (STATION_SLIP)
    /*--- select S stesion ---*/
    EscStation.uchData = PMG_SEL_SLIP;
    /* #if defined (DEVICE_7158)      D <#003#> */
    /*   EscStation.uchCmd2 = '1';    D <#003#> */
    /* #endif                         D <#003#> */
    memcpy(&aResetData.aucPrtMsg[usOffset], &EscStation, sizeof(EscStation));
    usOffset += sizeof(EscStation);

    /*--- set character code table ---*/
    EscFeedLen.uchData = ucFeedLenS;
    memcpy(&aResetData.aucPrtMsg[usOffset], &EscFeedLen, sizeof(EscFeedLen));
    usOffset += sizeof(EscFeedLen);

#if defined (DEVICE_7158)
    /* --- set slip paper waiting time (infinite) --- */
    memcpy(&aResetData.aucPrtMsg[usOffset], &EscSlipWait, sizeof(EscSlipWait));
    usOffset += sizeof(EscSlipWait);

    /* --- select trailing edge sensor to stop printing --- */
    memcpy(&aResetData.aucPrtMsg[usOffset], &EscSelSensor, sizeof(EscSelSensor));
    usOffset += sizeof(EscSelSensor);
#endif
#endif

    sRc = EscpControlCom(pPrtCtrl.hPort, PIF_PIP_RESET_SEQ);
    if (sRc == PIF_ERROR_COM_POWER_FAILURE) PmgReOpenPrt(usPrtType);
    if (sRc < 0) return(FALSE);

    pPrtCtrl.ucRcvSeqNo = pPrtCtrl.ucSndSeqNo = 0;
    usOffset += (USHORT)OFFSET(PRT_SNDMSG, aucPrtMsg[0]);
    sRc = EscpWriteCom(pPrtCtrl.hPort, &aResetData, usOffset);
    sRc = EscpReadCom(pPrtCtrl.hPort,
                     &pPrtResponce,
                     sizeof(pPrtResponce));
    pPrtCtrl.ucRcvSeqNo = pPrtCtrl.ucSndSeqNo;
    hPort = pPrtCtrl.hPort;

    while ((sRc > 0) && (pPrtResponce.ucSeqNo != 0)) {
        sRc = EscpReadCom(hPort,
                         &pPrtResponce,
                         sizeof(pPrtResponce));
        
    }

    if (sRc == PIF_ERROR_COM_POWER_FAILURE) PmgReOpenPrt(usPrtType);

    return(TRUE);
//    if (sRc > 0) return(PmgSendInitData(usPrtType));
//    return(FALSE);
}


/*
*******************************************************************************
*
**Synopsis:     PmgWriteCom(usPrtType, *pBuffer, usBytes)
*
*               input   USHORT      usPrtType;  : printer type
*               input   VOID FAR    *pBuffer;   : data buffer
*               input   USHORT      usBytes;    : data length
*
**Return:       pip error code
*
**Description:  write to communication port
*
*******************************************************************************
*/

SHORT PmgWriteCom(USHORT usPrtType, CONST VOID  *pBuffer, USHORT usBytes)
{
    SHORT   sRc;                /* return status            */
    SHORT   sErrLoop;           /* loop counter             */
    SHORT   sBusyLoop;          /* loop counter             */


    for (sErrLoop = 0; sErrLoop < 5; sErrLoop++) {
        for (sRc = sBusyLoop = 0; sRc == 0; sBusyLoop++) {
            sRc = EscpWriteCom(pPrtCtrl.hPort, (VOID FAR *)pBuffer, usBytes);

            if (sRc == 0) PifSleep(50);

            if (sBusyLoop >= 50) sRc = PIF_ERROR_COM_TIMEOUT;
        }
        if (sRc > 0) break;
        if (sRc == PIF_ERROR_COM_POWER_FAILURE) break;
        if (sRc == PIF_ERROR_COM_TIMEOUT) break;
    }

    return(sRc);

    usPrtType = usPrtType;
}


/*
*******************************************************************************
*
**Synopsis:     PmgReadCom(usPrtType, *pBuffer, usBytes)
*
*               input   USHORT      usPrtType;  : printer type
*               input   VOID FAR    *pBuffer;   : data buffer
*               input   USHORT      usBytes;    : data length
*
**Return:       pip error code
*
**Description:  read from communication port
*
*******************************************************************************
*/

SHORT PmgReadCom(USHORT usPrtType, VOID FAR *pBuffer, USHORT usBytes)
{
    SHORT   sRc;                /* return status            */
    SHORT   sErrLoop;           /* loop counter             */
    SHORT   sBusyLoop;          /* loop counter             */


    for (sErrLoop = 0; sErrLoop < 3; sErrLoop++) {
        for (sRc = sBusyLoop = 0; sRc == 0; sBusyLoop++) {
            sRc = EscpReadCom(pPrtCtrl.hPort, pBuffer, usBytes);

            if (sBusyLoop >= 10) sRc = PIF_ERROR_COM_TIMEOUT;
        }

        if (sRc > 0) break;
        if (sRc == PIF_ERROR_COM_POWER_FAILURE) break;
        if (sRc == PIF_ERROR_COM_TIMEOUT) break;
    }

    return(sRc);
    
    usPrtType = usPrtType;
}

/*
*******************************************************************************
*
**Synopsis:     VOID PmgErrorMsg(USHORT usErrorCode)
*
*               input   USHORT usErrorCode;  : Error Code
*
**Return:       error code
*
**Description:  Notify error code to registered callback address
*
*******************************************************************************
*/

VOID PmgErrorMsg(USHORT usErrorCode)
{
    if (pPmgCallBack == NULL) 
    {
        return;
    }

    pPmgCallBack((DWORD)usErrorCode);

    return;
    
}

/*
*******************************************************************************
*
**Synopsis:     BOOL PmgGetSerialCinfig(PSERIAL_CONFIG pSerialConf)
*
*               input   USHORT usErrorCode;  : Error Code
*
**Return:       error code
*
**Description:  Notify error code to registered callback address
*
*******************************************************************************
*/

BOOL PmgGetSerialConfig(PSERIAL_CONFIG pSerialConf)
{
    DWORD   dwCount;
    DWORD   dwData, dwDataType, dwDummy;
    TCHAR   atchName[SCF_USER_BUFFER];
    TCHAR   atchPort[SCF_USER_BUFFER];

    GetDeviceName(&dwCount, atchName);

#if defined (DEVICE_7161)
    pSerialConf->usPip            = PIF_COM_PROTOCOL_XON;
#else
    pSerialConf->usPip            = PIF_COM_PROTOCOL_NON;
#endif
    pSerialConf->uchComPort       = 0xFF;
    pSerialConf->ulComBaud        = 9600;
    pSerialConf->uchComByteFormat = 0;

    if (dwCount)
    {
        if (GetParameter(   atchName, 
                            _T("Port"), 
                            &dwDataType, 
                            (LPVOID)atchPort, 
                            sizeof(atchPort), 
                            &dwDummy) != SCF_SUCCESS)
        {
            return FALSE;
        }
        pSerialConf->uchComPort = (UCHAR)(*((WORD *)atchPort)-*((WORD *)_T("0")));


        if (GetParameter(   atchName, 
                            SCF_DATANAME_BAUD, 
                            &dwDataType, 
                            (LPVOID)&dwData, 
                            sizeof(dwData), 
                            &dwDummy) != SCF_SUCCESS)
        {
            return FALSE;
        }

        switch (dwData)
        {
        case SCF_CAPS_BAUD_300:
            pSerialConf->ulComBaud = 300;
            break;
        case SCF_CAPS_BAUD_600:
            pSerialConf->ulComBaud = 600;
            break;
        case SCF_CAPS_BAUD_1200:
            pSerialConf->ulComBaud = 1200;
            break;
        case SCF_CAPS_BAUD_2400:
            pSerialConf->ulComBaud = 2400;
            break;
        case SCF_CAPS_BAUD_4800:
            pSerialConf->ulComBaud = 4800;
            break;
        case SCF_CAPS_BAUD_9600:
            pSerialConf->ulComBaud = 9600;
            break;
        case SCF_CAPS_BAUD_19200:
            pSerialConf->ulComBaud = 19200;
            break;
        case SCF_CAPS_BAUD_38400:
            pSerialConf->ulComBaud = 38400;
            break;
        case SCF_CAPS_BAUD_57600:
            pSerialConf->ulComBaud = 57600;
            break;
        case SCF_CAPS_BAUD_115200:
			pSerialConf->ulComBaud = 115200;
			break;
		default:
            pSerialConf->ulComBaud = 0;
            break;
        }


        if (GetParameter(   atchName, 
                            SCF_DATANAME_CHARBIT, 
                            &dwDataType, 
                            (LPVOID)&dwData, 
                            sizeof(dwData), 
                            &dwDummy) != SCF_SUCCESS)
        {
            return FALSE;
        }

        switch (dwData)
        {
        case SCF_CAPS_CHAR_8:
            pSerialConf->uchComByteFormat |= COM_BYTE_8_BITS_DATA;
            break;
        case SCF_CAPS_CHAR_7:
            pSerialConf->uchComByteFormat |= COM_BYTE_7_BITS_DATA;
            break;
        default:
            pSerialConf->uchComByteFormat |= COM_BYTE_8_BITS_DATA;
            break;
        }


        if (GetParameter(   atchName, 
                            SCF_DATANAME_PARITY, 
                            &dwDataType, 
                            (LPVOID)&dwData, 
                            sizeof(dwData), 
                            &dwDummy) != SCF_SUCCESS)
        {
            return FALSE;
        }

        switch (dwData)
        {
        case  SCF_CAPS_PARITY_EVEN:
            pSerialConf->uchComByteFormat |= COM_BYTE_EVEN_PARITY;
            break;
        case  SCF_CAPS_PARITY_ODD:
            pSerialConf->uchComByteFormat |= COM_BYTE_ODD_PARITY;
            break;
        case  SCF_CAPS_PARITY_NONE:
        case  SCF_CAPS_PARITY_MARK:
        case  SCF_CAPS_PARITY_SPACE:
        default:
            break;
        }


        if (GetParameter(   atchName, 
                            SCF_DATANAME_STOPBIT, 
                            &dwDataType, 
                            (LPVOID)&dwData, 
                            sizeof(dwData), 
                            &dwDummy) != SCF_SUCCESS)
        {
            return FALSE;
        }

        switch (dwData)
        {
        case SCF_CAPS_STOP_2:
            pSerialConf->uchComByteFormat |= COM_BYTE_2_STOP_BITS;
            break;
        case SCF_CAPS_STOP_1:
        default:
            break;
        }
    }
    return TRUE;
}

