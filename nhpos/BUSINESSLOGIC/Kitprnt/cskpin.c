/*========================================================================*\
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
*   Title              : Client/Server Kitchen Printer module, Program List
*   Category           : Client/Server Kitchen Printer module, NCR2170 US HOSPITAL FOR US MODEL
*   Program Name       : CSKPIN.C
*  ------------------------------------------------------------------------
*   Abstract           :The provided function names are as follows.
*
* KpsLockCheck                  * Lock check *
* KpsPrintProcess(VOID);        * Print Process *
* KpsTimerProcess(VOID);        * Timer Process *
* KpsMoniter(VOID);
* KpsPrintEdit(VOID);
* KpsEachTran(VOID);
* KpsEachItem(VOID);
* KpsPrintEditRecover(VOID);
* KpsTranEdit(UCHAR uchPrintNo, UCHAR *auchRcvBuffer, UCHAR uchPrinter);
* KpsItemEdit(UCHAR uchPrintNo, UCHAR *auchRcvBuffer, USHORT usReadPointer, USHORT usWritePointer, USHORT usReadPoint, UCHAR uchPrinter);
* KpsItemPrint(UCHAR uchPrinterNo, SHORT hsFileHandle, USHORT usOffset)
* KpsAlt(USHORT uchPrintNo, UCHAR uchPrinter)
* KpsHeader(UCHAR uchPrinterNo, UCHAR *puchHeaderAddr)
* KpsTrailer(UCHAR uchPrinterNo, UCHAR *puchTrailerAddr)
* KpsPaperCut(UCHAR uchPrinterNo)
* KpsVoidPrint(UCHAR uchPrinterNo)
* KpsMakeOutPutPrinter(KPSBUFFER *pKpsBuffer, UCHAR *puchPrinterStatus)
* Kps_Print(UCHAR uchPrinterNo, UCHAR uchColor, UCHAR uchChrtype, UCHAR *puchDataAddress, UCHAR uchSize)
* KpsAltCheck(UCHAR uchPrinterNo, KPSBUFFER *pKpsBuffer)
* KpsWriteCom(UCHAR uchPrinterNo, UCHAR *puchBuffer, UCHAR cuchCode)
* KpsPowerDownRecover(UCHAR uchPrinterNo)
* KpsDownPrinterPrint(UCHAR uchPrinterNo)
* KpsPaperFeed(UCHAR uchPrinterNo UCHAR uchFeed)
* *KpsQueTransfer(KPSQUE *KpsDeque, KPSQUE *KpsEnque)
* KpsClearRecoveryQue(VOID)
* KpsRecoveryCheckAndInit(KPSQUE *pque)
* KpsPowerDownCheck(VOID)
* KpsWaitPowerDown(VOID)
* KpsCheckAsMaster(VOID)
* KpsStartPrint(VOID)
* KpsCheckTime(DATE_TIME *pOld_Date_Time, DATE_TIME *pNow_Date_Time)
* KpsSetProtocol(PROTOCOL *pProtocol)
* KpsGetBuffFile(ULONG ulOffset, VOID *pData,
                 USHORT usSize, SHORT hsFileHandle)
* KpsPutBuffFile(ULONG ulOffset, VOID *pData,
                 USHORT usSize, SHORT hsFileHandle)
* KpsCheckSharedPara(VOID)                              Add R3.0 Removed.
* KpsShrInit(VOID)                                      Add R3.0
**
*  ------------------------------------------------------------------------
*    Update Histories   :
*
*   Date        Ver.Rev  :NAME         :Description
*   May-06-92 : 00.00.01 :M.YAMAMOTO   :Initial
*   Feb-08-93 : 00.01.09 :M.YAMAMOTO   :Add Protocol Converter.
*   May-19-93 :          :M.YAMAMOTO   :Correct KitPrintProcess()
*   Mar-11-94 : 00.00.04 :K.YOU        :add GC flex feature.(mod. KpsEachItem,
*                                      :KpsPrintEditRecover, KpsTranEdit, KpsItemPrint
*                                      :KpsHeader, KpsTrailer)
*                                      :(add KpsGetBuffFile, KpsPutBuffFile)
*   Apr-07-94 : 00.00.04 :M.SUZUKI     :change KpsWaitPowerDown(),
*                                      :KpsStartPrint().
*   Mar-30-95 : 02.05.04 :M.SUZUKI     :Add 7193 Control (mod. KpsMoniter,
*                                      :KpsAlt,KpsItemPrint,Kps_Print
*                                      :KpsPowerDownRecover,KpsDownPrinterPrint
*   May-11-95 : 02.05.04 :M.SUZUKI     :R3.0
*   Nov-06-95 : 03.01.00 :T.Nakahata   :R3.1 Initial
*
** NCR2171 **
*   Aug-26-99 : 01.00.00 :M.Teraki     :initial (for 2171)
*   Dec-16-99 : 01.00.00 :hrkato       :Saratoga
*   May-01-00 :          :M.Teraki     :Removed KpsCheckSharedPara()
*
** GenPOS **
*   Apr-24-15 : 02.02.01 :R.Chambers   :moved KPSSHRINF KpsShrInf from cskp.c to here.
*   Jan-11-18 : 02.02.02 : R.Chambers  :implementing Alt PLU Mnemonic in Kitchen Printing.
*   Dec-24-23 : 02.04.00 : R.Chambers  :made KpsItemPrint() static, read pointer now ULONG.
*   Dec-31-23 : 02.04.00 : R.Chambers  :moved global g_uchPortStatus from cskp.c to this file.
\*=======================================================================*/
#include	<windows.h>
#include	<tchar.h>
#include    <string.h>
#include    <stdio.h>

#include    "ecr.h"
#include    "pif.h"
#include    "paraequ.h"
#include    "para.h"
#include    "cskp.h"
#include    "csstbpar.h"
#include    "appllog.h"
#include    "nb.h"
#include    "plu.h"
#include    "csstbfcc.h"
#include    "csstbstb.h"
#include    "csserm.h"
#include    "shr.h"
#include    "csstbshr.h"
#include    "cskpin.h"
#include    <rfl.h>
#include "display.h"
#include "uie.h"

static TCHAR   auchKpsPrintBuffer[KPS_PRINT_BUFFER_SIZE];      /* buffer for output to kitchen printer */

static TCHAR   auchTmpBuff[KPS_FRAME_SIZE];

static KPSSHRINF KpsShrInf;         /* shared buffer    Add R3.0 */

static SHORT KpsItemPrint(UCHAR uchPrinterNo, SHORT hsFileHandle, ULONG ulReadPointer);
static SHORT KpsHeader(UCHAR uchPrinterNo, SHORT hsFileHandle, ULONG ulReadPointer);
static SHORT KpsTrailer(UCHAR uchPrinterNo, SHORT hsFileHandle, ULONG ulReadPointer);
static SHORT KpsGetBuffFile(ULONG ulOffset, VOID* pData, ULONG ulSize, SHORT hsFileHandle, ULONG* pulActualBytesRead);
static VOID  KpsPutBuffFile(ULONG ulOffset, VOID* pData, USHORT usSize, SHORT hsFileHandle);
static SHORT KpsTranEdit(UCHAR uchPrinterNo, KPSBUFFER* pBuffer, UCHAR uchPrinter, UCHAR* pauchDownPriter);

USHORT  g_uchPortStatus = 0;                             /* com ports status table *//* ### NEW 2172 Rel1.0 */

/*
*==========================================================================
**    Synopsis:   VOID   KpsSetProtocol(PROTOCOL *pProtocol)
*
*     Inout  :    pProtocol  - Storage location for data of protocol.
*                       
**  Return   :    0 <   -   It is impossible to messure time. rather than a hour.
*                 0 =>  -   Time (minutes)
*
**  Description:
*               This function is storage data of protocol.
*               
*               See also function PifGetSerialConfig() which sets up the SYSCONFIG memory
*               resident data base.
*==========================================================================
*/
static VOID    KpsSetProtocol(USHORT usPortId, PROTOCOL *pProtocol)
{
    SYSCONFIG CONST *pSysconfig = PifSysConfig();                /* get system configration */
	XGHEADER  xgHeader = {0};

	pProtocol->fPip = PIF_COM_PROTOCOL_NON;
    pProtocol->usPipAddr = 0;
    pProtocol->ulComBaud = 9600;            // 9600 baud
    pProtocol->uchComByteFormat = 0x1B;     // Even parity, 8 data bits, 1 stop bit.  COM_BYTE_EVEN_PARITY | COM_BYTE_8_BITS_DATA
    pProtocol->uchComTextFormat = 0;
    pProtocol->auchComNonEndChar[0] = 0;
    pProtocol->auchComNonEndChar[1] = 0;
    pProtocol->auchComNonEndChar[2] = 0;
    pProtocol->auchComNonEndChar[3] = 0;
    pProtocol->auchComEndChar[0] = 0;
    pProtocol->auchComEndChar[1] = 0;
    pProtocol->auchComEndChar[2] = 0;
    pProtocol->uchComDLEChar = 0;
    pProtocol->usNetPortNo = 0;
    pProtocol->xgHeader = xgHeader;

	pProtocol->fPip = pSysconfig->usfPip[usPortId];
	pProtocol->ulComBaud = pSysconfig->ausComBaud[usPortId];
	pProtocol->uchComByteFormat = pSysconfig->auchComByteFormat[usPortId];
	pProtocol->xgHeader = pSysconfig->xgComHeader[usPortId];
}

/*
*==========================================================================
**    Synopsis:   VOID  KpsLockCheck(UCHAR uchUniqueAddress)
*
*       Input:    uchUniqueAddress           teminal unique address 
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*               Terminal Lock check.
*==========================================================================
*/
SHORT KpsLockCheck(UCHAR uchUniqueAddress)
{
    if ((!g_uchKpsTerminalLock) || (uchUniqueAddress == (UCHAR)g_uchKpsTerminalLock)) {
        return(KPS_PERFORM);
    }
    return(KPS_LOCK);
}
/*
*==========================================================================
**    Synopsis:   VOID  KpsPrintProcess(VOID)
*
*       Input:    Nothing 
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*               Print process.
*==========================================================================
*/
VOID THREADENTRY  KpsPrintProcess(VOID)
{
    BOOL    bMsgFromMonProc = FALSE;

    for(;;) {
        USHORT  *pusMessage;

        uchKpsPrinting = 0;

        /* wake up monitor thread */
        if (bMsgFromMonProc == TRUE) {
            PifReleaseSem(husKpsQueueSem);
        }

        pusMessage = PifReadQueue(husKpsQueue);
		if (pusMessage == NULL) {
			PifSleep (1000);
			continue;
		}

        /* check if semaphoe release is requierd or not next time */
        if ((*pusMessage == KPS_MSG_FROM_RCV_MONITER) ||
            (*pusMessage == KPS_MSG_FROM_MONITER)) {
            bMsgFromMonProc = TRUE; /* semaphoe should be released at next time */
        }
        else {
            bMsgFromMonProc = FALSE; /* semaphoe should not be released at next time */
        }           

        uchKpsPrinting = 1;

        if (KpsRcvQue.pFirstQue != NULL) {
            SHORT   sWork;
            DATE_TIME DateTime;
            PifGetDateTime(&DateTime);
            if ((sWork = KpsCheckTime(&KpsRcvQue.pFirstQue->Date_Time, &DateTime)) >= 0) {
                if (sWork > 5) {
                    KpsClearRecoveryQue();  /* Time up 5 minute */
                }
            } else {
                KpsClearRecoveryQue();  /* Time up 5 minute */
            }
        }

        KpsPowerDownCheck();

        if (*pusMessage == KPS_MSG_FROM_MONITER) {
			SHORT  nPortCnt;
			ULONG  ulPrinterSet1 = 0;
			ULONG  ulPrinterSet2 = 0;
			for (nPortCnt = 0; nPortCnt < KPS_NUMBER_OF_PRINTER; nPortCnt++) {
				ulPrinterSet1 |= (auchKpsDownPrinter[nPortCnt] & KPS_DOWN_PRINTER) ? (1 << nPortCnt) : 0;
			}
            KpsMoniter(0);
			for (nPortCnt = 0; nPortCnt < KPS_NUMBER_OF_PRINTER; nPortCnt++) {
				ulPrinterSet2 |= (auchKpsDownPrinter[nPortCnt] & KPS_DOWN_PRINTER) ? (1 << nPortCnt) : 0;
			}
			if (ulPrinterSet1 ^ ulPrinterSet2) {
				char  xBuff[128];
				sprintf (xBuff, "==NOTE: kitchen printer down state change 0x%8.8x to 0x%8.8x", ulPrinterSet1, ulPrinterSet2);
				NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
			}
            continue;
        }
        if (*pusMessage == KPS_MSG_FROM_RCV_MONITER) {
            if (uchKpsFlag & KPS_STOP_RECOVERY) {
                continue;
            }
            KpsMoniter(0);
            if (KpsRcvQue.pFirstQue != NULL) {
                KpsPrintEditRecover(KpsRcvQue.pFirstQue);
                if (KpsRecoveryCheckAndInit(&KpsRcvQue) != KPS_PERFORM) {
                    continue;
                } else {
                    KpsQueTransfer(&KpsRcvQue, &KpsEmptyQue);
                }
            }
        } else {
            DATE_TIME DateTime = { 0 };
            PifGetDateTime(&DateTime);
            KpsProcQue.pFirstQue->Date_Time = DateTime;  /* copy structure */
            KpsPrintEdit(KpsProcQue.pFirstQue);
            if (KpsRecoveryCheckAndInit(&KpsProcQue) == KPS_PERFORM) {
                KpsQueTransfer(&KpsProcQue, &KpsEmptyQue);
            }
        }
    }
}
/*
*==========================================================================
**    Synopsis:   VOID  KpsTimerProcess(VOID)
*
*       Input:    Nothing 
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*               Timer Process.
*==========================================================================
*/
VOID  THREADENTRY KpsTimerProcess(VOID)
{
    USHORT  usMsg;

    for (;;) {

        /* guard multiple message passing from this thread */
        PifRequestSem(husKpsQueueSem);

        PifSleep(usKpsTimerValue);

        if (KpsRcvQue.pFirstQue != NULL) {
           usKpsTimerValue = KPS_RECOVER_TIMER_VALUE;
           usMsg = KPS_MSG_FROM_RCV_MONITER;    
        } else {
           usKpsTimerValue = KPS_IDLE_TIMER_VALUE;
           usMsg = KPS_MSG_FROM_MONITER;
        }

        /* request to print thread */
        PifWriteQueue(husKpsQueue, &usMsg);
    }
}

