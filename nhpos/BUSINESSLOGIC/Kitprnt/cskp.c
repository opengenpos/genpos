/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*========================================================================*\
*   Title              : Client/Server KITCHEN PRINTER module, Program List
*   Category           : Client/Server KITCHEN PRINTER module, NCR2170 US HOSPITAL FOR US MODEL
*   Program Name       : CSKP.C
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as follows.
*
*       VOID    KpsInit         * Initiarize for kitchen print *
*       SHORT   KpsPrint        * print kitchen printer *
*       SHORT   KpsUnlock       * Clear Terminal Lock memory *
*       USHORT  KpsConvertError * Convert Error Code to leadthru number *
*       SHORT   KpsCheckPrint   * Check if printer is printing or not. *
*       SHORT   KpsCreateFile   * Create buffer file
*       SHORT   KpsDeleteFile   * Delete buffer file
*
*  ------------------------------------------------------------------------
*    Update Histories   :
*
*   Date        Ver.Rev  :NAME         :Description
*   May-06-92 : 00.00.01 :M.YAMAMOTO   :Initial
*   Feb-08-93 : 01.00.11 :M.YAMAMOTO   :Add Protocol Converter.
*   Mar-31-93 : 01.00.12 :M.YAMAMOTO   :Correct QA Comment (issue W Kit-Receipt)
*   Apr-07-94 : 00.00.04 :K.YOU        :add flex GC feature.(add. KpsCreateFile,
*                                      :KpsDeleteFile)(mod. KpsPrint, KpsInit)
*   Mar-30-95 : 02.05.04 :M.Suzuki     :add uchKpsPrinterType R3.0
*   Nov-06-95 : 03.01.00 :T.Nakahata   :R3.1 Initial
*
** NCR2171 **
*   Aug-26-99 : 01.00.00 :M.Teraki     :initial (for 2171)
*   Dec-15-99 : 01.00.00 :hrkato       :Saratoga
*   May-01-00 :          :M.Teraki     :Change count of KPS Buffer 2->16
*   May-08-00 :          :M.Teraki     :changed symbol from pSysconfig->ausOption[0]
*             :          :             : to usBMOption.
*   Jul-31-00 : V1.0.0.3 :M.Teraki     :Added Processing queue.
*                                      : (to support multi-frame request)
*   Aug-04-00 : V1.0.0.4 :M.Teraki     :Added code to stop timer on Server module.
*             :          :             : (to support multi-frame request)
*   Jan-10-11 : V1.0.0.10:M.Ozawa      :Correct KpsDeleteFile() to prevent another file close at KpsCreateFile()
*
** GenPOS **
*   Apr-24-15 : 02.02.01 :R.Chambers   :Correct KpsInit() initialization, include csstbstb.h fix compiler warning.
*   Jan-11-18 : 02.02.02 :R.Chambers   :implementing Alt PLU Mnemonic in Kitchen Printing.
*   Dec-31-23 : 02.04.00 : R.Chambers  :moved global g_uchPortStatus from this file to cskpin.c.
\*=======================================================================*/
#include	<tchar.h>
#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h>

#include    "ecr.h"
#include    "pif.h"
#include    "paraequ.h"
#include    "para.h"
#include    "rfl.h"
#include    "csstbpar.h"
#include    "cskp.h"
#include    "appllog.h"
#include    "shr.h"
#include    "cskpin.h"
#include    "csstbkps.h"
#include    "csstbstb.h"
#include    "uie.h"
#include    "pifmain.h"

/******* Static memory *********/

static TCHAR FARCONST aszKPSBuffFile1[] = _T("KPSBUFF1");     /* file Name of Kps buffer 1 */
static TCHAR FARCONST aszKPSBuffFile2[] = _T("KPSBUFF2");     /* file Name of Kps buffer 2 */
static TCHAR FARCONST aszKPSBuffFile3[] = _T("KPSBUFF3");     /* file Name of Kps buffer 3 */
static TCHAR FARCONST aszKPSBuffFile4[] = _T("KPSBUFF4");     /* file Name of Kps buffer 4 */
static TCHAR FARCONST aszKPSBuffFile5[] = _T("KPSBUFF5");     /* file Name of Kps buffer 5 */
static TCHAR FARCONST aszKPSBuffFile6[] = _T("KPSBUFF6");     /* file Name of Kps buffer 6 */
static TCHAR FARCONST aszKPSBuffFile7[] = _T("KPSBUFF7");     /* file Name of Kps buffer 7 */
static TCHAR FARCONST aszKPSBuffFile8[] = _T("KPSBUFF8");     /* file Name of Kps buffer 8 */
static TCHAR FARCONST aszKPSBuffFile9[] = _T("KPSBUFF9");     /* file Name of Kps buffer 9 */
static TCHAR FARCONST aszKPSBuffFile10[] = _T("KPSBUFF10");   /* file Name of Kps buffer 10 */
static TCHAR FARCONST aszKPSBuffFile11[] = _T("KPSBUFF11");   /* file Name of Kps buffer 11 */
static TCHAR FARCONST aszKPSBuffFile12[] = _T("KPSBUFF12");   /* file Name of Kps buffer 12 */
static TCHAR FARCONST aszKPSBuffFile13[] = _T("KPSBUFF13");   /* file Name of Kps buffer 13 */
static TCHAR FARCONST aszKPSBuffFile14[] = _T("KPSBUFF14");   /* file Name of Kps buffer 14 */
static TCHAR FARCONST aszKPSBuffFile15[] = _T("KPSBUFF15");   /* file Name of Kps buffer 15 */
static TCHAR FARCONST aszKPSBuffFile16[] = _T("KPSBUFF16");   /* file Name of Kps buffer 16 */

       USHORT        husKpsQueue;
       PifSemHandle  husKpsQueueSem = PIF_SEM_INVALID_HANDLE;
