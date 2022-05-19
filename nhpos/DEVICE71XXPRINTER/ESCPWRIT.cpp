/*
*===========================================================================
* Title       : Write Function
* Category    : ESC/POS Printer Controler, NCR 2170 System S/W
* Program Name: ESCPWRIT.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Compact Model                                               
* Options     : /c /ACw /W4 /G1s /Os /Zap                                 
* --------------------------------------------------------------------------
* Abstract:     This contains the following function:
*
*           SHORT EscpWriteCom(VOID *pBuff, SHORT sCount);
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :Ver.Rev.:   Name    : Description
* Oct-14-99:00.00.01:T.Koyama   : Initial
* Feb-28-00:00.00.01:H.Endo     : Add EscpWriteCmdChange(for 7158Printer)
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
#include "escp.h"
	
/*
*===========================================================================
*   Synopsis:   SHORT EscpWriteCom(SHORT sDummy, VOID *pBuff, SHORT sCount);
*
*       pBuff       points request data buffer
*       sCount      size of buffer
*
**  Return:     0 >=   - sCount if successful
*               SYSERR - Not Opened
*               TIMEOUT- send timeout
*               Others - Serial I/O Port errors
*
**  Description: This sends out print data.
*===========================================================================
*/
SHORT EscpWriteCom(SHORT sDummy, VOID FAR *pBuff, SHORT sCount)
{
    ESCP *pEscp;
    ESCP_REQUEST FAR *pReq;
    SHORT sLength;
    SHORT sResult;
    SHORT sStatus;
//    SHORT sLoop;
//    CHAR achTemp[8];
    CHAR  Escv[] = "\x1bv";
//    CHAR  Escv[] = "\x1d\x05";
    pEscp = &Escp[0];
    sLength = sCount - (sizeof(*pReq)-sizeof(pReq->auchData));
    if (!pEscp->chOpen || sLength < 0) { /* see if opened and enough buffer */
        return (PIF_ERROR_SYSTEM);
    }

    if ( pEscp->Fifo.uchCount >= ESCP_FIFO_SIZE  ) {
        return(0);
    }
    
    sResult = sCount;
    pReq = (ESCP_REQUEST FAR *)pBuff;
    PifRequestSem(pEscp->sSem);                   /* 222task switch off */
#ifdef DEBUG
    if(strstr((const char *)&pReq->auchData,"VAL    L")){
        OutputDebugString(TEXT("val last\n"));
    }
#endif
	sStatus = PifWriteCom(pEscp->sSio, &pReq->auchData, sLength);
    if (sStatus == PIF_ERROR_COM_POWER_FAILURE) {
		sResult = PIF_ERROR_COM_POWER_FAILURE;
	}
    else if (sStatus != sLength) {
		sResult = PIF_ERROR_COM_TIMEOUT;
	}
#ifdef DEBUG
    TCHAR    work[1024],work2[1024];
    memset(work,0,sizeof(work));
	mbstowcs(work, (const char *)&pReq->auchData,sLength);
	wsprintf(work2,TEXT("!!! ESCP Write[%d] : MSG=%s RESULT=%d\n"),pReq->uchSeqNo,work,sResult);
    OutputDebugString(work2);
#endif
    PifReleaseSem(pEscp->sSem);                   /* 222task switch on */
    if (sResult == sCount) {
       /* send 'Esc v' */
        sLength = strlen(Escv);
        PifRequestSem(pEscp->sSem);                   /* 222task switch off */
		sStatus = PifWriteCom(pEscp->sSio, Escv, sLength);
	    if (sStatus == PIF_ERROR_COM_POWER_FAILURE) {
			sResult = PIF_ERROR_COM_POWER_FAILURE;
		}
	    else if (sStatus != sLength) {
			sResult = PIF_ERROR_COM_TIMEOUT;
		}
#ifdef DEBUG
	wsprintf(work2,TEXT("!!! ESCP Write[%d] : MSG=Escv RESULT=%d\n"),pReq->uchSeqNo,sResult);
    OutputDebugString(work2);
#endif
        PifReleaseSem(pEscp->sSem);                   /* 222task switch on */
    }

    if (sResult == sCount) {
        PifRequestSem(pEscp->sSem);                   /* task switch off */
        EscpFifoEnque(&Escp->Fifo, pReq->uchSeqNo);
        PifReleaseSem(pEscp->sSem);                   /* task switch on */
                                        /* enque the sequence # */
        return(sCount);             /* successfully completed */
    }

    /* failed to send */

    PifRequestSem(pEscp->sSem);                   /* task switch off */
    EscpFifoClear(&Escp->Fifo);         /* Clear Sequence # FIFO */
    PifReleaseSem(pEscp->sSem);                   /* task switch on */
//    for (sLoop = 0 ; sLoop < ESCP_RETRY_CLEAR ; sLoop++) {  /* recovery by PMG */
//        PifReadCom(pEscp->sSio, achTemp, sizeof(achTemp));
//    }
    return (sResult);
    sDummy = sDummy;
}