/*
*==========================================================================
**    Synopsis:   VOID  KpsMoniter(VOID)
*
*       Input:    Nothing 
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*               Moniter Process.
*               2000.04.27 removed polling process to KP connected other terminal
*==========================================================================
*/
VOID  KpsMoniter(USHORT usInit)
{
    SYSCONFIG CONST *pSysconfig = PifSysConfig();                /* get system configration */

    /* make(initialize) COM - printer table */
    _KpsMakePortTable(); /* ### ADD 2172 Rel1.0 */

    /* ### CAUTION!!! PortCnt != KPrinterNo (in Saratoga) */
    for (SHORT nPortCnt = 0; nPortCnt < KPS_NUMBER_OF_PRINTER; nPortCnt++) {
        USHORT   const uchMask = (0x01 << nPortCnt);
        /* --- determine this K/P is already stated POWER DOWN or not --- */
        if (g_auchKpsSysConfig[nPortCnt] == (UCHAR)(~DEVICE_KITCHEN_PRINTER)) {
            g_uchPortStatus &= ~uchMask;    /* set printer status is down */
			auchKpsDownPrinter[nPortCnt] = KPS_NOT_DOWN_PRINTER; //SR456
            continue;
        }
        else {
			PROTOCOL Protocol = {0};
			USHORT  usPortId = g_auchKpsSysConfig[nPortCnt];

			KpsSetProtocol(usPortId, &Protocol);      /* Set protocol data */

            for (USHORT usRetryCounter = 0; usRetryCounter < KPS_RETRY_COUNTER; usRetryCounter++) {
	            int		nSize;
                USHORT  usTime;
                SHORT   sStatus;
                UCHAR   cuchCode = 0;
                UCHAR   auchReadBuffer[KPS_READ_BUFFER_SIZE];
	            UCHAR	uchTmpKPBuff[KPS_PRINT_BUFFER_SIZE*2];

                if (g_hasKpsPort[nPortCnt] < 0) { /* specified K/P is not assigned port */
                    SHORT   hsPort = PifOpenNetComIoEx (usPortId, &Protocol);
                    if (hsPort < 0) {                   /* re-connection is failed */
                        g_uchPortStatus &= ~uchMask;    /* set printer status is down */
						if ((usRetryCounter+1) >= KPS_RETRY_COUNTER) {
							if (!(auchKpsDownPrinter[nPortCnt] & KPS_DOWN_PRINTER)) {
								PifLog (MODULE_KPS, LOG_EVENT_KPS_PORT_OPEN_FAIL);
								PifLog(MODULE_DATA_VALUE(MODULE_KPS), usPortId);
								PifLog(MODULE_ERROR_NO(MODULE_KPS), hsPort);
								auchKpsDownPrinter[nPortCnt] = KPS_DOWN_AND_TROUBLE; //SR456
								// determine whether we are processing at GenPOS startup or not.
								// if at GenPOS startup, the error dialog functionality is not properly
								// functional so we do not want to display an error dialog therefore we
								// will issue an ASSRTLOG instead.  Issue found with kitchen printer
								// provisioned but no kitchen printer attached.
								if (usInit == 0) {
									KpsDisplayPrtError();
								} else {
									NHPOS_ASSERT_TEXT ((auchKpsDownPrinter[nPortCnt] != KPS_DOWN_AND_TROUBLE), "==NOTE: Kps Init Printer error.");
								}
							}
						}
                        continue;    // attempt to open port failed. attempt to retry again
                    }
                    g_hasKpsPort[nPortCnt] = hsPort;  /* re-connection is successful */
					if (auchKpsDownPrinter[nPortCnt] & KPS_DOWN_PRINTER) {
						PifLog (MODULE_KPS, LOG_EVENT_KPS_PORT_OPEN_REOPEN);
						PifLog(MODULE_DATA_VALUE(MODULE_KPS), usPortId);
					}
                }

                /* Set Time-out Timer of send */
                usTime = KPS_MONITOR_SEND_TIME_OUT;
                sStatus = PifControlNetComIoEx(g_hasKpsPort[nPortCnt], PIF_COM_SEND_TIME, usTime);
                if (sStatus == PIF_OK) {
                    usTime = KPS_RECEIVE_TIME_OUT;
                    sStatus = PifControlNetComIoEx (g_hasKpsPort[nPortCnt], PIF_COM_SET_TIME, usTime);
                }
                if ((sStatus == PIF_ERROR_COM_POWER_FAILURE) || (sStatus == PIF_ERROR_COM_ACCESS_DENIED)) {
                    PifCloseNetComIoEx(g_hasKpsPort[nPortCnt]);
                    uchKpsPrintDown = 0;                /* it is not power down during printing */
                    g_hasKpsPort[nPortCnt] = KPS_OPEND_ERROR;
                    g_uchPortStatus &= ~uchMask;    /* set printer status is down */
					auchKpsDownPrinter[nPortCnt] = KPS_DOWN_AND_TROUBLE; //SR456
                    continue;
                }
                cuchCode = 0;
                auchKpsPrintBuffer[cuchCode++] = KPS_ESC;
                auchKpsPrintBuffer[cuchCode++] = KPS_ESC_STATUS;
				nSize = WideCharToMultiByte(pSysconfig->unCodePage, 0, auchKpsPrintBuffer, cuchCode, uchTmpKPBuff, sizeof(uchTmpKPBuff), NULL, NULL);
                sStatus = PifWriteNetComIoEx(g_hasKpsPort[nPortCnt], uchTmpKPBuff, (USHORT)nSize);         /* There is no argument */
                if (sStatus >= 0) {
                    sStatus = PifReadNetComIoEx(g_hasKpsPort[nPortCnt], (VOID *)auchReadBuffer, KPS_READ_BUFFER_SIZE);
                }
                if (sStatus != 1) { /* Read function is failed */
                    if ((sStatus == PIF_ERROR_COM_POWER_FAILURE) || (sStatus == PIF_ERROR_COM_ACCESS_DENIED)) {
                        PifCloseNetComIoEx(g_hasKpsPort[nPortCnt]);
                        uchKpsPrintDown = 0;                /* it is not power down during printing */
                        g_hasKpsPort[nPortCnt] = KPS_OPEND_ERROR;
                        g_uchPortStatus &= ~uchMask;    /* set printer status is down */
						auchKpsDownPrinter[nPortCnt] = KPS_DOWN_AND_TROUBLE; //SR456
                        continue;
                    } else if (sStatus > 0) {               /* noize has received */
                        g_uchPortStatus &= ~uchMask;    /* set printer status is down */
						auchKpsDownPrinter[nPortCnt] = KPS_DOWN_AND_TROUBLE; //SR456
                        continue;
                    }
                    g_uchPortStatus &= ~uchMask;    /* set printer status is down */
					auchKpsDownPrinter[nPortCnt] |= KPS_DOWN_AND_TROUBLE; //SR456
                } else {            /* Read function is successful */
					
					/* --- Check the returned status of the printer to set it down or not down --- */
					if (auchReadBuffer[0] & KPS_STATUS_MASK) {
						g_uchPortStatus &= ~uchMask;    /* set printer status is down */
						if (auchReadBuffer[0] & KPS_COVER_OPEN) { //SR456
							auchKpsDownPrinter[nPortCnt] = KPS_DOWN_AND_OPEN;
						} else if (auchReadBuffer[0] & KPS_PAPER_LOW) {
							auchKpsDownPrinter[nPortCnt] = KPS_DOWN_AND_OUT; 
						} else {
							auchKpsDownPrinter[nPortCnt] = KPS_DOWN_AND_TROUBLE;
						}
					} else {
						/* --- determine this K/P is POWER DOWN before this monitoring --- */
						if (uchMask != (g_uchPortStatus & uchMask)) {   /* power downed */
							cuchCode = 0;
							auchKpsPrintBuffer[cuchCode++] = KPS_ESC;   /* DUMMY */
							auchKpsPrintBuffer[cuchCode++] = KPS_7B;    /* DUMMY */
							auchKpsPrintBuffer[cuchCode++] = KPS_0;     /* DUMMY */
							auchKpsPrintBuffer[cuchCode++] = KPS_ESC;
							auchKpsPrintBuffer[cuchCode++] = KPS_INIT;
							/* SET STOP AT NO PAPER CODE */
							auchKpsPrintBuffer[cuchCode++] = KPS_ESC;
							auchKpsPrintBuffer[cuchCode++] = KPS_NO_PAPER;
							auchKpsPrintBuffer[cuchCode++] = '4';
							auchKpsPrintBuffer[cuchCode++] = KPS_NO_PAPER_STOP;
							nSize = WideCharToMultiByte(pSysconfig->unCodePage, 0, auchKpsPrintBuffer, cuchCode, uchTmpKPBuff, sizeof(uchTmpKPBuff), NULL, NULL);
							if ((sStatus = PifWriteNetComIoEx(g_hasKpsPort[nPortCnt], uchTmpKPBuff, (USHORT)nSize)) < 0) {
								if ((sStatus == PIF_ERROR_COM_POWER_FAILURE) 
									|| (sStatus == PIF_ERROR_COM_ACCESS_DENIED)) {
									uchKpsPrintDown = 0;    /* it is not power down during printing */
									KpsPowerDownRecover((UCHAR)_KpsComNo2KPNo((SHORT)(nPortCnt+1))); /* ### MOD 2172 Rel1.0 (COM # == nPortCnt+1) */
									continue;
								}
								g_uchPortStatus &= ~uchMask;    /* set printer status is down */
								auchKpsDownPrinter[nPortCnt] = KPS_DOWN_AND_TROUBLE; //SR456
							} else {
								g_uchPortStatus |= uchMask;     /* set printer status is not down */
								auchKpsDownPrinter[nPortCnt] = KPS_NOT_DOWN_PRINTER; //SR456
								KpsClearPrtError(); /* clear downed printer status */
							}

							/*********************** ADD R3.0 *********************/
							cuchCode = 0;
							auchKpsPrintBuffer[cuchCode++] = KPS_ESC;
							auchKpsPrintBuffer[cuchCode++] = KPS_ESC_DRAWER;    /* Dummy Command */
							auchKpsPrintBuffer[cuchCode++] = KPS_0;             /* Dummy Command */
							nSize = WideCharToMultiByte(pSysconfig->unCodePage, 0, auchKpsPrintBuffer, cuchCode, uchTmpKPBuff, sizeof(uchTmpKPBuff), NULL, NULL);
							sStatus = PifWriteNetComIoEx(g_hasKpsPort[nPortCnt], uchTmpKPBuff, (USHORT)nSize);
							if (sStatus >= 0) {
								sStatus = PifReadNetComIoEx(g_hasKpsPort[nPortCnt], (VOID *)auchReadBuffer, KPS_READ_BUFFER_SIZE);
							}
							/*********************** ADD R3.0 *********************/

						} else {                                            /* status is OK before monitoring */
							g_uchPortStatus |= uchMask;     /* set printer status is not down */
							auchKpsDownPrinter[nPortCnt] = KPS_NOT_DOWN_PRINTER; //SR456
						}
					}
                }
                break;
            }
        }
    }
}

/*
*==========================================================================
**    Synopsis:   VOID  KpsPrintEdit(KPSBUFFER *KpsBuffer)
*
*       Input:    Nothing 
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*               Print edit process.
*==========================================================================
*/
VOID  KpsPrintEdit(KPSBUFFER *pKpsBuffer)
{
    KpsPowerDownCheck();
    uchKpsPrintDown = KPS_PRINT_DOWN;
    if (CliParaMDCCheck(MDC_KPTR_ADR, ODD_MDC_BIT0)) { /* 1 = each item */
        KpsEachItem(pKpsBuffer);
    } else {
        KpsEachTran(pKpsBuffer);
    }
}

/*
*==========================================================================
**    Synopsis:   VOID  KpsEachTran(KPSBUFFER *pKpsBuffer)
*
*       Input:    Nothing 
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*               Each Transaction process
*==========================================================================
*/
VOID  KpsEachTran(KPSBUFFER *pKpsBuffer)
{
    USHORT  usPrinterCnt;   /* ### CAUTION!!! PrinterCnt -> 0 - 7 PrinterNo -> 1 - 8 */
    SHORT   sStatus;

    for(;;) {
        // cycle through the kitchen printer list to see if there are any with
        // transactions ready to print. If so then print the transaction and
        // move on to the next printer.
        // if there is an error try to use an alternate printer.
        for (usPrinterCnt = 0; usPrinterCnt < KPS_NUMBER_OF_PRINTER; usPrinterCnt++) {
            /* Output K/P Number(1 - 8) after removing any flags, R3.1 Add */
            UCHAR uchOutputKPNo = pKpsBuffer->auchOutputPrinter[usPrinterCnt] & (UCHAR)0x0f;
            if (KPS_CHK_KPNUMBER(uchOutputKPNo)) {
                pKpsBuffer->ulReadPoint = KPS_DATA * 2;
                if (KpsTranEdit((UCHAR)uchOutputKPNo,       /* ### CAUTION! Printer No */
                                pKpsBuffer,
                                (UCHAR)(usPrinterCnt + 1),  /* ### CAUTION! Printer No */
                                auchKpsDownPrinter) == KPS_PERFORM) {
                    /* --- declare the transaction is printed on this remote printer --- */
                    pKpsBuffer->auchOutputPrinter[usPrinterCnt] = 0;
                } else {    /* alternation check */
                    auchKpsDownPrinter[uchOutputKPNo - 1] = KPS_DOWN_AND_TROUBLE; //SR456
                    KpsAltCheck((UCHAR)usPrinterCnt /* ### CAUTION! Printer Counter (0 - 7)*/ , pKpsBuffer);
                }
            }
        }
        for (usPrinterCnt = 0, sStatus = 1; usPrinterCnt < KPS_NUMBER_OF_PRINTER; usPrinterCnt++) {
            if (pKpsBuffer->auchOutputPrinter[usPrinterCnt] != 0) {
                sStatus = 0;    /* declare some K/P print is failed */
                break;
            }
        }
        if (sStatus == 1) { /* print each transaction is complete */
            break;
        }
    }
}