static PifSemHandle  husKpsMainSem = PIF_SEM_INVALID_HANDLE;
static PifSemHandle  husKpsNoAsMtSem = PIF_SEM_INVALID_HANDLE;

static KPSBUFFER KpsBuffer1;
static KPSBUFFER KpsBuffer2;
static KPSBUFFER KpsBuffer3;
static KPSBUFFER KpsBuffer4;
static KPSBUFFER KpsBuffer5;
static KPSBUFFER KpsBuffer6;
static KPSBUFFER KpsBuffer7;
static KPSBUFFER KpsBuffer8;
static KPSBUFFER KpsBuffer9;
static KPSBUFFER KpsBuffer10;
static KPSBUFFER KpsBuffer11;
static KPSBUFFER KpsBuffer12;
static KPSBUFFER KpsBuffer13;
static KPSBUFFER KpsBuffer14;
static KPSBUFFER KpsBuffer15;
static KPSBUFFER KpsBuffer16;

/*UCHAR   uchKpsPrinterStatus;*/                        /* printer status table */

UCHAR   g_uchKpsTerminalLock;                           /* terminal check During excuting Terminal No. (uniqu address) */
SHORT   g_hasKpsPort[KPS_NUMBER_OF_PRINTER];            /* Port handle save area,       Modify at R3.1 */

/* SHORT   hasKpsPort[4];                                  Port handle save area */
USHORT  usKpsTimerValue;                                /* Timer value */
UCHAR   g_auchKpsSysConfig[KPS_NUMBER_OF_PRINTER];      /* Port ID save area,   Modify at R3.1 */
/* UCHAR   auchKpsSysConfig[PIF_LEN_PORT];                 port assign */

static UCHAR  uchKpsSeqNo[KPS_NUMBER_OF_PRINTER];       /* sequence No of communication */
       UCHAR  uchKpsPrintDown;                          /* for power down recovery */
/*UCHAR   uchKpsPrinterType;*/                          /* 0:7193  1:EPSON  Add R3.0 *//* ### DEL 2172 rel1.0 */

UCHAR   uchKpsPrinting;                                 /* Print thread acting */

UCHAR   uchKpsSemEvent;                                 /* Semaphore event flag */
UCHAR   uchKpsQueEvent;                                 /* KpsQue event flag */

       KPSQUE KpsEmptyQue;                              /* buffer empty que */
       KPSQUE KpsProcQue;                               /* buffer process que */
       KPSQUE KpsRcvQue;                                /* buffer recovery que */
static KPSQUE KpsSpoolingQue[KPS_NUMBER_OF_PRINTER];    /* spooling buffer que *//* A (V1.0.0.3) */

UCHAR  auchKpsDownPrinter[KPS_NUMBER_OF_PRINTER];       /* save id of down printer */
UCHAR  uchKpsFlag;                                      /* Stop recovery process when */

static USHORT ausKpsConsNo[KPS_TERMINAL_OF_CLUSTER];           /* Storage for Cons. No by Terminal */
static USHORT  ausKpsOutPrtInfo[KPS_TERMINAL_OF_CLUSTER];  /* PrinterInfo(AltFlag + Prt No) *//* ### ADD Saratoga (041900) */


/* static VOID (THREADENTRY *pFunc1)(VOID) = KpsPrintProcess;    / Add R3.0 */
/* static VOID (THREADENTRY *pFunc2)(VOID) = KpsTimerProcess;    / Add R3.0 */

static SHORT KpsIsWaitingEndFrame(VOID);

