/*
*===========================================================================
* Title       : Read Function
* Category    : ESC/POS Printer Controler, NCR 2170 System S/W
* Program Name: ESCPREAD.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Compact Model                                               
* Options     : /c /ACw /W4 /G1s /Os /Zap                                 
* --------------------------------------------------------------------------
* Abstract:     This contains the following function:
*
*           SHORT EscpReadCom(VOID *pBuff, SHORT sCount);
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :Ver.Rev.:   Name    : Description
* Oct-14-99:00.00.01:T.Koyama   : Initial
*          :        :           :                                    
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
#include "stdafx.h"
#include <string.h>

#include <ecr.h>
#include <pif.h>
//#include <rfl.h>
#include "printio.h"
#include "escp.h"
#include "pmg.h"

/*
*===========================================================================
*   Synopsis:   SHORT EscpReadCom(SHORT sDummy, VOID *pBuff, SHORT sCount);
*
*       pBuff       points response data buffer
*       sCount      size of buffer
*
**  Return:     0 >=   - sizeof(ESCP_RESPONSE) if successful
*               SYSERR - Not Opened
*               TIMEOUT- No response timeout
*               Others - Serial I/O Port errors
*
**  Description: This waits for a status returned by the printer against
*                ESC v, generates PIP response message and returns it.
*===========================================================================
*/
SHORT EscpReadCom(SHORT sDummy, VOID FAR *pBuff, SHORT sCount)
{
    ESCP *pEscp;
    UCHAR uchError;
    UCHAR uchStatus;
    SHORT sSeqNo;
    ESCP_RESPONSE FAR *pResp;
    SHORT sResult=PIF_OK;

    pEscp = &Escp[0];
    if (!pEscp->chOpen || sCount < sizeof(ESCP_RESPONSE)) {
                                        /* see if opened and enough buffer */
        return (PIF_ERROR_SYSTEM);
    }

    sResult = EscpReadPrinterStatus(&uchStatus, &uchError);

    /* Dequeue Sequence # whether or not a response is received. */

    PifRequestSem(pEscp->sSem);                   /* task switch off */
    sSeqNo = EscpFifoDeque(&Escp->Fifo);
#ifdef DEBUG
    TCHAR   twork[1024];
    wsprintf(twork,TEXT("!!! ESCP Read : EscpReadCom[%d] State[%02x] Error[%02x]\n"),
             sSeqNo, uchStatus, uchError);
    OutputDebugString(twork);
#endif

    PifReleaseSem(pEscp->sSem);                   /* task switch on */

    if (sSeqNo < 0 ||  sResult == PIF_ERROR_COM_TIMEOUT
				   ||  sResult == PIF_ERROR_COM_POWER_FAILURE) {
#ifdef DEBUG
        char uchRcvBuf;
        PifReadCom(pEscp->sSio, &uchRcvBuf, sizeof(uchRcvBuf));
#endif
        pEscp->sEscvCount=0;
        PifRequestSem(pEscp->sSem);                   /* task switch off */
        EscpFifoClear(&Escp->Fifo);     /* Clear Sequence # FIFO */
        PifReleaseSem(pEscp->sSem);                   /* task switch on */
		if (sResult == PIF_ERROR_COM_POWER_FAILURE) {
	        return (PIF_ERROR_COM_POWER_FAILURE);
		}
        return (PIF_ERROR_COM_TIMEOUT);               /* no valid response received */
    }

    /* Now, generate a response message */

    pResp = (ESCP_RESPONSE FAR *)pBuff;
//    _RflFMemSet(pResp, 0, sizeof(*pResp));
    memset(pResp, 0, sizeof(*pResp));
    pResp->uchSeqNo = (UCHAR)sSeqNo;    /* Sequence # */
    pResp->uchStatus = uchStatus;       /* Status of the printer */
    pResp->uchError  = uchError;        /* set error  */
    return (sizeof(*pResp));
    sDummy = sDummy;
}