/*
*==========================================================================
**    Synopsis:   VOID  KpsEachItem(KPSBUFFER *pKpsBuffer)
*
*       Input:    Nothing 
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*               Each Item process
*==========================================================================
*/
VOID  KpsEachItem(KPSBUFFER *pKpsBuffer)
{        
    USHORT  usPrinterCnt;
    SHORT   sStatus;

    for (;;) {
        for (usPrinterCnt = 0; usPrinterCnt < KPS_NUMBER_OF_PRINTER; usPrinterCnt++) {
            UCHAR uchPrintSelection =  (UCHAR)(0x01 << usPrinterCnt);   /*####*/
            /* Output K/P Number, R3.1 Add */
            UCHAR uchOutputKPNo = pKpsBuffer->auchOutputPrinter[usPrinterCnt] & (UCHAR)0x0f;
            if (!KPS_CHK_KPNUMBER(uchOutputKPNo)) {
                continue;
            }
            /*uchOpPrtNo = (UCHAR)(0x01 << (uchOutputKPNo - 1));
            if (!(chKpsPrinterStatus & uchOpPrtNo))   * <> 0 */
            if(!_KpsIsEnable((SHORT)uchOutputKPNo)){    /* ### MOD 2172 Rel1.0 */
                KpsAltCheck((UCHAR)usPrinterCnt, pKpsBuffer);
                continue;
            }
            pKpsBuffer->ulReadPoint = KPS_DATA;
            for (;;) {
                UCHAR   uchGetData;
	            ULONG	ulActualBytesRead; //RPH 11-10-3 SR# 201
                //RPH 11-10-3 SR# 201
				KpsGetBuffFile(pKpsBuffer->ulReadPoint + KPS_PRINT_SEL,
                               &uchGetData,
                               sizeof(UCHAR),
                               pKpsBuffer->hsKpsBuffFH, &ulActualBytesRead);
                if (uchGetData & (UCHAR)KPS_PRINT_SEL_MASK) {  /* Is this item a remote print item ? */
                    if (uchGetData & uchPrintSelection) {   /* Does this item print on this remote printer ? */
                        if (KpsItemEdit((UCHAR)uchOutputKPNo,
                                        pKpsBuffer,
                                        (UCHAR)(usPrinterCnt + 1),
                                        auchKpsDownPrinter) == KPS_PERFORM) {
                            /* --- declare the item is printed on this remote printer --- */
                            uchGetData &= ~uchPrintSelection;
                            KpsPutBuffFile(pKpsBuffer->ulReadPoint + KPS_PRINT_SEL,
                                           &uchGetData,
                                           sizeof(UCHAR),
                                           pKpsBuffer->hsKpsBuffFH);
                        } else {    /* alternation check */
                            auchKpsDownPrinter[uchOutputKPNo - 1] = KPS_DOWN_AND_TROUBLE; //SR456
                            KpsAltCheck((UCHAR)usPrinterCnt, pKpsBuffer);
                            break;      /* exit loop, and print next K/P */
                        }
                    }
                }
                /* --- point to next item --- */
                //RPH 11-10-3 SR# 201
				KpsGetBuffFile(pKpsBuffer->ulReadPoint + KPS_ILI,
                               &uchGetData,
                               sizeof(UCHAR),
                               pKpsBuffer->hsKpsBuffFH, &ulActualBytesRead);
                pKpsBuffer->ulReadPoint += uchGetData;
                
                /* --- determine all item in a transaction is completely printed --- */
                if (pKpsBuffer->ulReadPoint >= pKpsBuffer->ulWritePoint) {
                    /* --- declare the transaction is printed on this remote printer --- */
                    pKpsBuffer->auchOutputPrinter[usPrinterCnt] = 0;
                    break;      /* exit loop, and print next K/P */
                }
            }
        }
        for (usPrinterCnt = 0, sStatus = 1; usPrinterCnt < KPS_NUMBER_OF_PRINTER; usPrinterCnt++) {
            if (pKpsBuffer->auchOutputPrinter[usPrinterCnt] != 0) {
                sStatus = 0;    /* declare some K/P print is failed */
                break;
            }
        }
        if (sStatus == 1) { /* print each item is complete */
            break;
        }
    }
}

/*
*==========================================================================
**    Synopsis:   VOID  KpsPrintEditRecover(KPSBUFFER *KpsBuffer)
*
*       Input:    Nothing 
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*               edit print data recovery
*==========================================================================
*/
VOID  KpsPrintEditRecover(KPSBUFFER *pKpsBuffer)
{
    USHORT  usTime;
    SHORT   sStatus = KPS_PRINT_ERROR;
    UCHAR   uchPrinterNo;
    UCHAR   uchPrinterCnt;
    UCHAR   uchOpPrtNo;
    UCHAR   uchPrtNoFmAppl;
    UCHAR   cuchI;
    UCHAR   uchGetData;
    UCHAR   auchManuAlt[KPS_NUMBER_OF_PRINTER] = { 0 };

    ParaManuAltKitchReadAll(auchManuAlt);

    for (cuchI = 0; cuchI < KPS_NUMBER_OF_PRINTER; cuchI++) {
        if (KPS_CHK_KPNUMBER(auchManuAlt[cuchI])) {
            /*if (0 != (uchKpsPrinterStatus & (0x01 << (UCHAR)(auchManuAlt[cuchI] - 1))) )*/
            if (_KpsIsEnable((SHORT)auchManuAlt[cuchI])) { /* ### MOD 2172 rel1.0 */
                if (pKpsBuffer->auchRequestRecover[cuchI]) {
                    pKpsBuffer->auchRequestRecover[cuchI] = auchManuAlt[cuchI];
                    pKpsBuffer->auchRequestRecover[cuchI] |= 0x80;   /* Manual alternation bit id */
                }
            }
        } else if (0 == auchManuAlt[cuchI]) {
            if (0x80 == (pKpsBuffer->auchRequestRecover[cuchI] & 0xf0)) {
                pKpsBuffer->auchRequestRecover[cuchI] = (UCHAR)(cuchI + 1);
            }
        }
    }

    for (usTime = 0; usTime < 2; usTime++) {
        for (uchPrinterCnt = 0; uchPrinterCnt < KPS_NUMBER_OF_PRINTER; uchPrinterCnt++)  {
            pKpsBuffer->ulReadPoint = KPS_DATA;
            if (pKpsBuffer->auchRequestRecover[uchPrinterCnt]) {  /* auchKpsRequestRecover[uchPrinterCnt] != 0 */
                if ((usTime) || (0x80 == (pKpsBuffer->auchRequestRecover[uchPrinterCnt] & (UCHAR)0xf0))) {
                    uchOpPrtNo = (UCHAR)(0x01 << ((UCHAR)((pKpsBuffer->auchRequestRecover[uchPrinterCnt] & (UCHAR)0x0f) - 1)));
                    uchPrtNoFmAppl = (UCHAR)(0x01 << uchPrinterCnt);
                    uchPrinterNo = (UCHAR)(pKpsBuffer->auchRequestRecover[uchPrinterCnt] & (UCHAR)0x0f);
                } else {
                    uchOpPrtNo =  (UCHAR)(0x01 << uchPrinterCnt); /* First Time */
                    uchPrtNoFmAppl = uchOpPrtNo;
                    uchPrinterNo = (UCHAR)(uchPrinterCnt + 1);
                }
                /*if (uchKpsPrinterStatus & uchOpPrtNo)   * <> 0 */
                if( _KpsIsEnable((SHORT)uchPrinterNo) ){ /* ### MOD 2172 Rel1.0 */
                    if (ParaMDCCheck(MDC_KPTR_ADR, ODD_MDC_BIT0)) { /* 1 = each item */
                        ULONG	ulActualBytesRead;//RPH 11-10-3 SR# 201
                        for (;;) {
                            //RPH 11-10-3 SR# 201
							KpsGetBuffFile(pKpsBuffer->ulReadPoint + KPS_PRINT_SEL,
                                           &uchGetData,
                                           sizeof(UCHAR),
                                           pKpsBuffer->hsKpsBuffFH, &ulActualBytesRead);
                            if (uchGetData & uchPrtNoFmAppl) {  /* Does this item print on this remote printer ? */
                                if ((sStatus = KpsItemEdit(uchPrinterNo,
                                                           pKpsBuffer,
                                                           (UCHAR)(uchPrinterCnt + 1),
                                                           auchKpsDownPrinter)) == KPS_PERFORM) {
                                    /* --- declare the item is printed on this remote printer --- */
                                    uchGetData &= ~uchPrtNoFmAppl;
                                    KpsPutBuffFile(pKpsBuffer->ulReadPoint + KPS_PRINT_SEL,
                                                   &uchGetData,
                                                   sizeof(UCHAR),
                                                   pKpsBuffer->hsKpsBuffFH);
                                } else {
                                    auchKpsDownPrinter[uchPrinterNo - 1] = KPS_DOWN_AND_TROUBLE; //SR456
                                    break;                                
                                }
                            }
                            /* --- point to next item --- */
                            //RPH 11-10-3 SR# 201
							KpsGetBuffFile(pKpsBuffer->ulReadPoint + KPS_ILI,
                                           &uchGetData,
                                           sizeof(UCHAR),
                                           pKpsBuffer->hsKpsBuffFH, &ulActualBytesRead);
                            pKpsBuffer->ulReadPoint += uchGetData;
                            /* --- determine all item in a transaction is completely printed --- */
                            if (pKpsBuffer->ulReadPoint >= pKpsBuffer->ulWritePoint) {
                                break;
                            }
                        }
                    } else {                                        /* 0 = each transaction */
                        sStatus = KpsTranEdit(uchPrinterNo,
                                              pKpsBuffer,
                                              (UCHAR)(uchPrinterCnt + 1),
                                              auchKpsDownPrinter);
                    }
                    if (sStatus == KPS_PERFORM) {   /* recovery print is successful */
                        /*uchKpsPrinterStatus |= uchOpPrtNo;*/
                        _KpsSetPrinterSts((USHORT)uchPrinterNo,TRUE); /* ### MOD 2172 Rel1.0 */
                        pKpsBuffer->auchRequestRecover[uchPrinterCnt] = 0;
                    } else {
                        auchKpsDownPrinter[uchPrinterNo - 1] = KPS_DOWN_AND_TROUBLE; //SR456
                    }
                } 
            } /* end if */
        } /* end for( NO_OF_PRINTER ) */
    } /* end for( RETRY_COUNTER ) */
}
/*
*==========================================================================
**    Synopsis:   SHORT KpsTranEdit(UCHAR uchPrintNo, KPSBUFFER *pBuffer, UCHAR uchPrinter)
*
*       Input:    USHORT    uchPrintNo          * Printer No to be printed * ### CAUTION!!! (1 - 8)
*                 KPSBUFFER *pBuffer         * Pinter to data at ili. *
*                 UCHAR     uchPrinter          * Printer No by Application *### CAUTION!!! (1 - 8)
*                 UCHAR     *pauchDownPrinter   * Down printer no *
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    KPS_PERFORM                   * normal end *
*                 KPS_PRINT_ERROR               * Printer error *
*
**  Description:
*               Edit 1 transaction.
*==========================================================================
*/
static SHORT   KpsTranEdit(UCHAR uchPrinterNo, KPSBUFFER *pBuffer, UCHAR uchPrinter, UCHAR *pauchDownPriter)
{
    USHORT   uchIli;
    USHORT   uchPrinterSelection;
    SHORT    sStatus = KPS_PERFORM;
    UCHAR    uchFeed;
    UCHAR    uchTrailer = 0;      /* flag for trailer printing */

    KpsShrInit();
    while (pBuffer->ulReadPoint < pBuffer->ulWritePoint) {
	    ULONG	 ulActualBytesRead;//RPH 11-10-3 SR# 201
        //RPH 11-10-3 SR# 201
		KpsGetBuffFile(pBuffer->ulReadPoint,
                       &uchIli,
                       sizeof(USHORT),
                       pBuffer->hsKpsBuffFH, &ulActualBytesRead);
        if (uchIli <= 2) {
            pBuffer->ulReadPoint += uchIli;    /* point next ili */
            continue;
        }
        //RPH 11-10-3 SR# 201
		KpsGetBuffFile(pBuffer->ulReadPoint + 2,
                       &uchPrinterSelection,
                       sizeof(USHORT),
                       pBuffer->hsKpsBuffFH, &ulActualBytesRead);
        uchPrinterSelection &= KPS_PRINT_SEL_MASK;

        if (!uchTrailer) {   /* uchTrailer == 0 */
            if (KPS_DOWN_PRINTER & pauchDownPriter[uchPrinterNo - 1]) { //SR456
                uchFeed = KPS_HEAD_FEED_RECOVER;
            } else {
                uchFeed = KPS_HEAD_FEED;
            }
            if (KPS_PRINT_ERROR == KpsPaperFeed(uchPrinterNo, uchFeed)) {
                /* uchKpsPrinterStatus &= ~(0x01 << (uchPrinterNo - 1));*/
                _KpsSetPrinterSts(uchPrinterNo,FALSE);/* ### MOD 2172 Rel1.0 */
                return(KPS_PRINT_ERROR);
            }
            if (KPS_DOWN_PRINTER & pauchDownPriter[uchPrinterNo - 1]) { //SR456
                /* --- print "////////" mnemonic on K/P */
                sStatus = KpsDownPrinterPrint(uchPrinterNo);
                pauchDownPriter[uchPrinterNo - 1] = 0;
                if (sStatus == KPS_PRINT_ERROR) {
                    /*uchKpsPrinterStatus &= ~(0x01 << (uchPrinterNo - 1));*/
                    _KpsSetPrinterSts(uchPrinterNo,FALSE);/* ### MOD 2172 Rel1.0 */
                    return(sStatus);
                }
            }

            if(_KpsIsAlt(pBuffer->usOutPrinterInfo))
                sStatus = KpsAlt(uchPrinterNo, (UCHAR)(pBuffer->usOutPrinterInfo & 0x0ff));

            if (sStatus != KPS_PRINT_ERROR) {
                sStatus = KpsHeader(uchPrinterNo, pBuffer->hsKpsBuffFH, KPS_HEADER);
                uchTrailer = 1;         /* Set Trailer flag */
            }
        }
        if (sStatus == KPS_PRINT_ERROR) {
            /*uchKpsPrinterStatus &= ~(0x01 << (uchPrinterNo - 1));*/
            _KpsSetPrinterSts(uchPrinterNo,FALSE);/* ### MOD 2172 Rel1.0 */
            return(sStatus);
        }
        if ((sStatus = KpsItemPrint(uchPrinterNo, pBuffer->hsKpsBuffFH, pBuffer->ulReadPoint)) == KPS_PRINT_ERROR) {
            /*uchKpsPrinterStatus &= ~(0x01 << (uchPrinterNo - 1));*/
            _KpsSetPrinterSts(uchPrinterNo,FALSE);/* ### MOD 2172 Rel1.0 */
            return(sStatus);
        }
        pBuffer->ulReadPoint += uchIli;    /* point next ili */
    }

    if (uchTrailer) {   /* uchTrailer != 0 */
        if ((sStatus = KpsTrailer(uchPrinterNo, pBuffer->hsKpsBuffFH, (KPS_TRAILER * sizeof(TCHAR)))) == KPS_PRINT_ERROR) {
            /*uchKpsPrinterStatus &= ~(0x01 << (uchPrinterNo - 1));*/
            _KpsSetPrinterSts(uchPrinterNo,FALSE);/* ### MOD 2172 Rel1.0 */
            return(sStatus);
        }
		KpsPaperCut(uchPrinterNo);
		//We send a transaction end command to the printer so we print the Kitchen PRinter Chit JHHJ
		KpsTransactionPrint(uchPrinterNo, KPS_TRANS_END);
    }
    return(KPS_PERFORM);
}