/*
*==========================================================================
**    Synopsis:   VOID    KpsInit(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*               Initialize for Kitchen printer.
*==========================================================================
*/
VOID    KpsInit(VOID)
{
    SYSCONFIG CONST   *pSysconfig = PifSysConfig();                /* get system configration */
    UCHAR             uchPort;

    /* Check System Configuration */
    /* ===== Increase No. of Remote Printer (Release 3.1) BEGIN ===== */

    for (uchPort = 0; uchPort < KPS_NUMBER_OF_PRINTER; uchPort++) {
		g_auchKpsSysConfig[uchPort] = (UCHAR)(~DEVICE_KITCHEN_PRINTER);
		g_hasKpsPort[uchPort] = KPS_NOT_OPEN;
	}
    /* ===== Increase No. of Remote Printer (Release 3.1) END ===== */

    /* Check protocol converter port */
    /* ### MOD 2172 Saratoga (041800) */
    for(uchPort = 1;uchPort < PIF_LEN_PORT;uchPort++){
        if (pSysconfig->auchComPort[uchPort] == DEVICE_KITCHEN_PRINTER) {
               g_auchKpsSysConfig[uchPort - 1] = uchPort;
        }
    }

    g_uchKpsTerminalLock = 0;            /* clear terminal lock */

    /* clear all sequence number */
    for (uchPort = 0; uchPort < KPS_NUMBER_OF_PRINTER; uchPort++) {
        uchKpsSeqNo[uchPort] = 0;
    }
    memset(ausKpsConsNo, 0, KPS_TERMINAL_OF_CLUSTER);
    memset(ausKpsOutPrtInfo, 0, KPS_TERMINAL_OF_CLUSTER); /* ### ADD Saratoga (041900) */

    /* get semaphore */
    husKpsMainSem  = PifCreateSem(1);
    husKpsQueueSem = PifCreateSem(1);
    husKpsQueue    = PifCreateQueue(16);
    husKpsNoAsMtSem = PifCreateSem(1);

    uchKpsFlag = 0;      /* Reset STOP_RECOVERY and POWERUP_OK */

    /* make que of buffer */
    /* after inititalize, all KpsBuffer is linked from Empty Queue */
    KpsEmptyQue.pFirstQue = &KpsBuffer1;
    KpsEmptyQue.pLastQue  = &KpsBuffer16;
    KpsBuffer1.pNextQue  = &KpsBuffer2;
    KpsBuffer2.pNextQue  = &KpsBuffer3;
    KpsBuffer3.pNextQue  = &KpsBuffer4;
    KpsBuffer4.pNextQue  = &KpsBuffer5;
    KpsBuffer5.pNextQue  = &KpsBuffer6;
    KpsBuffer6.pNextQue  = &KpsBuffer7;
    KpsBuffer7.pNextQue  = &KpsBuffer8;
    KpsBuffer8.pNextQue  = &KpsBuffer9;
    KpsBuffer9.pNextQue  = &KpsBuffer10;
    KpsBuffer10.pNextQue = &KpsBuffer11;
    KpsBuffer11.pNextQue = &KpsBuffer12;
    KpsBuffer12.pNextQue = &KpsBuffer13;
    KpsBuffer13.pNextQue = &KpsBuffer14;
    KpsBuffer14.pNextQue = &KpsBuffer15;
    KpsBuffer15.pNextQue = &KpsBuffer16;
    KpsBuffer16.pNextQue = NULL;
    KpsProcQue.pFirstQue = NULL;
    KpsProcQue.pLastQue = NULL;
    KpsRcvQue.pFirstQue = NULL;
    KpsRcvQue.pLastQue  = NULL;
    for (uchPort = 0; uchPort < KPS_NUMBER_OF_PRINTER; uchPort++) {              /* A START (V1.0.0.3) */
        KpsSpoolingQue[uchPort].pFirstQue = KpsSpoolingQue[uchPort].pLastQue = NULL;
    }                                                                   /* A END (V1.0.0.3) */

    /* ### ADD 2172 Rel10. (Saratoga) */
    KpsBuffer1.usOutPrinterInfo  = 0;
    KpsBuffer2.usOutPrinterInfo  = 0;
    KpsBuffer3.usOutPrinterInfo  = 0;
    KpsBuffer4.usOutPrinterInfo  = 0;
    KpsBuffer5.usOutPrinterInfo  = 0;
    KpsBuffer6.usOutPrinterInfo  = 0;
    KpsBuffer7.usOutPrinterInfo  = 0;
    KpsBuffer8.usOutPrinterInfo  = 0;
    KpsBuffer9.usOutPrinterInfo  = 0;
    KpsBuffer10.usOutPrinterInfo = 0;
    KpsBuffer11.usOutPrinterInfo = 0;
    KpsBuffer12.usOutPrinterInfo = 0;
    KpsBuffer13.usOutPrinterInfo = 0;
    KpsBuffer14.usOutPrinterInfo = 0;
    KpsBuffer15.usOutPrinterInfo = 0;
    KpsBuffer16.usOutPrinterInfo = 0;
    
	/* correct the order of start thread, after the open files */
    if ( 1 != usBMOption ) {    /* saratoga */
        KpsMoniter(1);
    }
    /* Set Timer Value */
    usKpsTimerValue = KPS_RECOVER_TIMER_VALUE;

    /* open kitchen buffer file */
    KpsBuffer1.hsKpsBuffFH = PifOpenFile(aszKPSBuffFile1, auchTEMP_OLD_FILE_READ_WRITE);
    KpsBuffer2.hsKpsBuffFH = PifOpenFile(aszKPSBuffFile2, auchTEMP_OLD_FILE_READ_WRITE);
    KpsBuffer3.hsKpsBuffFH = PifOpenFile(aszKPSBuffFile3, auchTEMP_OLD_FILE_READ_WRITE);
    KpsBuffer4.hsKpsBuffFH = PifOpenFile(aszKPSBuffFile4, auchTEMP_OLD_FILE_READ_WRITE);
    KpsBuffer5.hsKpsBuffFH = PifOpenFile(aszKPSBuffFile5, auchTEMP_OLD_FILE_READ_WRITE);
    KpsBuffer6.hsKpsBuffFH = PifOpenFile(aszKPSBuffFile6, auchTEMP_OLD_FILE_READ_WRITE);
    KpsBuffer7.hsKpsBuffFH = PifOpenFile(aszKPSBuffFile7, auchTEMP_OLD_FILE_READ_WRITE);
    KpsBuffer8.hsKpsBuffFH = PifOpenFile(aszKPSBuffFile8, auchTEMP_OLD_FILE_READ_WRITE);
    KpsBuffer9.hsKpsBuffFH = PifOpenFile(aszKPSBuffFile9, auchTEMP_OLD_FILE_READ_WRITE);
    KpsBuffer10.hsKpsBuffFH = PifOpenFile(aszKPSBuffFile10, auchTEMP_OLD_FILE_READ_WRITE);
    KpsBuffer11.hsKpsBuffFH = PifOpenFile(aszKPSBuffFile11, auchTEMP_OLD_FILE_READ_WRITE);
    KpsBuffer12.hsKpsBuffFH = PifOpenFile(aszKPSBuffFile12, auchTEMP_OLD_FILE_READ_WRITE);
    KpsBuffer13.hsKpsBuffFH = PifOpenFile(aszKPSBuffFile13, auchTEMP_OLD_FILE_READ_WRITE);
    KpsBuffer14.hsKpsBuffFH = PifOpenFile(aszKPSBuffFile14, auchTEMP_OLD_FILE_READ_WRITE);
    KpsBuffer15.hsKpsBuffFH = PifOpenFile(aszKPSBuffFile15, auchTEMP_OLD_FILE_READ_WRITE);
    KpsBuffer16.hsKpsBuffFH = PifOpenFile(aszKPSBuffFile16, auchTEMP_OLD_FILE_READ_WRITE);
}