/*
*===========================================================================
*   Synopsis:   SHORT EscpReadPrinterStatus(UCHAR *puchStat, UCHAR *puchError);
*
*       puchStat    pointer to paper status
*       puchError   pointer to error status
*
**  Return:     PIF_OK - successful
*               TIMEOUT- No response timeout
*
**  Description: Get status form printer, and change format to PMG status form.
*
*===========================================================================
*/
#if defined (DEVICE_7140)
SHORT EscpReadPrinterStatus(UCHAR *puchStat, UCHAR *puchError)
{
    ESCP *pEscp;
    SHORT sLength;
    SHORT sStatus;
    SHORT sRetry;
    SHORT sRet;
    CHAR  chDleEotCmd;
    UCHAR uchRcvBuf;
    SHORT sResult=PIF_OK;
    CHAR  DleEot[] = "\x10\x04\x00";

    sRet = PIF_ERROR_COM_TIMEOUT;
    pEscp = &Escp[0];
    /*pEscp->uchStatus += (PRT_STAT_ERROR | PRT_STAT_INIT_END);*/
    pEscp->uchStatus |= (PRT_STAT_ERROR | PRT_STAT_INIT_END);
    *puchError = 0;
    *puchStat  = pEscp->uchStatus;       /* set printer status */


    if (pEscp->sEscvCount>0) {
        chDleEotCmd = DLE_EOT_GET_STATUS;
    } else {
        chDleEotCmd = DLE_EOT_WAIT_ESCV;
    }
#ifdef DEBUG
    TCHAR   twork[1024];
    wsprintf(twork,TEXT("!!! ESCP Read : EscpReadPrinterStatus..%02x\n"),chDleEotCmd);
    OutputDebugString(twork);
#endif

    for (sRetry = 0 ; sRetry < ESCP_RETRY_RECV ; sRetry++) {
        sStatus = PifReadCom(pEscp->sSio, &uchRcvBuf, sizeof(uchRcvBuf));
#ifdef DEBUG
        if (sStatus < 0){
            wsprintf(twork,TEXT("!!! ESCP Read : PifReadCom Error..%02x\n"),sStatus);
            OutputDebugString(twork);
        }
#endif
        
        if (sStatus == 1) {
#ifdef DEBUG
    wsprintf(twork,TEXT("!!! ESCP Read : PifReadCom..%02x\n"),uchRcvBuf);
    OutputDebugString(twork);
#endif
            if (!(uchRcvBuf & STAT_DLE_EOT_DATA)) {
                /* Receive ESC v */
                pEscp->uchStatus = (PRT_STAT_ERROR | PRT_STAT_INIT_END);
                /*pEscp->uchStatus |= (uchRcvBuf & ESCV_STAT_R_NEAR_END) ?    PRT_STAT_R_NEAR_END : pEscp->uchStatus;*/
                pEscp->uchStatus |= (uchRcvBuf & ESCV_STAT_J_NEAR_END) ?    PRT_STAT_J_NEAR_END : pEscp->uchStatus;
                /*pEscp->uchStatus |= (uchRcvBuf & ESCV_STAT_NO_VALIDATION) ? PRT_STAT_FORM_IN : pEscp->uchStatus;*/
                pEscp->uchStatus |= (uchRcvBuf & ESCV_STAT_NO_VALIDATION) ? pEscp->uchStatus : PRT_STAT_FORM_IN;
                if (chDleEotCmd == DLE_EOT_WAIT_ESCV) {
                    *puchStat = pEscp->uchStatus;       /* set printer status */
                    chDleEotCmd = DLE_EOT_GET_STATUS;
                }
                pEscp->sEscvCount++;

            } else {
            
                switch(chDleEotCmd) {
                case DLE_EOT_GET_STATUS:
                    if (!(uchRcvBuf & STAT1_PRINTER_BUSY)) {
                        chDleEotCmd = DLE_EOT_END_STAT;
                        sRet = PIF_OK;
                        break;           /* no error */
                    }
                    *puchStat  &= ~PRT_STAT_ERROR;   /* set error status */
                    *puchError = PRT_ERR_HOME;       /* default no home error */
                    chDleEotCmd = DLE_EOT_GET_OFFLINE_STATUS;
                    break;
                case DLE_EOT_GET_OFFLINE_STATUS:
                    *puchError = (uchRcvBuf & STAT2_ERR_NO_PAPER) ?    PRT_ERR_PAPER_END : *puchError;
                    if (!(uchRcvBuf & STAT2_ERR_CONDITION)) {
                        chDleEotCmd = DLE_EOT_END_STAT;
                        sRet = PIF_OK;
                    }
                    chDleEotCmd = DLE_EOT_GET_ERROR_STATUS;
                    break;
                case DLE_EOT_GET_ERROR_STATUS:
                    *puchError = (uchRcvBuf & STAT3_ERR_CUTTER_JAM) ?    PRT_ERR_KNIFE : *puchError;
                    chDleEotCmd = DLE_EOT_END_STAT;
                    sRet = PIF_OK;
                    break;
                default:
                    chDleEotCmd = DLE_EOT_END_STAT;
                    sRet = PIF_ERROR_COM_TIMEOUT;
                }
            }
            if (chDleEotCmd == DLE_EOT_END_STAT) {
                break;
            }

            /* send DLE EOT command */
            DleEot[2] = chDleEotCmd;
            sLength = strlen(DleEot);
            PifRequestSem(pEscp->sSem);                   /* 222task switch off */
            if (PifWriteCom(pEscp->sSio, DleEot, sLength) != sLength) {
#ifdef DEBUG
       		wsprintf(twork,TEXT("!!! ESCP Write : DleEot..%02x Error\n"),chDleEotCmd);
            OutputDebugString(twork);
#endif
                sRet = PIF_ERROR_COM_TIMEOUT;
                PifReleaseSem(pEscp->sSem);                   /* 222task switch on */
                break;
            }
#ifdef DEBUG
       		wsprintf(twork,TEXT("!!! ESCP Write : DleEot..%02x\n"),chDleEotCmd);
            OutputDebugString(twork);
#endif
            PifReleaseSem(pEscp->sSem);                   /* 222task switch on */
        }
		else if (sStatus == PIF_ERROR_COM_POWER_FAILURE) {
            sRet = PIF_ERROR_COM_POWER_FAILURE;
			break;
		}
    }
    pEscp->sEscvCount = (pEscp->sEscvCount==0)? 0 : pEscp->sEscvCount-1;
#ifdef DEBUG
    wsprintf(twork,TEXT("!!! ESCP Read : sEscvCount..%02x\n"),pEscp->sEscvCount);
    OutputDebugString(twork);
#endif
    return(sRet);
}
#endif