/*
*==========================================================================
**    Synopsis:   SHORT     KpsItemEdit(UCHAR uchPrintNo, KPSBUFFER *pBuffer, UCHAR uchPrinter, UCHAR pauchDownPrinter)
*
*       Input:    USHORT    uchPrintNo          * Printer No(1...8) to be printed*
*                 UCHAR     *auchRcvBuffer      * Pinter to data at ili.
*                 UCHAR     uchPrinter          * Printer No by application
*                 UCHAR     pauchDownPrinter    * Down Printer No *
*
*      Output:    UCHAR     puchReadPointer     * Read Pointer *
*       InOut:    Nothing
*
**  Return   :    KPS_PERFORM                   * normal end *
*                 KPS_PRINT_ERROR               * Printer error *
*
**  Description:
*               Edit 1 Item.
*==========================================================================
*/
SHORT KpsItemEdit(UCHAR uchPrinterNo, KPSBUFFER *pBuffer, UCHAR uchPrinter, UCHAR *pauchDownPriter)
{
    SHORT  sStatus = KPS_SUCCESS;
    UCHAR  uchFeed;

    /* check if during alternation printing */
    if (uchPrinterNo == 0){
        return KPS_PERFORM;
    }
    KpsShrInit();
    if (KPS_DOWN_PRINTER & pauchDownPriter[uchPrinterNo - 1]) { //SR456
        uchFeed = KPS_HEAD_FEED_RECOVER;
    } else {
        uchFeed = KPS_HEAD_FEED;
    }
    if (KPS_PRINT_ERROR == KpsPaperFeed(uchPrinterNo, uchFeed)) {
        /*uchKpsPrinterStatus &= ~(0x01 << (uchPrinterNo - 1));*/
        _KpsSetPrinterSts(uchPrinterNo,FALSE);/* ### MOD 2172 Rel1.0 */
        return(KPS_PRINT_ERROR);
    }
    if (KPS_DOWN_PRINTER & pauchDownPriter[uchPrinterNo - 1]) { //SR456
        /* --- print "////////" mnemonic on K/P */
        sStatus = KpsDownPrinterPrint(uchPrinterNo);
        pauchDownPriter[uchPrinterNo - 1] = 0;
        if (sStatus == KPS_PRINT_ERROR) {
            /*uchKpsPrinterStatus &= ~(0x01 << (uchPrinterNo - 1));*/
            _KpsSetPrinterSts(uchPrinterNo,FALSE);/* ### MOD 2172 Rel1.0 */
            return(sStatus);
        }
    }

    if (_KpsIsAlt(pBuffer->usOutPrinterInfo)) {
        sStatus = KpsAlt(uchPrinterNo, (UCHAR)(pBuffer->usOutPrinterInfo & 0x0ff));

        if(sStatus == KPS_PRINT_ERROR){
            /*uchKpsPrinterStatus &= ~(0x01 << (uchPrinterNo - 1));*/
            _KpsSetPrinterSts(uchPrinterNo,FALSE);/* ### MOD 2172 Rel1.0 */
            return(sStatus);
        }
    }

    if ((sStatus = KpsHeader(uchPrinterNo, pBuffer->hsKpsBuffFH, KPS_HEADER)) != KPS_PRINT_ERROR) {
        if ((sStatus = KpsItemPrint(uchPrinterNo, pBuffer->hsKpsBuffFH, pBuffer->ulReadPoint)) != KPS_PRINT_ERROR) {
            sStatus = KpsTrailer(uchPrinterNo, pBuffer->hsKpsBuffFH, (KPS_TRAILER * sizeof(TCHAR)));
        }
    }
    if (sStatus == KPS_PRINT_ERROR) {
        /*uchKpsPrinterStatus &= ~(0x01 << (uchPrinterNo - 1));*/
        _KpsSetPrinterSts(uchPrinterNo,FALSE);/* ### MOD 2172 Rel1.0 */
        return(sStatus);
    }
    KpsPaperCut(uchPrinterNo);
    return(KPS_PERFORM);
}
/*
*==========================================================================
**    Synopsis:   SHORT KpsItemPrint(UCHAR uchPrinterNo,
*                                    SHORT hsFileHandle,
*                                    ULONG ulReadPointer)
*
*       Input:    USHORT    uchPrinterNo            * Printer No to be printed by appl *
*                 SHORT     hsFileHandle            * file handle *
*                 USHORT    usReadPointer           * offset of read file *
*
*      Output:    UCHAR     Nothing
*       InOut:    Nothing
*
**  Return   :    KPS_PERFORM                       * normal end *
*                 KPS_PRINT_ERROR                   * Printer error *
*
**  Description:
*               Print 1 Item.
*==========================================================================
*/
static SHORT KpsItemPrint(UCHAR uchPrinterNo, SHORT hsFileHandle, ULONG ulReadPointer)
{
    TCHAR   * pBuffer = auchTmpBuff;
	ULONG	ulActualBytesRead;//RPH 11-10-3 SR# 201
    USHORT  cuchCode;
    USHORT  uchData;
    SHORT   sIli;
    SHORT   sStatus;

    /* get data from buffer file */

    //RPH 11-10-3 SR# 201
	KpsGetBuffFile(ulReadPointer, auchTmpBuff, sizeof(auchTmpBuff), hsFileHandle, &ulActualBytesRead);
	{              
		USHORT  usIli;         /* Ili */
		usIli = auchTmpBuff[0];      /* SAVE ILI containing number of bytes to be printed */
		sIli = usIli;
	}
    if (sIli <=  4) {     /* DATA IS NONE */
        /* pBuffer += uchIli; */
        return(KPS_PERFORM);
    }

    sIli -= 2;
    pBuffer++;              /* POINT TO PRINTER SELECTION */
    sIli -= 2;
    pBuffer++;              /* one more increment to skip printer control  R3.1 */

    /* CHECK VOID */
    if (*pBuffer & KPS_VOID_MASK) {  /* BIT ON */
        /* --- print void modifier mnemonic --- */
        if (KpsVoidPrint(uchPrinterNo) != KPS_PERFORM) {
            return(KPS_PRINT_ERROR);
        }
    }

    cuchCode = 0;
    /* SET W WIDE CODE */
    auchKpsPrintBuffer[cuchCode++] = KPS_ESC;       /* esc */
    auchKpsPrintBuffer[cuchCode++] = KPS_21;         /* ! */
    auchKpsPrintBuffer[cuchCode++] = KPS_W_WIDE;    /* w wide */

    /*********************** ADD R3.0 *********************/
    /* ### MOD Saratoga (042000) if (uchKpsPrinterType & (UCHAR)(0x01 << (uchPrinterNo - 1)))*/
    if(_KpsIsEpson(uchPrinterNo)){
        auchKpsPrintBuffer[cuchCode-1] |= KPS_FONT_B;    /* Font B */
    }
    /*********************** End ADD R3.0 ******************/

    sIli -= 2;
    pBuffer++;
    while(sIli > 0) {
        for (uchData = 0; (SHORT)(uchData * sizeof(TCHAR)) < sIli; ) {
            if (!uchData) {
                auchKpsPrintBuffer[cuchCode++] = KPS_ESC;       /* esc */
                auchKpsPrintBuffer[cuchCode++] = KPS_r;         /* r */
                if (KPS_RED_CODE == *pBuffer) {
                    auchKpsPrintBuffer[cuchCode++] = KPS_ESC_RED;      /* bit on is red */
                    pBuffer++;
                    sIli -= 2;
                } else {
                    auchKpsPrintBuffer[cuchCode++] = KPS_ESC_BLACK;    /* bit on is black */
                }
            }
            if (KPS_LF == *(pBuffer + uchData++)) {
                break;
            }
        }

		_tcsncpy(auchKpsPrintBuffer + cuchCode, pBuffer, uchData);

		if ((sStatus = KpsWriteCom(uchPrinterNo, auchKpsPrintBuffer, (UCHAR)(cuchCode + uchData))) != KPS_PERFORM) {
            return KPS_PRINT_ERROR;
        }
        sIli -= uchData * sizeof(TCHAR);
        pBuffer += uchData;
        cuchCode = 3;
    }
    return(KPS_PERFORM);
}
/*
*==========================================================================
**    Synopsis:   SHORT KpsAlt(UCHAR uchPrinterNo, UCHAR uchPrinter)
*
*       Input:    USHORT    uchPrinterNo           * Printer No to be printed by applcation *
*                 UCHAR     uchPrinter             * Printer No by Application *
*
*      Output:    UCHAR     Nothing
*       InOut:    Nothing
*
**  Return   :    KPS_PERFORM                   * normal end *
*                 KPS_PRINT_ERROR               * Printer error *
*
**  Description:
*               Print Alternation mark.
*==========================================================================
*/
SHORT KpsAlt(UCHAR uchPrinterNo, UCHAR uchPrinter)
{
    SHORT   sStatus;
    UCHAR   cuchWork = 0;

    /* SET W WIDE CODE */
    auchKpsPrintBuffer[cuchWork++] = KPS_ESC;       /* esc */
    auchKpsPrintBuffer[cuchWork++] = KPS_21;        /* ! */
    auchKpsPrintBuffer[cuchWork++] = KPS_W_WIDE;    /* w wide */

    /*********************** ADD R3.0 *********************/
    /* ### MOD Saratoga (042000) if (uchKpsPrinterType & (UCHAR)(0x01 << (uchPrinterNo - 1))) */
    if(_KpsIsEpson((SHORT)uchPrinterNo)){
        auchKpsPrintBuffer[cuchWork-1] |= KPS_FONT_B;    /* Font B */
    }
    /*********************** ADD R3.0 *********************/

    /* SET COLOR */
    auchKpsPrintBuffer[cuchWork++] = KPS_ESC;       /* esc */
    auchKpsPrintBuffer[cuchWork++] = KPS_r;         /* r */
    auchKpsPrintBuffer[cuchWork++] = KPS_ESC_BLACK; /* black */
    /* set alternaiton mark */
    _tcsnset(&auchKpsPrintBuffer[cuchWork], _T('@'), KPS_NUMBER_OF_ALTMARK);
    cuchWork += KPS_NUMBER_OF_ALTMARK;
    auchKpsPrintBuffer[cuchWork++] = KPS_SPACE;       /* space */
    auchKpsPrintBuffer[cuchWork++] = (UCHAR)(uchPrinter | 0x30);    /* Printer No */
    auchKpsPrintBuffer[cuchWork++] = KPS_SPACE;       /* space */
    _tcsnset(&auchKpsPrintBuffer[cuchWork], _T('@'), KPS_NUMBER_OF_ALTMARK);
    cuchWork += KPS_NUMBER_OF_ALTMARK;
    auchKpsPrintBuffer[cuchWork++] = KPS_LF;          /* LF */
    if ((sStatus = KpsWriteCom(uchPrinterNo, auchKpsPrintBuffer, cuchWork)) != KPS_PERFORM) {
        return KPS_PRINT_ERROR;
    }

    return(KPS_PERFORM);
}
/*
*==========================================================================
**    Synopsis:   SHORT KpsHeader(UCHAR uchPrinterNo,
*                                 SHORT hsFileHandle,
*                                 ULONG ulReadPointer)
*
*       Input:    USHORT    uchPrinterNo            * Printer No to be printed by appl *
*                 SHORT     hsFileHandle            * file handle *
*                 USHORT    usReadPointer           * offset of read file *
*
*      Output:    UCHAR     Nothing
*       InOut:    Nothing
*
**  Return   :    KPS_PERFORM                   * normal end *
*                 KPS_PRINT_ERROR               * Printer error *
*
**  Description:
*               Print Header.
*==========================================================================
*/
static SHORT  KpsHeader(UCHAR uchPrinterNo, SHORT hsFileHandle, ULONG ulReadPointer)
{
	TCHAR	*tchOperator;
    TCHAR   *puchHeaderAddr;
	ULONG	ulActualBytesRead;//RPH 11-10-3 SR# 201
    UCHAR   cuchByte;
    UCHAR   fbAllSpace;


    /* get data from buffer file */
	KpsTransactionPrint(uchPrinterNo, KPS_TRANS_BGN);

    //RPH 11-10-3 SR# 201
	KpsGetBuffFile(ulReadPointer, auchTmpBuff, sizeof(auchTmpBuff), hsFileHandle, &ulActualBytesRead);
                   
    puchHeaderAddr = auchTmpBuff;

    /* Print Out Operator Name R3.1 */
    for (cuchByte = 0, fbAllSpace = 1; cuchByte < KPS_OPERATOR_SIZE; cuchByte++) {
        if ((KPS_SPACE != *(((TCHAR*)puchHeaderAddr) + cuchByte)) && (0 != *(((TCHAR*)puchHeaderAddr) + cuchByte))) {
            fbAllSpace = 0;                              
            break;
        }
    }
    if (!fbAllSpace) {
        /* V3.3 */
		tchOperator = (TCHAR*)puchHeaderAddr;
        Kps_Print(uchPrinterNo, KPS_ESC_BLACK, KPS_W_WIDE, puchHeaderAddr, (UCHAR)_tcslen(tchOperator));
        /* Kps_Print(uchPrinterNo, KPS_ESC_BLACK, KPS_W_WIDE, puchHeaderAddr, KPS_OPERATOR_SIZE); */
    }

    /* Check Header Data */
    puchHeaderAddr += KPS_OPERATOR_SIZE;
    for (cuchByte = 0, fbAllSpace = 1; cuchByte < KPS_HEADER_SIZE; cuchByte++) {
        if ((KPS_SPACE != *(((TCHAR*)puchHeaderAddr) + cuchByte)) && (0 != *(((TCHAR*)puchHeaderAddr) + cuchByte))) {
            fbAllSpace = 0;                              
            break;
        }
    }
    if (!fbAllSpace) {
        Kps_Print(uchPrinterNo, KPS_ESC_BLACK, KPS_W_WIDE, puchHeaderAddr, KPS_HEADER_SIZE);
    }
    /* Check Customer Data */
    puchHeaderAddr += KPS_HEADER_SIZE;
    for (cuchByte = 0, fbAllSpace = 1; cuchByte < KPS_CUSTOMER_NAME_SIZE; cuchByte++) {
        if ((KPS_SPACE != *(((TCHAR*)puchHeaderAddr) + cuchByte)) && (0 != *(((TCHAR*)puchHeaderAddr) + cuchByte))) {
            fbAllSpace = 0;                              
            break;
        }
    }
    if (!fbAllSpace) {                                  /* V3.3 FVT#14 */
        Kps_Print(uchPrinterNo, KPS_ESC_BLACK, KPS_W_WIDE, puchHeaderAddr, KPS_CUSTOMER_NAME_SIZE);
    }

    /* --- One Space Line,  V3.3 FVT#14 --- */
    if (CliParaMDCCheck(MDC_KP2_ADR, EVEN_MDC_BIT0)) {
        if (KPS_PRINT_ERROR == KpsPaperFeed(uchPrinterNo, 1)) {
            /*uchKpsPrinterStatus &= ~(0x01 << (uchPrinterNo - 1));*/
            _KpsSetPrinterSts(uchPrinterNo,FALSE);/* ### MOD 2172 Rel1.0 */
            return(KPS_PRINT_ERROR);
        }
    }
    /* --- One Space Line,  V3.3 FVT#14 --- */

    return KPS_PERFORM;
}
/*
*==========================================================================
**    Synopsis:   SHORT KpsTrailer(UCHAR uchPrinterNo,
*                                  SHORT hsFileHandle,
*                                  ULONG ulReadPointer)
*
*       Input:    USHORT    uchPrinterNo            * Printer No to be printed by appl *
*                 SHORT     hsFileHandle            * file handle *
*                 USHORT    usReadPointer           * offset of read file *
*
*      Output:    UCHAR     Nothing
*       InOut:    Nothing
*
**  Return   :    KPS_PERFORM                   * normal end *
*                 KPS_PRINT_ERROR               * Printer error *
*
**  Description:
*               Print Trailer.
*==========================================================================
*/
static SHORT  KpsTrailer(UCHAR uchPrinterNo, SHORT hsFileHandle, ULONG ulReadPointer)
{
	ULONG	ulActualBytesRead;//RPH 11-11-3 SR# 201

    /* get data from buffer file */
    //RPH 11-11-3 SR# 201
	KpsGetBuffFile(ulReadPointer, auchTmpBuff, sizeof(auchTmpBuff), hsFileHandle, &ulActualBytesRead);
    return Kps_Print(uchPrinterNo, KPS_ESC_BLACK, KPS_S_WIDE, auchTmpBuff, KPS_TRAILER_SIZE);
}
/*
*==========================================================================
**    Synopsis:   SHORT KpsPaperCut(UCHAR uchPrinterNo)
*
*       Input:    USHORT    uchPrinterNo           * Printer No to be printed by appl *
*
*      Output:    UCHAR     Nothing
*       InOut:    Nothing
*
**  Return   :    KPS_PERFORM                   * normal end *
*                 KPS_PRINT_ERROR               * Printer error *
*
**  Description:
*               Cut Paper.
*==========================================================================
*/
SHORT KpsPaperCut(UCHAR uchPrinterNo)
{
    SHORT   sStatus;
	BOOL	fbDoublePrinting = FALSE;
    UCHAR   cuchCode = 0;

	fbDoublePrinting = CliParaMDCCheckField(MDC_DBLSIDEPRINT_ADR, MDC_PARAM_BIT_A);

	/*
		The shared printer must use the OPOS control escape 
		codes. The kitchen printer which is still using the 
		com port must use a different set of escape codes.If
		the OPOS control recieves the kitchen printer escape
		code it will not do a paper cut until the next 
		transaction. ESMITH
	*/
	if(_KpsIsSharedPrinter(uchPrinterNo))
	{
		auchKpsPrintBuffer[cuchCode++] = KPS_ESC;
		auchKpsPrintBuffer[cuchCode++] = '|';
		auchKpsPrintBuffer[cuchCode++] = '7';
		auchKpsPrintBuffer[cuchCode++] = 'l';
		auchKpsPrintBuffer[cuchCode++] = 'F';

		if(!fbDoublePrinting)
		{
			auchKpsPrintBuffer[cuchCode++] = KPS_ESC;
			auchKpsPrintBuffer[cuchCode++] = '|';
			auchKpsPrintBuffer[cuchCode++] = '7';       
			auchKpsPrintBuffer[cuchCode++] = '5';
			auchKpsPrintBuffer[cuchCode++] = 'P';
		}
	}
	else
	{
		auchKpsPrintBuffer[cuchCode++] = KPS_ESC;
		auchKpsPrintBuffer[cuchCode++] = KPS_d;
		auchKpsPrintBuffer[cuchCode++] = KPS_PAPER_CUT_FEED;       /* 7 line feed */
		//we remove this because we do the printer cut
		//in TransactionEnd call
		if(!fbDoublePrinting)
		{
			auchKpsPrintBuffer[cuchCode++] = KPS_ESC;               /* esc */
			auchKpsPrintBuffer[cuchCode++] = KPS_ESC_PARTIAL_CUT;   /* i */
		}
	}
    

	//we dont end the frame during double sided printing because we 
	//need to send the "TransactionPrint" function to actually tell
	//a printer with double sided printing to end the transaction JHHJ
    if(!fbDoublePrinting)
	{
		KpsShrInf.HdInf.fbFrame |= SHR_END_FRAME;              /* Add R3.0 */
	}

    if ((sStatus = KpsWriteCom(uchPrinterNo, auchKpsPrintBuffer, cuchCode)) != KPS_PERFORM) {
        return KPS_PRINT_ERROR;
    }

    return(KPS_PERFORM);
}
/*
*==========================================================================
**    Synopsis:   SHORT KpsTransactionPrint(UCHAR uchPrinterNo, UCHAR uchType)
*
*       Input:    USHORT    uchPrinterNo           * Printer No to be printed by appl *
*
*      Output:    UCHAR     Nothing
*       InOut:    Nothing
*
**  Return   :    KPS_PERFORM                   * normal end *
*                 KPS_PRINT_ERROR               * Printer error *
*
**  Description:
*               Send a message to the printer to begin or end a, transaction if
*				the MDC for double sided printing is set to ON
*==========================================================================
*/
SHORT KpsTransactionPrint(UCHAR uchPrinterNo, UCHAR uchType)
{
	
	if(CliParaMDCCheckField(MDC_DBLSIDEPRINT_ADR, MDC_PARAM_BIT_A))
	{
		TCHAR	aszFormatString[20] = {0};

		aszFormatString[0] = uchType;
		aszFormatString[1] = KPS_KITCHENPRINTING;

		if(uchType == KPS_TRANS_END)
		{
			KpsShrInf.HdInf.fbFrame |= SHR_END_FRAME;              /* Add R3.0 */
		}

		Kps_Print(uchPrinterNo, KPS_ESC_BLACK, KPS_S_WIDE, aszFormatString, (UCHAR)_tcslen(aszFormatString));
	}

    return(KPS_PERFORM);
}
/*
*==========================================================================
**    Synopsis:   SHORT KpsVoidPrint(UCHAR uchPrinterNo)
*
*       Input:    UCHAR uchPrintNO
*                 UCHAR uchColor        1 = red
*                                       0 = black
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    KPS_PERFORM
*                 KPS_PRINT_ERROR
*
**  Description:
*               Print Void.
*==========================================================================
*/
SHORT KpsVoidPrint(UCHAR uchPrinterNo)
{
    TCHAR  aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = { 0 }; /* PARA_... defined in "paraequ.h" */
    UCHAR  cuchData;

    /* SET VOID MODIFIER */
    RflGetSpecMnem(aszSpecMnem, SPC_VOID_ADR);
    for (cuchData = 0; cuchData < PARA_SPEMNEMO_LEN; cuchData++ ) {
		if (aszSpecMnem[cuchData] == 0) {
            break;
        }
		auchKpsPrintBuffer[cuchData] = aszSpecMnem[cuchData];
    }
    return Kps_Print(uchPrinterNo, KPS_ESC_RED, KPS_W_WIDE, auchKpsPrintBuffer, cuchData);
}
/*
*==========================================================================
**    Synopsis:   VOID KpsMakeOutPutPrinter(KPSBUFFER *pKpsBuffer, UCHAR *puchKpsPrinterStatus)
*
*       Input:    KPSBUFFER *pKpsBuffer
*
*      Output:    UCHAR     *puchKpsPrinterStatus
*       InOut:    Nothing
*
**  Return   :    
*                 
**  Description:
*               Make Output Printer Table.
*==========================================================================
*/
VOID KpsMakeOutPutPrinter(KPSBUFFER *pKpsBuffer, UCHAR *puchPrinterStatus)
{
    UCHAR   auchManuAlt[KPS_NUMBER_OF_PRINTER];
    UCHAR   auchAutoAlt[KPS_NUMBER_OF_PRINTER];
    UCHAR   cuchI;

    ParaManuAltKitchReadAll(auchManuAlt);
    ParaAutoAltKitchReadAll(auchAutoAlt);

    /* Check Manual Alternation */
    /* *puchPrinterStatus = uchKpsPrinterStatus;*/
    *puchPrinterStatus = (UCHAR)_KpsPortSts2PrtSts(); /* clear output status table *//* ### MOD 2172 Rel1.0 */
    for (cuchI = 0; cuchI < KPS_NUMBER_OF_PRINTER; cuchI++) {
        if (KPS_CHK_KPNUMBER(auchManuAlt[cuchI])) {
            /*if (0 == (uchKpsPrinterStatus & (0x01 << (UCHAR)(auchManuAlt[cuchI] - 1))) ) */
            if (!_KpsIsEnable((SHORT)auchManuAlt[cuchI]) ) { /* ### MOD 2172 Rel1.0 */
                pKpsBuffer->auchOutputPrinter[cuchI] = 0;
            } else {
                /* --- execute manual alternation print --- */
                pKpsBuffer->auchOutputPrinter[cuchI] = auchManuAlt[cuchI];
                pKpsBuffer->auchOutputPrinter[cuchI] |= 0x80;   /* Manual alternation bit id */
            }
        } else {    /* manual alternation is not specified */
            /* --- determine this printer is POWER DOWN --- */
            /* if (0 == (uchKpsPrinterStatus & (0x01 << (pKpsBuffer->auchOutputPrinter[cuchI] - 1)))) */
            /***if (0 == ((*puchPrinterStatus) & (0x01 << (pKpsBuffer->auchOutputPrinter[cuchI] - 1)))) @@@***/ /* ### MOD 2172 Rel1.0 */
            if (!_KpsIsEnable( (SHORT)(pKpsBuffer->auchOutputPrinter[cuchI] & 0x0F) ) ) { /* ### MOD 2172 Rel1.0 */
                /* --- determine auto alternation is specified --- */
                /* if ((0 < auchAutoAlt[cuchI]) & ((auchAutoAlt[cuchI] - 1) < (KPS_NUMBER_OF_PRINTER + 1))) */
                if ((0 < auchAutoAlt[cuchI]) && (auchAutoAlt[cuchI] <= KPS_NUMBER_OF_PRINTER)) { /* ### MOD 2172 Rel1.0 (02/23/00) */
                    /*if (0 == (uchKpsPrinterStatus & (0x01 << (auchAutoAlt[cuchI] - 1))))*/
                    /***if (0 == ((*puchPrinterStatus) & (0x01 << (auchAutoAlt[cuchI] - 1)))) @@@***/ /* ### MOD 2172 Rel1.0 */
                    if (!_KpsIsEnable((SHORT)(auchAutoAlt[cuchI])) ) { /* ### MOD 2172 Rel1.0 */
                        pKpsBuffer->auchOutputPrinter[cuchI] = 0;
                        continue;
                    } else {
                        /* --- execute auto alternation print --- */
                        pKpsBuffer->auchOutputPrinter[cuchI] = auchAutoAlt[cuchI];
                    }
                } else {    /* auto alternation is not specified */
                    pKpsBuffer->auchOutputPrinter[cuchI] = 0;
                }
            }
        }
        /* make printer status table */
        if (pKpsBuffer->auchOutputPrinter[cuchI] != 0) {
            *puchPrinterStatus |= (0x01 << cuchI);
        } else {
            *puchPrinterStatus &= ~(0x01 << cuchI);
        }
    }
}
/*
*==========================================================================
**    Synopsis:   SHORT Kps_Print(UCHAR uchPrinterNo, UCHAR uchColor, UCHAR uchChrtype,
*                                 UCHAR *puchDataAddress, UCHAR uchSize)
*
*       Input:    UCHAR     uchPrinterNo
*                 UCHAR     uchColor
*                 UCHAR     uchChrtype
*                 UCHAR     *puchDataAddress
*                 UCHAR     uchSize
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    
*                 
**  Description:
*               Print Data to kitchin printer.
*==========================================================================
*/
SHORT Kps_Print(UCHAR uchPrinterNo, UCHAR uchColor, UCHAR uchChrtype, TCHAR *puchDataAddress, UCHAR uchSize)
{
	TCHAR	tchPrintBuffer[256] = {0};
    UCHAR   cuchCode = 0;
                            
    /* Set Character Type */
    tchPrintBuffer[cuchCode++] = KPS_ESC;          /* esc */
    tchPrintBuffer[cuchCode++] = KPS_21;           /* ! */
    tchPrintBuffer[cuchCode++] = uchChrtype;       /* Charcter Type */

    /*********************** ADD R3.0 *********************/
    /* ### MOD Saratoga (042000) if (uchKpsPrinterType & (UCHAR)(0x01 << (uchPrinterNo - 1))) */
    if(_KpsIsEpson((SHORT)uchPrinterNo)){
        tchPrintBuffer[cuchCode-1] |= KPS_FONT_B;    /* Font B */
    }
    /*********************** ADD R3.0 *********************/

    /* SET AND CHECK COLOR */
    tchPrintBuffer[cuchCode++] = KPS_ESC;          /* esc */
    tchPrintBuffer[cuchCode++] = KPS_r;            /* r */
    tchPrintBuffer[cuchCode++] = uchColor;         /* color */

    memcpy(&tchPrintBuffer[cuchCode], puchDataAddress, uchSize * sizeof(TCHAR));
    tchPrintBuffer[uchSize + cuchCode++] = KPS_LF;     /* LF */

	if (KpsWriteCom(uchPrinterNo, tchPrintBuffer, (UCHAR)(uchSize + cuchCode)) != KPS_PERFORM) {
        return (KPS_PRINT_ERROR);
    }
    return(KPS_PERFORM);
}
/*
*==========================================================================
**    Synopsis:   VOID KpsAltCheck(UCHAR uchPrinterNo)
*
*       Input:    UCHAR     uchPrintNo          * Printer No 0,1,2,3 *
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    
*                 
**  Description:
*               Make Output Printer Table Individual when error occurred.
*==========================================================================
*/
VOID KpsAltCheck(UCHAR uchPrinterNo, KPSBUFFER *pKpsBuffer)
{
    UCHAR   auchAutoAlt[KPS_NUMBER_OF_PRINTER] = { 0 };

    ParaAutoAltKitchReadAll(auchAutoAlt);

    /* --- determine manual alternation is specified --- */
    if (0x80 == (pKpsBuffer->auchOutputPrinter[uchPrinterNo] & 0xf0)) {
        /* --- execute manual alternation print --- */
        pKpsBuffer->auchRequestRecover[uchPrinterNo] = (UCHAR)(pKpsBuffer->auchOutputPrinter[uchPrinterNo]);
        pKpsBuffer->auchOutputPrinter[uchPrinterNo] = 0;
    } else if (KPS_CHK_KPNUMBER(auchAutoAlt[uchPrinterNo])) {
        /* --- determine auto alternation is specified --- */

        /* --- Is now printing auto alternation print ? --- */
        if (pKpsBuffer->auchOutputPrinter[uchPrinterNo] == auchAutoAlt[uchPrinterNo]) {
            pKpsBuffer->auchRequestRecover[uchPrinterNo] = pKpsBuffer->auchOutputPrinter[uchPrinterNo];
            pKpsBuffer->auchOutputPrinter[uchPrinterNo] = 0;
        } else {
            /*if (0 != (uchKpsPrinterStatus & (0x01 << (auchAutoAlt[uchPrinterNo] - 1)))) */
            if (g_uchPortStatus & (UCHAR)(0x01 << (_KpsKPNo2ComNo(auchAutoAlt[uchPrinterNo]) - 1))) {
                /* --- Application has not executed auto alternation print yet,
                    and then it executes alternation print --- */
                pKpsBuffer->auchOutputPrinter[uchPrinterNo] = auchAutoAlt[uchPrinterNo];
            } else {
                pKpsBuffer->auchRequestRecover[uchPrinterNo] = pKpsBuffer->auchOutputPrinter[uchPrinterNo];
                pKpsBuffer->auchOutputPrinter[uchPrinterNo] = 0;
            }
        }
    } else {    /* manual/auto alternation is not specified */
        pKpsBuffer->auchRequestRecover[uchPrinterNo] = pKpsBuffer->auchOutputPrinter[uchPrinterNo];
        pKpsBuffer->auchOutputPrinter[uchPrinterNo] = 0;
    }
}