/*
*==========================================================================
**    Synopsis:   SHORT    KpsPrint(UCHAR *auchRcvBuffer, USHORT usRcvBufLen, 
                UCHAR uchUniqueAddress, USHORT *usPrintStatus,USHORT usOutPrinterInfo)
*
*       Input:    UCHAR  auchRcvBuffer
*                 USHORT usRcvBufLen
*                 UCHAR  uchUniqueAddress
*                               -   Terminal # tendered
*               ### ADD 2172 Rel1.0(Saratoga)
*                 USHORT usOutPrinterInfo
*                               -    Target Kitchen Printer Number & Alt flag  
*                                   PrinterNo : 1 <= 0x00?? <= MAX_DEST_SIZE (==8)
*                                   AltFlag   : 0x80??(Manual)/0x40??(Auto)/0x00??(normal)
*                 
*      Output:    USHORT *usPrintStatus
*       InOut:    Nothing
*
**  Return   :    KPS_PRINTER_BUSY
*                 KPS_BUFFER_FULL
*                 KPS_TIMER_ON
*                 KPS_PERFORM
*
**  Description:
*               Print to  Kitchen printer.
*==========================================================================
*/
SHORT KpsPrint(UCHAR *auchRcvBuffer, USHORT usRcvBufLen, UCHAR uchUniqueAddress,
               USHORT *pusPrintStatus, USHORT usOutPrinterInfo)
{
    TCHAR       auchConsNo[KPS_CONS_SIZE + 1];      /* Storage for Cons No from Trailer */
    USHORT      usProcessId;
    USHORT      usConsNo;           /* Converts a character from Trailer to an ushort value */
                                    /* 1 to 9999 */
    ULONG       ulMaxSpoolSize;
    ULONG       ulActPos;
    KPS_1_FRAME *pKps_1_Frame;      /* for Cast */

    SHORT       nStatus;
    KPSQUE      *pSpoolQue = &(KpsSpoolingQue[(usOutPrinterInfo & 0x00ff) - 1]);
    UCHAR       *puchSeqNo = &(uchKpsSeqNo[(usOutPrinterInfo & 0x00ff) - 1]);

    /* nonzero indicates last frame was performed with auto alternation */
    static UCHAR  auchLastFrameAutoAltered[KPS_NUMBER_OF_PRINTER] = {0, 0, 0, 0, 0, 0, 0, 0};
    UCHAR *puchLastFrameAutoAltered = &(auchLastFrameAutoAltered[(usOutPrinterInfo & 0x00ff) - 1]);

    /* disable unlock */
    PifRequestSem(husKpsMainSem);
    pKps_1_Frame = (KPS_1_FRAME *)auchRcvBuffer;

    /* If KPS on terminal 'unique addr.' is locked, return to caller */
    if (KpsLockCheck(uchUniqueAddress) == KPS_LOCK) {
        PifReleaseSem(husKpsMainSem);
        return(KPS_PRINTER_BUSY);
    }

    /* At first frame, initialize auto altered flag */
    if ((pKps_1_Frame->COM_ID/*auchRcvBuffer[KPS_COM_FRAME]*/ & KPS_FIRST_FRAME) != 0x00) {
        *puchLastFrameAutoAltered = 0;
    }
    else {
        /* At continued frame */
        /* If Auto Alternate flag is not specified, return with value 'PRINTER_DOWN' */
        if ((*puchLastFrameAutoAltered != 0) &&
            ((usOutPrinterInfo & 0x4000) == 0)) {
            PifReleaseSem(husKpsMainSem);
            return KPS_PRINTER_DOWN;
        }
    }

    /* passed data is first frame */
    if ((pKps_1_Frame->COM_ID/*auchRcvBuffer[KPS_COM_FRAME]*/ & KPS_FIRST_FRAME) != 0x00) {

        /* if empty queue is not exist, return to caller */
        if (KpsEmptyQue.pFirstQue == NULL) {

            /* if all buffer is linked to recovery queue, return Buffer-full status.. */
            if (KpsProcQue.pFirstQue == NULL) {               /* all buffer is recovering */
                *pusPrintStatus = 0;        
                PifReleaseSem(husKpsMainSem);
                return(KPS_BUFFER_FULL);        /* take to kitchen */
            }

            /* if all buffer is linked to processing queue, return Busy status */
            PifReleaseSem(husKpsMainSem);
            return(KPS_PRINTER_BUSY);
        }

        /* initialize consecutive number */
        _tcsncpy(auchConsNo, pKps_1_Frame->Trailer.uchCons, KPS_CONS_SIZE);
        auchConsNo[KPS_CONS_SIZE] = '\0';  /* set null-terminator for atoi() */
        usConsNo = (USHORT)_ttoi(auchConsNo);

        if (ausKpsConsNo[uchUniqueAddress - 1] == usConsNo) {
            if(ausKpsOutPrtInfo[uchUniqueAddress - 1] == usOutPrinterInfo){
                PifLog(MODULE_KPS, LOG_ERROR_KPS_CODE_01);
                PifLog(MODULE_KPS, LOG_ERROR_KPS_SEQUENCE_ERROR);
                *puchSeqNo = 0; /* reset sequence number */
                g_uchKpsTerminalLock = 0;
                if (pKps_1_Frame->COM_ID/*auchRcvBuffer[KPS_COM_FRAME]*/ & KPS_END_FRAME) {
                    PifReleaseSem(husKpsMainSem);
                    return KPS_PERFORM;               /* Receive same Sequence No at End Frame */
                } else {
                    PifReleaseSem(husKpsMainSem);
                    return KPS_TIMER_ON;              /* Receive same Sequence No */
                }
            }
            else
                ausKpsOutPrtInfo[uchUniqueAddress - 1] = usOutPrinterInfo;
        }
        else {
            /* new consecutive number is passed from caller. update internal table */
            ausKpsConsNo[uchUniqueAddress - 1] = usConsNo;
            ausKpsOutPrtInfo[uchUniqueAddress - 1] = usOutPrinterInfo;
        }

        /* move KpsBuffer from Empty queue and link to each spool queue */
        KpsQueTransfer(&KpsEmptyQue, pSpoolQue);

        /* if Terminal is not locked,  */
        if (!g_uchKpsTerminalLock) {     /* usTerminallock == 0 */

            /* Set printer number */
            pSpoolQue->pFirstQue->auchOutputPrinter[0] = 1;
            pSpoolQue->pFirstQue->auchOutputPrinter[1] = 2;
            pSpoolQue->pFirstQue->auchOutputPrinter[2] = 3;
            pSpoolQue->pFirstQue->auchOutputPrinter[3] = 4;
            pSpoolQue->pFirstQue->auchOutputPrinter[4] = 5;
            pSpoolQue->pFirstQue->auchOutputPrinter[5] = 6;
            pSpoolQue->pFirstQue->auchOutputPrinter[6] = 7;
            pSpoolQue->pFirstQue->auchOutputPrinter[7] = 8;
            
            pSpoolQue->pFirstQue->ulWritePoint = 0;
        }
    }

    /* passed data is continued frame */
    else {

        /* if spooling queue has no KPSBUFFER, assume last frame was not accepted */
        if (pSpoolQue->pFirstQue == NULL) {
            PifReleaseSem(husKpsMainSem);
            return KPS_PRINTER_DOWN;
        }

        /* if sequence number is not valid, return to caller */
        if (pKps_1_Frame->COM_SEQ_NO/*auchRcvBuffer[KPS_SEQ_NO]*/ != (UCHAR)((*puchSeqNo) + 1)) {
            PifLog(MODULE_KPS, LOG_ERROR_KPS_CODE_02);
            PifLog(MODULE_KPS, LOG_ERROR_KPS_SEQUENCE_ERROR);
            *puchSeqNo = 0;   /* reset sequence number */
            g_uchKpsTerminalLock = 0;
            if (pKps_1_Frame->COM_ID/*auchRcvBuffer[KPS_COM_FRAME]*/ & KPS_END_FRAME) {
                PifReleaseSem(husKpsMainSem);
                return KPS_PERFORM;                 /* Receive same Sequence No at End Frame */
            } else {
                PifReleaseSem(husKpsMainSem);
                return KPS_TIMER_ON;                /* Receive same Sequence No */
            }
        }
        if (!g_uchKpsTerminalLock) {
            PifLog(MODULE_KPS, LOG_ERROR_KPS_CODE_03);
            PifLog(MODULE_KPS, LOG_ERROR_KPS_SEQUENCE_ERROR);
            *pusPrintStatus = 0;        
            *puchSeqNo = 0;   /* reset sequence number */
            g_uchKpsTerminalLock = 0;
            PifReleaseSem(husKpsMainSem);
            return(KPS_BUFFER_FULL);                /* It is fatal error */
        }
    }

    /* set output printer number & alt flag to spooling buffer */
    pSpoolQue->pFirstQue->usOutPrinterInfo = usOutPrinterInfo;
    
    *puchSeqNo = (UCHAR)pKps_1_Frame->COM_SEQ_NO;//auchRcvBuffer[KPS_SEQ_NO];              /* set sequence No */
    g_uchKpsTerminalLock = uchUniqueAddress;        /* lock KP manager on output terminal */

    /* calculate max. size of kitchen spool file */
    ulMaxSpoolSize = RflGetMaxRecordNumberByType(FLEX_ITEMSTORAGE_ADR) * KPS_SIZE_FOR_1ITEM - (KPS_DATA + KPS_BUFFER_HEAD) - KPS_DATE_TIME;
	NHPOS_ASSERT(ulMaxSpoolSize < 0xffff);

    /* if buffer storage is not enough, ignore request and return to caller */
    if (usRcvBufLen > (ulMaxSpoolSize - pSpoolQue->pFirstQue->ulWritePoint)) {
        PifLog(MODULE_KPS, LOG_ERROR_KPS_SPOOL_OVER);
        *pusPrintStatus = 0;        
        *puchSeqNo = 0; /* reset sequence number */
        g_uchKpsTerminalLock = 0;
        SerKpsTimerStopAll();          /* Stop timer started by KPS_TIMER_ON. (V1.0.0.4) */

        /* initialize KPS buffer and bring back to empty queue */
        pSpoolQue->pFirstQue->ulWritePoint = 0;
        KpsQueTransfer(pSpoolQue, &KpsEmptyQue);

        PifReleaseSem(husKpsMainSem);
        return(KPS_BUFFER_FULL);
    }

    /* write requested print data to spool file */
    if (PifSeekFile(pSpoolQue->pFirstQue->hsKpsBuffFH, pSpoolQue->pFirstQue->ulWritePoint, &ulActPos) != PIF_OK) {
        PifAbort(MODULE_KPS, FAULT_ERROR_FILE_SEEK);
    }
    PifWriteFile(pSpoolQue->pFirstQue->hsKpsBuffFH, &(pKps_1_Frame->uchOperatorName), (USHORT)(usRcvBufLen - KPS_DATA_HEAD));

    pSpoolQue->pFirstQue->ulWritePoint += usRcvBufLen - (KPS_DATA_HEAD);

    /* if requested frame is NOT last frame, return to caller */
    if ((pKps_1_Frame->COM_ID/*auchRcvBuffer[KPS_COM_FRAME]*/ & KPS_LAST_FRAME) == 0x00) {

        /* if desired printer is down, ignore request and clear current buffer */
        KpsMakeOutPutPrinterEx(pSpoolQue->pFirstQue, pusPrintStatus);
        if (*(UCHAR *)pusPrintStatus == 0) {
            pSpoolQue->pFirstQue->ulWritePoint = 0;
            KpsQueTransfer(pSpoolQue, &KpsEmptyQue);
            nStatus = KPS_PRINTER_DOWN;
        }
        else {
            nStatus = KPS_TIMER_ON;

            /* update 'last frame automatically altered' flag */
            if ((usOutPrinterInfo & 0x4000) != 0) {
                *puchLastFrameAutoAltered = 1;
            }
        }

        PifReleaseSem(husKpsMainSem);
        return nStatus;
    }

    /* following code runs only when last frame requested */

    /* initialize read point */
    pSpoolQue->pFirstQue->ulReadPoint = 0;
    usProcessId = KPS_PRINT_MODULE;

    /* make output status */
    nStatus = KPS_PERFORM;

    /* check printer status */
    KpsMakeOutPutPrinterEx(pSpoolQue->pFirstQue, pusPrintStatus);

    /* if printer is available, set KPS buffer to process queue */
    if (*(UCHAR *)pusPrintStatus != 0) {   /* desired printer is alive */
        KpsQueTransfer(pSpoolQue, &KpsProcQue);
        PifWriteQueue(husKpsQueue, &usProcessId);   /* notify trigger to print thread */
    }

    /* if desired printer is down, ignore request and clear current buffer */
    else {
        /* initialize KPS buffer and bring back to empty queue */
        pSpoolQue->pFirstQue->ulWritePoint = 0;
        KpsQueTransfer(pSpoolQue, &KpsEmptyQue);
		nStatus = auchKpsDownPrinter[usOutPrinterInfo]; //SR456
    }

    *puchSeqNo = 0;          /* reset sequence number */

    /* if all spooling buffer is transfered to process queue, unlock terminal */
    if (KpsIsWaitingEndFrame() == 0) {
        g_uchKpsTerminalLock = 0;   /* unlock terminal */
        SerKpsTimerStopAll();       /* Stop timer started by KPS_TIMER_ON. (V1.0.0.4) */
    }

    PifReleaseSem(husKpsMainSem);

    return  nStatus;
}