#if defined (DEVICE_7158) || defined (DEVICE_7194) || defined (DEVICE_7161) || defined (DEVICE_7196)
SHORT EscpReadPrinterStatus(UCHAR *puchStat, UCHAR *puchError)
{
    ESCP    *pEscp;
    UCHAR   uchStatus;
    SHORT   sRetry;
    SHORT   sStatus;
    SHORT   sMaxRetry;

    pEscp = &Escp[0];
/*    pEscp->uchStatus += (PRT_STAT_ERROR | PRT_STAT_INIT_END);*/
    pEscp->uchStatus |= (PRT_STAT_ERROR | PRT_STAT_INIT_END);
    *puchError = 0;
    *puchStat  = pEscp->uchStatus;       /* set printer status */

#if defined (DEVICE_7158) || defined (DEVICE_7194)
    sMaxRetry = 10;
#else
    sMaxRetry = ESCP_RETRY_RECV;
#endif
    for (sRetry = 0 ; sRetry < sMaxRetry; sRetry++) {
        sStatus = PifReadCom(pEscp->sSio, &uchStatus, sizeof(uchStatus));
        if (sStatus == 1) {
            break;                      /* successfully received */
        }
		else if (sStatus == PIF_ERROR_COM_POWER_FAILURE) {
            return(PIF_ERROR_COM_POWER_FAILURE);
		}
    }
	if (sRetry == sMaxRetry) {
#ifdef DEBUG
        CHAR  Escv[] = "\x1d\x04\x02\x1d\x04\x05";
        SHORT   sLength;
        UCHAR   uchErr;
        TCHAR   work2[1024];

        sLength = strlen(Escv);
        PifRequestSem(pEscp->sSem);                   /* 222task switch off */
		sStatus = PifWriteCom(pEscp->sSio, Escv, sLength);
        PifReleaseSem(pEscp->sSem);                   /* 222task switch on */
        sStatus = PifReadCom(pEscp->sSio, &uchErr, sizeof(uchErr));
        sStatus = PifReadCom(pEscp->sSio, &uchStatus, sizeof(uchStatus));
	    wsprintf(work2,TEXT("Error[%02x] SlipStatus[%02x]\n"), uchErr, uchStatus );
        OutputDebugString(work2);
#endif
        return(PIF_ERROR_COM_TIMEOUT);
	} else {
        pEscp->uchStatus = (PRT_STAT_ERROR | PRT_STAT_INIT_END);
        /*pEscp->uchStatus |= (uchStatus & ESCV_STAT_R_NEAR_END) ?    PRT_STAT_R_NEAR_END : pEscp->uchStatus;*/
        /*pEscp->uchStatus |= (uchStatus & ESCV_STAT_J_NEAR_END) ?    PRT_STAT_J_NEAR_END : pEscp->uchStatus;*/
        pEscp->uchStatus |= ((uchStatus & ESCV_STAT_SLIP_IN) && (uchStatus & ESCV_STAT_SLIP_OUT)) ?
                                                                    PRT_STAT_FORM_IN : pEscp->uchStatus;
        *puchStat  = pEscp->uchStatus;       /* set printer status */
        *puchError = 0;
        *puchError = (uchStatus & ESCV_STAT_COVER_OPEN) ? PRT_ERR_COVER_OPEN : *puchError;
        *puchError = (uchStatus & ESCV_STAT_JAM) ?        PRT_ERR_JAM        : *puchError;

		if (*puchError) {
            *puchStat &= ~PRT_STAT_ERROR;
		}
        return(PIF_OK);
	}
}
#endif