VOID  KpsDisplayPrtError(VOID)
{
    USHORT     usPrevious;	
	
	/* allow power switch at error display */
	usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE);
	UieErrorMsg(LDT_KTNERR_ADR, UIE_WITHOUT);
	PifControlPowerSwitch(usPrevious);
	
	flDispRegDescrControl |= PRT_OFFLINE_CNTRL;
	flDispRegKeepControl |= PRT_OFFLINE_CNTRL;

	/* Display Error and turn on descriptor */
	RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_1); 	//SR 236 Dummy Data for calling the descriptor
}

VOID  KpsClearPrtError(VOID)
{
	SHORT nPortCnt;

	for (nPortCnt = 0; nPortCnt < KPS_NUMBER_OF_PRINTER; nPortCnt++) {
		if (auchKpsDownPrinter[nPortCnt] & (UCHAR)KPS_DOWN_PRINTER) 
			return;
	}

	flDispRegDescrControl &= ~PRT_OFFLINE_CNTRL;
	flDispRegKeepControl &= ~PRT_OFFLINE_CNTRL;
}
/*
*==========================================================================
**    Synopsis:   SHORT  KpsWriteCom(UCHAR  uchPrinterNo, UCHAR *puchBuffer, UCHAR  cuchCode)
*
*       Input:    UCHAR     uchPrinterNo    * Printer No    1,2,3,4 *
*                 VOID      *pBuffer        * Pointer of Data to be written *
*                 USHORT    usBytes         * Number of bytes *
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :
*               KPS_PERFORM                 * Normal End *
*               KPS_PRINT_ERROR             * Port Open Error *
*                 
**  Description: Print output to either local Kitchen Printer or shared kitchen printer.
*               
*==========================================================================
*/
SHORT KpsWriteCom(UCHAR uchPrinterNo, TCHAR *puchBuffer, UCHAR cuchCode)
{
    SHORT   sStatus = KPS_PERFORM;

    if (_KpsIsSharedPrinter(uchPrinterNo)) {
		USHORT  usFlag = 0;
		USHORT  usRetryCounter;
		USHORT  i;
		TCHAR   utcTmpKpBuffer[KPS_PRINT_BUFFER_SIZE];

        /* case of shared printer */
        for (i=0; i < cuchCode; i += 3) {
            if (*(puchBuffer+i) != KPS_ESC) {
                break;
            }
            if (*(puchBuffer+i+1) == KPS_21) {
                if (*(puchBuffer+i+2) & KPS_W_WIDE) {
                    usFlag = 1;
                }
                continue;
            }
            if (*(puchBuffer+i+1) != KPS_r) {
                break;
            }
        }
        cuchCode -= i;
        puchBuffer += i;
        if (usFlag) {
			USHORT  j;

			for (i = j = 0; i < cuchCode; i++) {
				if ( *(puchBuffer + i) == KPS_LF) {
					utcTmpKpBuffer[j] = KPS_LF;
					j++;
				} else {
					utcTmpKpBuffer[j] = KPS_DOUBLE;
					j++;
					utcTmpKpBuffer[j] = *(puchBuffer + i);
					j++;
				}
			}
			NHPOS_ASSERT(j*sizeof(TCHAR) < sizeof(KpsShrInf.auchPrtData)); // RJC Test

            KpsShrInf.auchPrtData[0] = (UCHAR)(j*sizeof(TCHAR)); //ESMITH EJ PRINT    /* set print data length */
			memcpy (KpsShrInf.auchPrtData+1, utcTmpKpBuffer, KpsShrInf.auchPrtData[0]);
            KpsShrInf.HdInf.usILI += KpsShrInf.auchPrtData[0];
            KpsShrInf.HdInf.usILI++;       /* '+ 1' is for data length area */        
        } else {
            /*
				Set the number of bytes in the string of characters (remember this is UNICODE)
			    and then copy the print data to the buffer used to send the print line to the
			    terminal with the shared printer we are using.
			 */
			NHPOS_ASSERT(cuchCode * sizeof(TCHAR) < sizeof(KpsShrInf.auchPrtData)); // RJC Test

            KpsShrInf.auchPrtData[0] = (cuchCode * sizeof(TCHAR)); //ESMITH EJ PRINT   /* set print data length */
			memcpy(&KpsShrInf.auchPrtData[1], puchBuffer, KpsShrInf.auchPrtData[0]);
            KpsShrInf.HdInf.usILI += KpsShrInf.auchPrtData[0];
			KpsShrInf.HdInf.usILI++;        /* '+ 1' is for data length area */        
        }
        
        for (usRetryCounter = 0; usRetryCounter < KPS_RETRY_COUNTER; usRetryCounter++) {
            sStatus = CliShrPrintKP((UCHAR)_KpsGetMySharedPrinterNo(uchPrinterNo), &KpsShrInf, KpsShrInf.HdInf.usILI );
            if (sStatus != SHR_BUFFER_FULL) {
                break;
            }
			PifSleep (50);
        }
        if (KpsShrInf.HdInf.fbFrame & SHR_END_FRAME) {
            if (sStatus == SHR_PERFORM) {
                sStatus = KPS_PERFORM;
            } else {
                sStatus = KPS_PRINT_ERROR;
            }
        } else {
            if (sStatus == SHR_TIMERON) {
                sStatus = KPS_PERFORM;
            } else {
                sStatus = KPS_PRINT_ERROR;
            }
        }
        if (sStatus == KPS_PERFORM) {
            KpsShrInf.HdInf.fbFrame = SHR_CONTINUE_FRAME;                         
            KpsShrInf.HdInf.uchSeqNo ++;
            KpsShrInf.HdInf.usILI = sizeof(SHRHEADERINF);      /* reset current data length */
        }                                        
        return(sStatus);
	} else {
		SYSCONFIG CONST *pSysConfig = PifSysConfig();
		SHORT   nPortNo;
		USHORT  usTimeOut;  /* PifWriteCom time out value */
		USHORT  usRetryCounter;
		UCHAR	uchTmpKPBuff[KPS_PRINT_BUFFER_SIZE*2];
		UCHAR   auchPrintBuffer[9];

		/* case of local kitchen printer */
		/* get port number and check it */
		nPortNo = _KpsKPNo2ComNo((SHORT)uchPrinterNo);  /* ### ADD 2172 Rel1.0 */
		if (nPortNo <= 0)
			return KPS_PRINT_ERROR;

		auchPrintBuffer[0] = KPS_ESC;       /* DUMMY */
		auchPrintBuffer[1] = KPS_7B;        /* DUMMY */
		auchPrintBuffer[2] = KPS_0;         /* DUMMY */

		auchPrintBuffer[3] = KPS_ESC;       
		auchPrintBuffer[4] = KPS_INIT;

		auchPrintBuffer[5] = KPS_ESC;
		auchPrintBuffer[6] = KPS_NO_PAPER;
		auchPrintBuffer[7] = '4';
		auchPrintBuffer[8] = KPS_NO_PAPER_STOP;
	    
		/*if (g_hasKpsPort[uchPrinterNo - 1] < 0) */
		if (g_hasKpsPort[nPortNo - 1] < 0) { /* ### MOD 2172 rel1.0 */
			return KPS_PRINT_ERROR;
		}

		/* reset send timeout value from monitor-send-timeout to print-data-send-timeout */
		usTimeOut = KPS_PRTDATA_SEND_TIME_OUT;
		sStatus = PifControlNetComIoEx (g_hasKpsPort[nPortNo - 1], PIF_COM_SEND_TIME, usTimeOut);
		if (sStatus != PIF_OK) {
			return KPS_PRINT_ERROR;
		}

		for (usRetryCounter = 0; usRetryCounter < KPS_RETRY_COUNTER; usRetryCounter++) {
			if (sStatus >= 0) {
				int		nSize;
				/*sStatus = PifWriteNetComIoEx(hasKpsPort[uchPrinterNo - 1], puchBuffer, cuchCode);*/
				nSize = WideCharToMultiByte(pSysConfig->unCodePage, 0, puchBuffer, cuchCode, uchTmpKPBuff, sizeof(uchTmpKPBuff), NULL, NULL);
				sStatus = PifWriteNetComIoEx(g_hasKpsPort[nPortNo - 1], uchTmpKPBuff, (USHORT)nSize);/* ### MOD 2172 rel1.0 */
				if (sStatus >= 0) {
					return KPS_PERFORM;
				}
			}
			if ((sStatus == PIF_ERROR_COM_POWER_FAILURE) || (sStatus == PIF_ERROR_COM_ACCESS_DENIED)) {
				/*if (KpsPowerDownRecover((UCHAR)(uchPrinterNo - 1)) != KPS_PERFORM)*/
				if (KpsPowerDownRecover(uchPrinterNo) != KPS_PERFORM) {/* Retry *//* ### MOD 2172 Rel1.0 */
					return KPS_PRINT_ERROR;
				}
				sStatus = KPS_PERFORM;
				continue;
			}
			if ((sStatus == PIF_ERROR_COM_TIMEOUT) || (sStatus == PIF_ERROR_COM_NO_DSR)) {
				sStatus = PifWriteNetComIoEx(g_hasKpsPort[nPortNo - 1], auchPrintBuffer, 9);
				if (sStatus >= 0) {
					break;
				}
				continue;
			} else {
				return KPS_PRINT_ERROR;
			}
		}

		KpsDisplayPrtError();
	}

    return KPS_PRINT_ERROR;
}
/*
*==========================================================================
**    Synopsis:   SHORT  KpsPowerDownRecover(UCHAR uchReqPrinter)
*
*       Input:    
*                 UCHAR uchReqPrinter        * Printer No = 1...8 *
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :
*               KPS_PERFORM                  * SUCCESS *
*               KPS_OPEND_ERROR              * Port Open Error *
*                 
**  Description:
*               Power Down Recovery.
*               This Program is called by detecting Power Down.
*==========================================================================
*/
SHORT KpsPowerDownRecover(UCHAR uchReqPrinter)
{
    SHORT   hsNewPort;
    SHORT   sStatus;
    UCHAR   uchMask;
    UCHAR   uchPortCnt;
    UCHAR   cuchCode;
    UCHAR   auchPrintBuffer[KPS_PRINT_BUFFER_SIZE*2];
    CHAR    auchPortStsVal[KPS_NUMBER_OF_PRINTER] = { 0 }; /* ### MOD 2172 Rel1.0 (auchPrinterNo) */

    if (KpsWaitPowerDown() != KPS_PERFORM) {
        return KPS_OPEND_ERROR;
    }

    for (uchPortCnt = 0; uchPortCnt < KPS_NUMBER_OF_PRINTER; uchPortCnt++) {
		PROTOCOL Protocol = {0};
		USHORT   usPortId = g_auchKpsSysConfig[uchPortCnt];

        if (g_hasKpsPort[uchPortCnt] < 0) {
            continue;
        }

        /********************** Add R3.0 **************************/
        if (g_auchKpsSysConfig[uchPortCnt] == KPS_SHRED_PRINTER) {
            continue;
        }
        /********************** Add R3.0 **************************/

        PifCloseNetComIoEx(g_hasKpsPort[uchPortCnt]);
        /*uchMask = (UCHAR)(0x01 << (uchPrinterNo));*/
        uchMask = (UCHAR)(0x01 << uchPortCnt);

		KpsSetProtocol(usPortId, &Protocol);      /* Set protocol data */

		if ((hsNewPort = PifOpenNetComIoEx(usPortId, &Protocol)) < 0) {
            g_hasKpsPort[uchPortCnt] = KPS_OPEND_ERROR;
            /*uchKpsPrinterStatus &= ~uchMask;*//* set printer status is down */
            g_uchPortStatus &= ~uchMask; /* ### MOD 2172 Rel1.0 */
			auchKpsDownPrinter[uchPortCnt] = KPS_DOWN_AND_TROUBLE; //SR456
            auchPortStsVal[uchPortCnt] = KPS_OPEND_ERROR;
            continue;
        }

        g_hasKpsPort[uchPortCnt] = hsNewPort;
        /* uchKpsPrinterStatus |= uchMask; */ /* set printer status is up */
        g_uchPortStatus |= uchMask; /* ### MOD 2172 Rel1.0 */
		auchKpsDownPrinter[uchPortCnt] = KPS_NOT_DOWN_PRINTER; //SR456
        g_hasKpsPort[uchPortCnt] = hsNewPort;

        /* Make Power Down Print Format */
        cuchCode = 0;
        /* DUMMY */
        auchKpsPrintBuffer[cuchCode++] = KPS_ESC;   /* DUMMY */
        auchKpsPrintBuffer[cuchCode++] = KPS_7B;    /* DUMMY */
        auchKpsPrintBuffer[cuchCode++] = KPS_0;     /* DUMMY */
        /* SET INIT CODE */
        auchPrintBuffer[cuchCode++] = KPS_ESC;
        auchPrintBuffer[cuchCode++] = KPS_INIT;
        /* SET STOP AT NO PAPER CODE */
        auchPrintBuffer[cuchCode++] = KPS_ESC;
        auchPrintBuffer[cuchCode++] = KPS_NO_PAPER;
        auchPrintBuffer[cuchCode++] = '4';
        auchPrintBuffer[cuchCode++] = KPS_NO_PAPER_STOP;

        /* Check Power Down if during printing */
        if (uchKpsPrintDown == KPS_PRINT_DOWN) {
            /*if (uchReqPrinter == uchPrinterNo) */
            if (uchReqPrinter == _KpsComNo2KPNo((SHORT)(uchPortCnt + 1))) {/* if Printer is printing Printer *//* ### MOD 2172 Rel1.0 */
                /* SET W WIDE CODE */
                auchPrintBuffer[cuchCode++] = KPS_ESC;       /* esc */
                auchPrintBuffer[cuchCode++] = KPS_21;        /* ! */
                auchPrintBuffer[cuchCode++] = KPS_W_WIDE;    /* w wide */

                /*********************** ADD R3.0 *********************/
                /* ### MOD Saratoga (042000) if (uchKpsPrinterType & (UCHAR)(0x01 << uchPrinterNo)) */
                if(_KpsIsEpson((SHORT)uchReqPrinter)){
                    auchPrintBuffer[cuchCode-1] |= KPS_FONT_B;    /* Font B */
                }
                /*********************** ADD R3.0 *********************/

                /* SET COLOR */
                auchPrintBuffer[cuchCode++] = KPS_ESC;       /* esc */
                auchPrintBuffer[cuchCode++] = KPS_r;         /* r */
                auchPrintBuffer[cuchCode++] = KPS_ESC_BLACK; /* black */
                memset(&auchPrintBuffer[cuchCode], '/', KPS_NUMBER_OF_POWER_DOWN_MARK);
                cuchCode += KPS_NUMBER_OF_POWER_DOWN_MARK;
                auchPrintBuffer[cuchCode++] = KPS_LF;          /* LF */
            }
        }
        if ((sStatus = PifWriteNetComIoEx(hsNewPort, auchPrintBuffer, cuchCode)) < 0) {
           if ((sStatus == PIF_ERROR_COM_POWER_FAILURE) || (sStatus == PIF_ERROR_COM_ACCESS_DENIED)) {
                PifCloseNetComIoEx(hsNewPort);
                g_hasKpsPort[uchPortCnt] = KPS_OPEND_ERROR;
            }
            /*uchKpsPrinterStatus &= ~uchMask;*//* set printer status is down */
            g_uchPortStatus &= ~uchMask; /* ### MOD 2172 Rel1.0 */
			auchKpsDownPrinter[uchPortCnt] = KPS_DOWN_AND_TROUBLE; //SR456
            auchPortStsVal[uchPortCnt] = KPS_OPEND_ERROR;
            continue;
        }
        /* uchKpsPrinterStatus |= uchMask; *//* set printer status is up */
        g_uchPortStatus |= uchMask; /* ### MOD 2172 Rel1.0 */
		auchKpsDownPrinter[uchPortCnt] = KPS_NOT_DOWN_PRINTER; //SR456
        auchPortStsVal[uchPortCnt] = KPS_PERFORM;
        continue;
    }
    /*if (auchPrinterNo[uchReqPrinter] == KPS_PERFORM) */
    if(0 < (uchPortCnt = (UCHAR)_KpsKPNo2ComNo(uchReqPrinter))){
        if (auchPortStsVal[uchPortCnt - 1] == KPS_PERFORM) { /* ### MOD 2172 rel1.0 */
            return KPS_PERFORM;
        }
    }
    return KPS_OPEND_ERROR;
}
/*
*==========================================================================
**    Synopsis:   SHORT  KpsDownPrinterPrint(VOID)
*
*       Input:    Nothing
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :
*               KPS_PERFORM 
*               KPS_PRINT_ERROR              * Port Open Error *
*                 
**  Description:
*               Print Down Printer Mark " ///////....// ".
*==========================================================================
*/
SHORT  KpsDownPrinterPrint(UCHAR uchPrinterNo)
{
    UCHAR   cuchCode = 0;
    
    auchKpsPrintBuffer[cuchCode++] = KPS_ESC;       /* esc */
    auchKpsPrintBuffer[cuchCode++] = KPS_21;        /* ! */
    auchKpsPrintBuffer[cuchCode++] = KPS_S_WIDE;    /* single wide */

    /*********************** ADD R3.0 *********************/
    /* ### MOD Saratoga (042000) if (uchKpsPrinterType & (UCHAR)(0x01 << (uchPrinterNo - 1))) */
    if(_KpsIsEpson((SHORT)uchPrinterNo)){
        auchKpsPrintBuffer[cuchCode-1] |= KPS_FONT_B;    /* Font B */
    }
    /*********************** ADD R3.0 *********************/

    /* SET COLOR */                     
    auchKpsPrintBuffer[cuchCode++] = KPS_ESC;       /* esc */
    auchKpsPrintBuffer[cuchCode++] = KPS_r;         /* r */
    auchKpsPrintBuffer[cuchCode++] = KPS_ESC_BLACK; /* black */
	_tcsnset(&auchKpsPrintBuffer[cuchCode], _T('/'), KPS_NUMBER_OF_DOWN_PRINTER_MARK);
    cuchCode += KPS_NUMBER_OF_DOWN_PRINTER_MARK;
    auchKpsPrintBuffer[cuchCode++] = KPS_LF;          /* LF */
    if (KpsWriteCom(uchPrinterNo, auchKpsPrintBuffer, cuchCode) != KPS_PERFORM) {
        return KPS_PRINT_ERROR;
    }
    return(KPS_PERFORM);
}
/*
*==========================================================================
**    Synopsis:   SHORT  KpsPaperFeed(UCHAR uchPrinterNo, UCHAR uchFeed)
*
*       Input:    uchPrinterNo               * priter No to be feed *
*                 uchFeed                    * Feed Value *
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :
*               KPS_PERFORM 
*               KPS_PRINT_ERROR              * Port Open Error *
*                 
**  Description:
*               Paper feed (in order KPS_HEAD_FEED).
*==========================================================================
*/
SHORT  KpsPaperFeed(UCHAR uchPrinterNo, UCHAR uchFeed)
{
    UCHAR   cuchCode = 0;
    SHORT   sStatus  = KPS_PERFORM;

    if (!(CliParaMDCCheck(MDC_KPTR_ADR, ODD_MDC_BIT2)) || (uchFeed == KPS_HEAD_FEED_RECOVER)) {
        auchKpsPrintBuffer[cuchCode++] = KPS_ESC;
        auchKpsPrintBuffer[cuchCode++] = KPS_d;
        auchKpsPrintBuffer[cuchCode++] = uchFeed;
        if (KpsWriteCom(uchPrinterNo, auchKpsPrintBuffer, cuchCode) != KPS_PERFORM) {
            sStatus = KPS_PRINT_ERROR;
        }
    }
    return (sStatus);
}
/*
*==========================================================================
**    Synopsis:   VOID  *KpsQueTransfer(KPSQUE *KpsDeque, KPSQUE *KpsEnque)
*
*       Input:    KpsDeque               * remove que *
*                 KpsEnque               * add que *
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Deque address
*                 
**  Description:
*                 remove deque to enque.
*==========================================================================
*/
VOID  *KpsQueTransfer(KPSQUE *KpsDeque, KPSQUE *KpsEnque)
{
    KPSQUE  KpsReturnQue;

    KpsReturnQue.pFirstQue = KpsDeque->pFirstQue;

    if (KpsDeque->pFirstQue != NULL) {
        if (KpsEnque->pLastQue == NULL) {
            KpsEnque->pLastQue = KpsDeque->pFirstQue;
            KpsEnque->pFirstQue = KpsDeque->pFirstQue;
        } else {
            KpsEnque->pLastQue->pNextQue = KpsDeque->pFirstQue;
            KpsEnque->pLastQue = KpsDeque->pFirstQue;
        }
        KpsDeque->pFirstQue = KpsDeque->pFirstQue->pNextQue;    /* deque */
        KpsEnque->pLastQue->pNextQue = NULL;    /* KpsDeque->pFirstQue */
        if (KpsDeque->pFirstQue == NULL) {
            KpsDeque->pLastQue = NULL;    
        }
    }
    return KpsReturnQue.pFirstQue;
}
/*
*==========================================================================
**    Synopsis:   VOID  KpsClearRecoveryQue(VOID)  
*
*       Input:    KpsDeque               * remove que *
*                 KpsEnque               * add que *
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Description:
*                 Clear Recovery que .
*==========================================================================
*/
VOID  KpsClearRecoveryQue(VOID)
{
    SHORT   sI;
    KPSBUFFER   *pBuffer;

    for (sI = 0; sI < KPS_NUMBER_QUE; sI++) {
        if (NULL == (pBuffer = KpsQueTransfer(&KpsRcvQue, &KpsEmptyQue))) {
            break;
        } else {
            pBuffer->ulWritePoint = 0;
            pBuffer->ulReadPoint  = 0;
            pBuffer->auchRequestRecover[0] = 0;
            pBuffer->auchRequestRecover[1] = 0;
            pBuffer->auchRequestRecover[2] = 0;
            pBuffer->auchRequestRecover[3] = 0;

            /* ===== Increase No. of Remote Printer (Release 3.1) BEGIN ===== */
            pBuffer->auchRequestRecover[4] = 0;
            pBuffer->auchRequestRecover[5] = 0;
            pBuffer->auchRequestRecover[6] = 0;
            pBuffer->auchRequestRecover[7] = 0;
            /* ===== Increase No. of Remote Printer (Release 3.1) END ===== */
        }
    }
    PifLog(MODULE_KPS, LOG_ERROR_KPS_BUFFER_CLEAR);
}
/*
*==========================================================================
**    Synopsis:   SHORT  KpsRecoveryCheckAndInit(KPSQUE *KpsDeque, KPSQUE *KpsEnque)
*
*       Input:    KpsDeque               * remove que *
*                 KpsEnque               * add que *
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Recovery happend or not
*
*                 KPS_PERFORM       :   Recovery not happend
*                 KPS_PRINT_ERROR   :   Recovery happend
*                 
**  Description:
*                 Check Recovery and Init que.
*==========================================================================
*/
SHORT  KpsRecoveryCheckAndInit(KPSQUE *pque)
{
    SHORT   sI;
    for (sI = 0; sI < KPS_NUMBER_OF_PRINTER; sI++) {
        if (pque->pFirstQue->auchRequestRecover[sI] != 0) {
            pque->pFirstQue->ulReadPoint  = 0;
            if (pque->pFirstQue != KpsRcvQue.pFirstQue) {
                KpsQueTransfer(pque, &KpsRcvQue);
            }
            return KPS_PRINT_ERROR;
            break;
        }
    }
    pque->pFirstQue->ulWritePoint = 0;
    pque->pFirstQue->ulReadPoint  = 0;
    return KPS_PERFORM;
}
/*
*==========================================================================
**    Synopsis:   SHORT  KpsPowerDownCheck()
*
*                 
**  Description:
*                 Check Power Down of Terminal.
*==========================================================================
*/
SHORT    KpsPowerDownCheck(VOID)
{
    SHORT   nPortCnt;
    SHORT   nPrinterNo;
    USHORT  usTime = KPS_MONITOR_SEND_TIME_OUT;
    SHORT   sStatus;

    uchKpsPrintDown = 0;
    for (nPortCnt = 0; nPortCnt < KPS_NUMBER_OF_PRINTER; nPortCnt++) {
        if (g_auchKpsSysConfig[nPortCnt] == (UCHAR)(~DEVICE_KITCHEN_PRINTER)) {  
            continue;
        }

        if (g_hasKpsPort[nPortCnt] > 0) {
            sStatus = PifControlNetComIoEx(g_hasKpsPort[nPortCnt], PIF_COM_SEND_TIME, usTime); 
            if ((sStatus == PIF_ERROR_COM_POWER_FAILURE) || (sStatus == PIF_ERROR_COM_ACCESS_DENIED)) {
                nPrinterNo = _KpsComNo2KPNo(nPortCnt);  /* ### ADD 2172 Rel1.0 */
                if(0 < nPrinterNo){
                    if (KpsPowerDownRecover((UCHAR)nPrinterNo) == KPS_NOT_AS_MASTER) {
                        return KPS_NOT_AS_MASTER;
                    }
                }
            }
        }
    }

    KpsStartPrint();
    return KPS_PERFORM;
}
/*
*==========================================================================
**    Synopsis:   SHORT KpsWaitPowerDown(VOID)
*
**  Return   :   KPS_PERFORM        -   success
*                KPS_OPEND_ERROR    -   error
*
**  Description:
*                Wait for Power Down.
*==========================================================================
*/
SHORT  KpsWaitPowerDown(VOID)
{
    USHORT  fsSpeStatus;

    NbWaitForPowerUp();         /* wait for nb starting */
    NbWaitForPowerUp();         /* wait for server starting */
    NbWaitForPowerUp();         /* wait for server starting */
    for ( ; ; ) {
        SstReadFlag(&fsSpeStatus);
        if (!(fsSpeStatus & SER_SPESTS_INQUIRY)) {
            if (KPS_PERFORM == KpsCheckAsMaster()) {
                g_uchKpsTerminalLock = 0;         /* add hotel model */
                return KPS_PERFORM;
                break;
            }
            break;
        } else {
            PifSleep(KPS_WAIT_POWER_DOWN_TIMER);
        }
    }
    return KPS_OPEND_ERROR;
}
/*
*==========================================================================
**    Synopsis:   SHORT KpsCheckAsMaster(VOID)
*
**  Return   :    KPS_PERFORM       -   I am as Master.
*                 KPS_NOT_AS_MASTER -   I am not as Master.
*
**  Description:
*                
*==========================================================================
*/
SHORT   KpsCheckAsMaster(VOID)
{
    NBMESSAGE   NbInf;
    SYSCONFIG CONST *pSysconfig = PifSysConfig();    /* get system configration */;

    NbReadAllMessage(&NbInf);
    if (CLI_TGT_MASTER == pSysconfig->auchLaddr[CLI_POS_UA]) {
        if (!(NbInf.fsSystemInf & NB_MTUPTODATE)) {
            return KPS_NOT_AS_MASTER;
        }
    } else {
        if (!(NbInf.fsSystemInf & NB_BMUPTODATE)) {
            return KPS_NOT_AS_MASTER;
        }
/* --- Saratoga
        else if (NbInf.fsSystemInf & NB_MTUPTODATE){
            return KPS_NOT_AS_MASTER;
        }
 --- */
    }
    KpsStartPrint();
    return KPS_PERFORM;
}