/*
*==========================================================================
**    Synopsis:   VOID    KpsUnlock(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*               Unlock Terminal lock memory.
*               Server module calls this.
*==========================================================================
*/
VOID    KpsUnlock(VOID)
{
    int     nKpIdx; /* loop counter */                                               /* V1.0.0.3 */
    KPSQUE *pSpoolQue; /* spool que */                                               /* V1.0.0.3 */

    PifRequestSem(husKpsMainSem);
    if (g_uchKpsTerminalLock) {    /* terminal lock memory = 0 */
        g_uchKpsTerminalLock = 0;

        /* for each KP, remove buffer on all spooling queue */
        for (nKpIdx = 0; nKpIdx < KPS_NUMBER_OF_PRINTER; nKpIdx++) {
            pSpoolQue = &(KpsSpoolingQue[nKpIdx]);

            while (pSpoolQue->pFirstQue != NULL) {
                pSpoolQue->pFirstQue->ulWritePoint = 0;
                KpsQueTransfer(pSpoolQue, &KpsEmptyQue);
            }
        }
    }
    PifReleaseSem(husKpsMainSem);
    return;    
}
/*
*===========================================================================
** Synopsis:    USHORT     KpsConvertError(SHORT sError);
*     Input:    sError 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Leadthru No 
*
** Description: This function converts error code to leadthru number.
*===========================================================================
*/
USHORT   KpsConvertError(SHORT sError)
{
    USHORT  usLeadthruNo;

    switch ( sError ) {
    case  KPS_PERFORM :    /* Success  */
        usLeadthruNo = 0; 
        break;

    case  KPS_SHARED_DOWN :     /* Error 16 */
    case  KPS_PRINTER_DOWN :    /* Error -4 *//* ### ADD 2172 Rel1.0 (Saratoga) */
    case  KPS_PRINTER_BUSY :    /* Error -1 */
    case  KPS_BUFFER_FULL :     /* Error -2 */
        usLeadthruNo = LDT_KTNERR_ADR  ;   /*   3 * Kitchen Printer Error */
        break;

    case  KPS_DUR_INQUIRY :
    case  KPS_M_DOWN_ERROR:
        usLeadthruNo = LDT_LNKDWN_ADR  ;   /*   3 * Kitchen Printer Error */
        break;

    case  KPS_BM_DOWN_ERROR:
        usLeadthruNo = LDT_SYSBSY_ADR  ;   /*   3 * Kitchen Printer Error */
        break;

	case  KPS_TIME_OUT_ERROR:
		usLeadthruNo = LDT_REQST_TIMED_OUT;
		break;

	case  KPS_BUSY_ERROR:
		usLeadthruNo = LDT_REQST_TIMED_OUT;
		break;

	case  KPS_UNMATCH_TRNO :
		usLeadthruNo = LDT_COMUNI_ADR;   // Transaction number did not match expected number
		break;

      default:  
        usLeadthruNo = LDT_ERR_ADR   ;     /*  21 * Error(Global) */
        break;
    }

	if (sError != KPS_PERFORM) {
		PifLog (MODULE_KPS, LOG_ERROR_KPS_CONVERTERROR);
		PifLog (MODULE_ERROR_NO(MODULE_KPS), (USHORT)abs(sError));
		PifLog (MODULE_ERROR_NO(MODULE_KPS), usLeadthruNo);
	}

    return (usLeadthruNo);           
}
/*
*==========================================================================
**    Synopsis:   SHORT    KpsCheckPrint(VOID)
*
**  Return   :    KPS_PERFORM       -    Printer is not printing.
*                 KPS_PRINTER_BUSY  -    Printer is printing.
*
**  Description:
*           This Function check if Kitchen Printer is printing or not.
*==========================================================================
*/
SHORT    KpsCheckPrint(VOID)
{
    if (KpsProcQue.pFirstQue == NULL) {               /* Check if during Process */
        uchKpsFlag |= KPS_STOP_RECOVERY;
        return KPS_PERFORM;
    }
    return KPS_PRINTER_BUSY;
}

/*
*==========================================================================
**    Synopsis: SHORT   KpsCreateFile(USHORT usSize)
*                                              
*       Input:  USHORT  usSize  - number of item
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  KPS_PERFORM - performed creation file
*               KPS_ERROR   - not performed creation file 
*                                                                 
**  Description:
*           Create Kps buffer file 1,2.... 16.
*==========================================================================
*/
SHORT   KpsCreateFile(USHORT usSize)
{
    KPSBUFFER *pKpsBuf;     /* pointer to kps buffer management table */
    TCHAR     *uchFileName; /* filename to be opened */
    ULONG      ulActSize;   /* actual file size */
    ULONG      ulActMove;   /* actual seek distance */
    SHORT      i;           /* loop index */
    SHORT      sReturn;     /* return value from pif functions */
    SHORT      hsFileHandle; /* file handle */

    
    /* check kitchen buffer status */
    if ((sReturn = KpsCheckPrint()) != KPS_PERFORM) {
        return(LDT_KPS_BUSY);
    }

    /* calculate file size */
    ulActSize = usSize * KPS_SIZE_FOR_1ITEM;

    for (i = 0; i < 16; i++) {
        /* open Kps Buffer file */
        switch (i) {
        case 0:
            pKpsBuf = &KpsBuffer1;
            uchFileName = aszKPSBuffFile1;
            break;
        case 1:
            pKpsBuf = &KpsBuffer2;
            uchFileName = aszKPSBuffFile2;
            break;
        case 2:
            pKpsBuf = &KpsBuffer3;
            uchFileName = aszKPSBuffFile3;
            break;
        case 3:
            pKpsBuf = &KpsBuffer4;
            uchFileName = aszKPSBuffFile4;
            break;
        case 4:
            pKpsBuf = &KpsBuffer5;
            uchFileName = aszKPSBuffFile5;
            break;
        case 5:
            pKpsBuf = &KpsBuffer6;
            uchFileName = aszKPSBuffFile6;
            break;
        case 6:
            pKpsBuf = &KpsBuffer7;
            uchFileName = aszKPSBuffFile7;
            break;
        case 7:
            pKpsBuf = &KpsBuffer8;
            uchFileName = aszKPSBuffFile8;
            break;
        case 8:
            pKpsBuf = &KpsBuffer9;
            uchFileName = aszKPSBuffFile9;
            break;
        case 9:
            pKpsBuf = &KpsBuffer10;
            uchFileName = aszKPSBuffFile10;
            break;
        case 10:
            pKpsBuf = &KpsBuffer11;
            uchFileName = aszKPSBuffFile11;
            break;
        case 11:
            pKpsBuf = &KpsBuffer12;
            uchFileName = aszKPSBuffFile12;
            break;
        case 12:
            pKpsBuf = &KpsBuffer13;
            uchFileName = aszKPSBuffFile13;
            break;
        case 13:
            pKpsBuf = &KpsBuffer14;
            uchFileName = aszKPSBuffFile14;
            break;
        case 14:
            pKpsBuf = &KpsBuffer15;
            uchFileName = aszKPSBuffFile15;
            break;
        case 15:
            pKpsBuf = &KpsBuffer16;
            uchFileName = aszKPSBuffFile16;
            break;
        default:
            // should never hit this however the compiler is warning so let's
            // allow it to relax.
            return KPS_ERROR;
            break;
        }

        /* open file. */
        hsFileHandle = PifOpenFile(uchFileName, auchTEMP_NEW_FILE_READ_WRITE);
        if (hsFileHandle == PIF_ERROR_FILE_EXIST) {
            PifCloseFile(pKpsBuf->hsKpsBuffFH);
            PifDeleteFile(uchFileName);
            hsFileHandle = PifOpenFile(uchFileName, auchTEMP_NEW_FILE_READ_WRITE);
        }

		{
			char  xBuff[128];
			sprintf (xBuff, "**NOTE: KpsCreateFile() KpsBuffer %d hsFileHandle = %d", i, hsFileHandle);
			NHPOS_ASSERT_TEXT((0 <= hsFileHandle), xBuff);
		}

        if (hsFileHandle < 0) {
            pKpsBuf->hsKpsBuffFH = -1;  // assign bad file handle indicating file not open.
            return KPS_ERROR;
        }

        /* verify file size and check error */
        sReturn = PifSeekFile(hsFileHandle, ulActSize, &ulActMove);
        if ((sReturn < 0) || (ulActMove != ulActSize)) {
            PifCloseFile(hsFileHandle);
            PifDeleteFile(uchFileName);
			pKpsBuf->hsKpsBuffFH = -1;  // assign bad file handle indicating file not open.
            return KPS_ERROR;
        }

		/* size 0 is shown after create file, 08/31/01 */
        PifCloseFile(hsFileHandle);
        pKpsBuf->hsKpsBuffFH = PifOpenFile(uchFileName, auchTEMP_OLD_FILE_READ_WRITE);
        if (pKpsBuf->hsKpsBuffFH < 0) {
            return KPS_ERROR;
        }
    }

    return(KPS_PERFORM);
}