/*
*==========================================================================
**    Synopsis:   VOID    KpsStartPrint(VOID)
*
**  Return   :    KPS_PERFORM       -    Printer is not printing.
*                 KPS_PRINTER_BUSY  -    Printer is printing.
*
**  Description:
*           This Function set KPS_POWERUP_OK flag in uchKpsFlag.
*           when kitchen printer module processing.
*==========================================================================
*/
VOID    KpsStartPrint(VOID)
{
    if ((KpsEmptyQue.pFirstQue != NULL) || (KpsProcQue.pFirstQue != NULL)) {
        uchKpsFlag &= ~KPS_STOP_RECOVERY;
    }
/*    uchKpsTerminalLock = 0;           delete hotel model */
}
/*
*==========================================================================
**    Synopsis:   SHORT   KpsCheckTime
*                   (DATE_TIME *pOld_Date_Time, DATE_TIME *pNow_Date_Time)
*
*       Input:    pOld_Date_Time    -   Old Date & Time
*                 pNow_Date_Time    -   New Date & Time
*
*                       
**  Return   :    0 <   -   It is impossible to messure time. rather than a hour.
*                 0 =>  -   Time (minutes)
*
**  Description:
*               This function is to messure time between Old_Date_Time
*               and Now_Date_Time.
*==========================================================================
*/
SHORT   KpsCheckTime(DATE_TIME *pOld_Date_Time, DATE_TIME *pNow_Date_Time)
{
    USHORT  usWork;

    if (0 == (usWork = pNow_Date_Time->usYear - pOld_Date_Time->usYear)) {
        if (0 == (usWork = pNow_Date_Time->usMonth - pOld_Date_Time->usMonth)) {
            if (0 == (usWork = pNow_Date_Time->usMDay - pOld_Date_Time->usMDay)) {
                if (0 == (usWork = pNow_Date_Time->usHour - pOld_Date_Time->usHour)) {
                    return (pNow_Date_Time->usMin - pOld_Date_Time->usMin);
                } else if (1 == usWork) {
                    return ((pNow_Date_Time->usMin + 60) - pOld_Date_Time->usMin);
                }
            } else if (1 == usWork) {
                if (0 == (usWork = pNow_Date_Time->usHour + 24 - pOld_Date_Time->usHour)) {
                    return (pNow_Date_Time->usMin - pOld_Date_Time->usMin);
                } else if (1 == usWork) {
                    return ((pNow_Date_Time->usMin + 60) - pOld_Date_Time->usMin);
                }
            }
        } else if (1 == usWork) { /* Month */
            if (pNow_Date_Time->usMDay != 1) {
                return KPS_PRINT_ERROR;
            }
            switch (pOld_Date_Time->usMonth) {
                case 1:
                case 3:
                case 5:
                case 7:
                case 8:
                case 10:
                case 12:
                    if (31 != pOld_Date_Time->usMDay) {
                        return KPS_PRINT_ERROR;
                    }
                    break;
                case 4:
                case 6:
                case 9:
                case 11:
                    if (30 != pOld_Date_Time->usMDay) {
                        return KPS_PRINT_ERROR;
                    }
                    break;
                case 2:
                    if (0 == ((pOld_Date_Time->usYear - 1992) % 4)) {
                        if (29 != pOld_Date_Time->usMDay) {
                            return KPS_PRINT_ERROR;
                        }
                    } else {
                        if (28 != pOld_Date_Time->usMDay) {
                            return KPS_PRINT_ERROR;
                        }
                    }
                    break;
                default:
                    break;
            }
            if ((pOld_Date_Time->usHour == 23) &&
                (pNow_Date_Time->usHour == 0)) {
                return ((pNow_Date_Time->usMin + 60) - (pOld_Date_Time->usMin));
            }
        }
        return KPS_PRINT_ERROR;
    } else if (1 == usWork) {
        if ((pOld_Date_Time->usMonth == 12)
            && (pOld_Date_Time->usMDay == 31)
            && (pOld_Date_Time->usHour == 23)
            && (pNow_Date_Time->usMonth == 1)
            && (pNow_Date_Time->usMDay == 1)
            && (pNow_Date_Time->usHour == 0)) {
            return ((pNow_Date_Time->usMin + 60) - (pOld_Date_Time->usMin));
        }
    }
    return KPS_PRINT_ERROR;
}