/*
*==========================================================================
**    Synopsis: VOID    KpsDeleteFile(VOID)
*                                              
*       Input:  Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*                                                                 
**  Description:
*           Delete kitchen buffer file file in case of loding loader B/D.
*==========================================================================
*/
VOID KpsDeleteFile(VOID)
{

    if (KpsBuffer1.hsKpsBuffFH >= 0) {
        PifCloseFile(KpsBuffer1.hsKpsBuffFH);
		KpsBuffer1.hsKpsBuffFH = -1;
    }
    PifDeleteFile(aszKPSBuffFile1);

    if (KpsBuffer2.hsKpsBuffFH >= 0) {
        PifCloseFile(KpsBuffer2.hsKpsBuffFH);
		KpsBuffer2.hsKpsBuffFH = -1;
    }
    PifDeleteFile(aszKPSBuffFile2);

    if (KpsBuffer3.hsKpsBuffFH >= 0) {
        PifCloseFile(KpsBuffer3.hsKpsBuffFH);
		KpsBuffer3.hsKpsBuffFH = -1;
    }
    PifDeleteFile(aszKPSBuffFile3);

    if (KpsBuffer4.hsKpsBuffFH >= 0) {
        PifCloseFile(KpsBuffer4.hsKpsBuffFH);
		KpsBuffer4.hsKpsBuffFH = -1;
    }
    PifDeleteFile(aszKPSBuffFile4); /* corrected to prevent another file close, 01/10/01 */

    if (KpsBuffer5.hsKpsBuffFH >= 0) {
        PifCloseFile(KpsBuffer5.hsKpsBuffFH);
		KpsBuffer5.hsKpsBuffFH = -1;
    }
    PifDeleteFile(aszKPSBuffFile5);

    if (KpsBuffer6.hsKpsBuffFH >= 0) {
        PifCloseFile(KpsBuffer6.hsKpsBuffFH);
		KpsBuffer6.hsKpsBuffFH = -1;
    }
    PifDeleteFile(aszKPSBuffFile6);

    if (KpsBuffer7.hsKpsBuffFH >= 0) {
        PifCloseFile(KpsBuffer7.hsKpsBuffFH);
		KpsBuffer7.hsKpsBuffFH = -1;
    }
    PifDeleteFile(aszKPSBuffFile7);

    if (KpsBuffer8.hsKpsBuffFH >= 0) {
        PifCloseFile(KpsBuffer8.hsKpsBuffFH);
		KpsBuffer8.hsKpsBuffFH = -1;
    }
    PifDeleteFile(aszKPSBuffFile8);

    if (KpsBuffer9.hsKpsBuffFH >= 0) {
        PifCloseFile(KpsBuffer9.hsKpsBuffFH);
		KpsBuffer9.hsKpsBuffFH = -1;
    }
    PifDeleteFile(aszKPSBuffFile9);

    if (KpsBuffer10.hsKpsBuffFH >= 0) {
        PifCloseFile(KpsBuffer10.hsKpsBuffFH);
		KpsBuffer10.hsKpsBuffFH = -1;
    }
    PifDeleteFile(aszKPSBuffFile10);

    if (KpsBuffer11.hsKpsBuffFH >= 0) {
        PifCloseFile(KpsBuffer11.hsKpsBuffFH);
		KpsBuffer11.hsKpsBuffFH = -1;
    }
    PifDeleteFile(aszKPSBuffFile11);

    if (KpsBuffer12.hsKpsBuffFH >= 0) {
        PifCloseFile(KpsBuffer12.hsKpsBuffFH);
		KpsBuffer12.hsKpsBuffFH = -1;
    }
    PifDeleteFile(aszKPSBuffFile12);

    if (KpsBuffer13.hsKpsBuffFH >= 0) {
        PifCloseFile(KpsBuffer13.hsKpsBuffFH);
		KpsBuffer13.hsKpsBuffFH = -1;
    }
    PifDeleteFile(aszKPSBuffFile13);

    if (KpsBuffer14.hsKpsBuffFH >= 0) {
        PifCloseFile(KpsBuffer14.hsKpsBuffFH);
		KpsBuffer14.hsKpsBuffFH = -1;
    }
    PifDeleteFile(aszKPSBuffFile14);

    if (KpsBuffer15.hsKpsBuffFH >= 0) {
        PifCloseFile(KpsBuffer15.hsKpsBuffFH);
		KpsBuffer15.hsKpsBuffFH = -1;
    }
    PifDeleteFile(aszKPSBuffFile15);

    if (KpsBuffer16.hsKpsBuffFH >= 0) {
        PifCloseFile(KpsBuffer16.hsKpsBuffFH);
		KpsBuffer16.hsKpsBuffFH = -1;
    }
    PifDeleteFile(aszKPSBuffFile16);
}

                                                                           /* A START (V1.0.0.3) */
/*
*==========================================================================
**    Synopsis:   static SHORT KpsIsWaitingEndFrame()
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Zero     : all KP is not spooling.
*                 NON-zero : one or more KPSBUFF is waiting for end frame.
*
**  Description:
*           Check if KpsSpoolingQue[] has KPSBUFF
*           This routine is called from KpsPrint() and husKpsMainSem is
*           already requested by KpsPrint().
*==========================================================================
*/
static SHORT KpsIsWaitingEndFrame(VOID)
{
    int     nKpIdx;     /* loop counter */
    SHORT   sRet = 0;   /* zero means 'NOT Spooling' */

    /* for each KP, remove buffer on all spooling queue */
    for (nKpIdx = 0; nKpIdx < KPS_NUMBER_OF_PRINTER; nKpIdx++) {
		KPSQUE *pSpoolQue = &(KpsSpoolingQue[nKpIdx]);   /* spool que */
        if (pSpoolQue->pFirstQue != NULL) {              /* KPSBUFF is linked */
            sRet = -1;
        }
    }

    return sRet;
}                                                                            /* A END (V1.0.0.3) */
/****** End of Source ******/