/*
*==========================================================================
**    Synopsis: USHORT  KpsGetBuffFile(ULONG ulOffset, VOID *pData,
*                                      USHORT usSize, SHORT hsFileHandle)
*                                                        
*       Input:  ULONG   ulOffset - Number of bytes from the biginning of the file.
*               VOID    *pData - Pointer to read data buffer.
*               USHORT  usSize - Number of bytes to be read.
*               SHORT   hsFileHandle - File Handle.
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  KPS_NO_READ_SIZE
				KPS_ERROR
				KPS_SUCCESS
*
**  Description:
*           Get data from Kps buffer file.

  RPH 11-10-3 Changes for PifReadFile

  Now returns Success/Error size of read is returned in pulActualBytesRead
*==========================================================================
*/
static SHORT KpsGetBuffFile(ULONG ulOffset, VOID *pData,
                      ULONG ulSize, SHORT hsFileHandle, ULONG *pulActualBytesRead)
{
    ULONG   ulActPos;
	SHORT   sError;

	*pulActualBytesRead = 0;

    if (!ulSize) {          /* check writed size */
        return(KPS_NO_READ_SIZE);          /* if read nothing, return */
    }

	if (0 > hsFileHandle) {
		NHPOS_ASSERT (hsFileHandle >= 0);
		return(KPS_ERROR);
	}

    if ((sError = PifSeekFile(hsFileHandle, ulOffset, &ulActPos)) != PIF_OK) {
        PifLog(MODULE_KPS, FAULT_ERROR_FILE_SEEK);
        PifLog(MODULE_ERROR_NO(MODULE_KPS), (USHORT)abs(sError));
        PifAbort(MODULE_KPS, FAULT_ERROR_FILE_SEEK); /* if error, then abort */
    }

    PifReadFile(hsFileHandle, pData, ulSize, pulActualBytesRead);
	if(*pulActualBytesRead != ulSize){
		return(KPS_ERROR);
	}

    return (KPS_SUCCESS);
}

/*
*==========================================================================
**    Synopsis: VOID    KpsPutBuffFile(ULONG ulOffset, VOID *pData,
*                                      USHORT usSize, SHORT hsFileHandle)
*                                                        
*       Input:  ULONG   ulOffset - Number of bytes from the biginning of the file.
*               VOID    *pData - Pointer to write data buffer.
*               USHORT  usSize - Number of bytes to be written.
*               SHORT   hsFileHandle - File Handle.
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing 
*
**  Description:
*           Put data to Kps buffer file.
*==========================================================================
*/
static VOID KpsPutBuffFile(ULONG ulOffset, VOID *pData,
                    USHORT usSize, SHORT hsFileHandle)
{
    ULONG   ulActPos;
	SHORT   sError;

    if (!usSize) {                  /* check writed size */
        return;                     /* if nothing, return */
    }

	if (0 > hsFileHandle) {
		NHPOS_ASSERT (hsFileHandle >= 0);
		return;
	}

    if ((sError = PifSeekFile(hsFileHandle, ulOffset, &ulActPos)) != PIF_OK) {
        PifLog(MODULE_KPS, FAULT_ERROR_FILE_SEEK);
        PifLog(MODULE_ERROR_NO(MODULE_KPS), (USHORT)abs(sError));
        PifAbort(MODULE_KPS, FAULT_ERROR_FILE_SEEK);
    }

    PifWriteFile(hsFileHandle, pData, usSize);
}

/************************* ADD R3.0 ***********************************/
/*
*===========================================================================
** Format  : VOID  KpsShrInit(VOID);      
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function sets shared print initial data.
*===========================================================================
*/
VOID    KpsShrInit(VOID)
{
    /* -- set terminal unique address -- */
    KpsShrInf.HdInf.uchTermAdr = CliReadUAddr();       

    /* -- set consecutive No. -- */
    KpsShrInf.HdInf.usConsNo++;

    /* -- set frame name -- */
    KpsShrInf.HdInf.fbFrame = SHR_FIRST_FRAME;

    /* -- set sequence No. -- */
    KpsShrInf.HdInf.uchSeqNo = 1;
    
    /* -- set current data length -- */
    KpsShrInf.HdInf.usILI = sizeof(SHRHEADERINF);

    /* -- initialize print data area -- */
    memset(KpsShrInf.auchPrtData, '\0', sizeof(KpsShrInf.auchPrtData));
}
/************************* END ADD R3.0 ***********************************/

/****** End of Source ******/
