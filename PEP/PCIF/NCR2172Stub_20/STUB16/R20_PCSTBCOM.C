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
* Title       : Client/Server PC STUB module, Source File                        
* Category    : Client/Server PC STUB, internal functions
* Program Name: PCSTBCOM.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Large Model                                               
* Options     : /c /Alfw /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*       CstInitialize();                STUB initialize      
*       CstIamMaster();                 Check am I Master ?
*       CstIamBMaster();                Check am I Back Up Master ?
*       CstIamSatellite();              Check am I Satellite ?
*
*       CstSendMaster();                Send request to Master
*       CstSendBMaster();               Send request to Back Up Master
*       SstUpdateAsMaster();            Update Master File
*       SstUpdateBackUp();              Update Back Up Master File
*       SstReadAsMaster();              Read File from as master
*       SstReadFAsMaster();             Read File from as master by comm.
*
*       CstComMakeMessage();            Make comm. message
*       CstComSendReceive();            Send request and receive response
*       CstComChekInquiry();            Check condition to send request
*       CstComMakeMultiData();          Make Data to send
*       CstComCheckError();             Check communication error
*       CstComRespHandle();             response message handling
*       CstComErrHandle();              error handling
*       CstComSendConfirm();            Send confirmation message
*       CstComReadStatus();             read communication status
*       
*       CstDisplayError();              display error
*       CstBackUpError();               back up error handling
*       CstBMOutOfDate();               Make BM out of date
*       CstMTOutOfDate();               Make MT out of date
*       CstReadMDCPara();               Read MDC (Timer, retry co)
*       CstReadGCFType();               Read Para (GCF)
*       
*       CstSleep();                     Sleep function
*       CstNetOpen();                   Net work open
*       CstNetClose();                  Net work close
*       CstNetSend();                   Net work send
*       CstNetReceive();                Net work receive
*       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* Sep-01-98:           : Changed the CstNetClose() function.
*          :           :                                    
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
#include   <windows.h>
#include   <winsock.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    <memory.h>

#include    <r20_pc2172.h>
#include    <r20_pif.h>
#include    <log.h>
#include    <appllog.h>
#include    "csstubin.h"
#include "mypifdefs.h"

/*
------------------------------------------
    Internal Work Area (initialized)
------------------------------------------
*/
USHORT          husCliExeNet = -1;           /* semapho, APPLI - ISP */
XGRAM           CliSndBuf;              /* Send message buffer */       
XGRAM           CliRcvBuf;              /* Receive message buffer */
CLICOMIF        CliMsg;                 /* common message structure */
CLINETCONFIG    CliNetConfig;           /* NET control structure */


VOID    CstLogLine (char *aszPrintLine)
{
	static  int    iLogFile = -1;

	if (iLogFile < 0) {
		iLogFile = 0;
		if (getenv ("PIF_LOG_ON")) {
			iLogFile = 1;
		}
	}

	if (iLogFile) {
		static  FILE * hFile = 0;
		static  CHAR   fileHeader[32] = {0};
		static  ULONG  ulFileMaxRecords = 64000L;
		static  ULONG  ulFileCurrentRecord = 0L;
		char    xBuff[324];
		char    formatHeader[] = "%8.8d,%8.8d             ";

		formatHeader[23] = '\n';
		if (hFile == 0) {
			size_t readBytes = 0;

			hFile = fopen  ("PCIFLOG", "r+b");
			if (hFile) {
				readBytes = fread (fileHeader, sizeof(char), 24, hFile);
				if (readBytes < 24) {
					sprintf (fileHeader, formatHeader, ulFileMaxRecords, ulFileCurrentRecord);
					fileHeader[23] = '\n';
					fseek (hFile, 0L, SEEK_SET);
					fwrite (fileHeader, sizeof(char), 24, hFile);
				} else {
					ulFileMaxRecords = atol (fileHeader);
					ulFileCurrentRecord = atol (fileHeader + 9);
					if (ulFileMaxRecords < 10000L) ulFileMaxRecords = 32000L;
				}
			} else {
				hFile = fopen  ("PCIFLOG", "w+b");
				if (hFile) {
					sprintf (fileHeader, formatHeader, ulFileMaxRecords, ulFileCurrentRecord);
					fileHeader[23] = '\n';
					fseek (hFile, 0L, SEEK_SET);
					fwrite (fileHeader, sizeof(char), 24, hFile);
				}
			}
		}
		if (hFile && aszPrintLine) {
			int         iLength = 0;
			SYSTEMTIME  DateTime;

			GetLocalTime(&DateTime);
			//format the string to tell the time, date, thread name
			iLength += sprintf(xBuff, "%02d/%02d/%02d,%02d:%02d:%02d.%4.4d %-240.240s",
				DateTime.wMonth, DateTime.wDay, DateTime.wYear%100, DateTime.wHour, DateTime.wMinute, DateTime.wSecond, DateTime.wMilliseconds,
				aszPrintLine);
			xBuff[255] = '\n';
			fseek (hFile, (ulFileCurrentRecord * 256 + 24), SEEK_SET);
			fwrite (xBuff, sizeof(char), 256, hFile);

			ulFileCurrentRecord = ((ulFileCurrentRecord + 1) % ulFileMaxRecords);
			sprintf (fileHeader, formatHeader, ulFileMaxRecords, ulFileCurrentRecord);
			fileHeader[23] = '\n';
			fseek (hFile, 0L, SEEK_SET);
			fwrite (fileHeader, sizeof(char), 24, hFile);
			fflush (hFile);
		}
	}
}


VOID    CstLogNetSend (char *aszPrintLineFirst)
{
	CLIREQCOM   *pSend = (CLIREQCOM *)CliSndBuf.auchData;
	int         iLength = 0;
	PIFUSRTBLINFO  usrStats;
	char        aszPrintLine[255];

	PifNetControl(CliNetConfig.usHandle, PIF_NET_GET_USER_INFO, &usrStats);

	iLength += sprintf (aszPrintLine, " %-30.75s usrStats.usLastSenduchNo %d CLIREQCOM:usFunCode 0x%4.4x usSeqNo 0x%4.4x ",
		aszPrintLineFirst, usrStats.usLastSenduchNo, pSend->usFunCode, pSend->usSeqNo);

	CstLogLine (&aszPrintLine[0]);

	{
		int i, j;
		j = 0;
		aszPrintLine[j++] = ' ';
		aszPrintLine[j++] = ' ';
		aszPrintLine[j++] = ' ';
		aszPrintLine[j++] = ' ';
		aszPrintLine[j++] = ' ';
		for (i = 0; i < 64; i++) {
			UCHAR tempChar = ((CliSndBuf.auchData[i] >> 4) & 0x0f);
			if ((i % 4) == 0) aszPrintLine[j++] = ' ';
			aszPrintLine[j++] = (tempChar < 10) ? (tempChar + '0') : ((tempChar - 10) + 'a');
			tempChar = (CliSndBuf.auchData[i] & 0x0f);
			aszPrintLine[j++] = (tempChar < 10) ? (tempChar + '0') : ((tempChar - 10) + 'a');
		}
		aszPrintLine[j++] = 0;
	}
	CstLogLine (&aszPrintLine[0]);
}

VOID    CstLogNetRecv (char *aszPrintLineFirst)
{
	CLIRESCOM   *pResp = (CLIRESCOM *)CliRcvBuf.auchData;
	int         iLength = 0;
	PIFUSRTBLINFO  usrStats;
	char        aszPrintLine[255];

	PifNetControl(CliNetConfig.usHandle, PIF_NET_GET_USER_INFO, &usrStats);

	iLength += sprintf (aszPrintLine, " %-30.75s usrStats.usLastRecvuchNo %d CLIRESCOM: usFunCode 0x%4.4x usSeqNo 0x%4.4x sResCode %4.4d sReturn %4.4d",
		aszPrintLineFirst, usrStats.usLastRecvuchNo, pResp->usFunCode, pResp->usSeqNo, pResp->sResCode, pResp->sReturn);

	CstLogLine (&aszPrintLine[0]);

	{
		int i, j;
		j = 0;
		aszPrintLine[j++] = ' ';
		aszPrintLine[j++] = ' ';
		aszPrintLine[j++] = ' ';
		aszPrintLine[j++] = ' ';
		aszPrintLine[j++] = ' ';
		for (i = 0; i < 64; i++) {
			UCHAR tempChar = ((CliRcvBuf.auchData[i] >> 4) & 0x0f);
			if ((i % 4) == 0) aszPrintLine[j++] = ' ';
			aszPrintLine[j++] = (tempChar < 10) ? (tempChar + '0') : ((tempChar - 10) + 'a');
			tempChar = (CliRcvBuf.auchData[i] & 0x0f);
			aszPrintLine[j++] = (tempChar < 10) ? (tempChar + '0') : ((tempChar - 10) + 'a');
		}
		aszPrintLine[j++] = 0;
	}
	CstLogLine (&aszPrintLine[0]);
}


/*
*===========================================================================
** Synopsis:    VOID    CstInitialize(VOID);
*
** Return:      none.
*
** Description: This function initilizes STUB function.
*===========================================================================
*/

VOID    CstInitialize(VOID)
{
    husCliExeNet = PifCreateSem(1);
}

/*
*===========================================================================
** Synopsis:    VOID    CstUninitialize(VOID);
*
** Return:      none.
*
** Description: This function initilizes STUB function.
*===========================================================================
*/
VOID    CstUninitialize(VOID)
{
    PifDeleteSem(husCliExeNet);
}

/*
*===========================================================================
** Synopsis:    SHORT    CstIamMaster(VOID);
*
** Return:      STUB_SUCCESS:   I am Master Terminal
*               STUB_ILLEGAL:   I am not Mater Terminal
*
** Description:
*===========================================================================
*/
SHORT   CstIamMaster(VOID)
{
    return STUB_ILLEGAL;
}

/*
*===========================================================================
** Synopsis:    SHORT    CstIamBMaster(VOID);
*
** Return:      STUB_SUCCESS:   I am Back Up Master Terminal
*               STUB_ILLEGAL:   I am not Back Up Mater Terminal
*
** Description:
*===========================================================================
*/
SHORT   CstIamBMaster(VOID)
{
    return STUB_ILLEGAL;
}

/*
*===========================================================================
** Synopsis:    SHORT    CstIamSatellite(VOID);
*
** Return:      STUB_SUCCESS:   I am Satellite Terminal
*               STUB_ILLEGAL:   I am not Satellite Terminal
*
** Description:
*===========================================================================
*/
SHORT   CstIamSatellite(VOID)
{
    return STUB_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    SHORT    CstSendMaster(VOID);
*
** Return:      STUB_SUCCESS:   communication success
*               STUB_SELF:      I am update Master terminal      
*               STUB_M_DOWN:    Master terminal is out of service
*
** Description:
*   This function supports to communicate with Master terminal.
*===========================================================================
*/
SHORT   CstSendMaster(VOID)
{
//    CstComChekInquiry();
//    if (0 == (CstComReadStatus() & CLI_STS_M_UPDATE)) {
//        return STUB_M_DOWN;
//    }
    if (CLI_IAM_MASTER) {
        return STUB_SELF;
    }

	memset (CliRcvBuf.auchData, 0, sizeof(CLIRESCOM));

    CstComMakeMessage(CLI_TGT_MASTER);
    CstComSendReceive();

    if (0 == (CstComReadStatus() & CLI_STS_M_UPDATE)) {
        return STUB_M_DOWN;
    }
    return STUB_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    VOID    CstComMakeMessage(UCHAR uchServer);
*
*     Input:    uchServer   - uniuqe address of treget server
*
** Return:      none.
*
** Description:
*   This function makes send message.
*===========================================================================
*/
VOID    CstComMakeMessage(UCHAR uchServer)
{
    memcpy(CliSndBuf.auchFaddr, CliNetConfig.auchFaddr, PIF_LEN_IP);
/*****    
      CliSndBuf.auchFaddr[CLI_POS_UA] = uchServer;
      CliSndBuf.auchFaddr[1] = 0;
*****/
    uchServer = uchServer;                  /* only refer */

    CliSndBuf.usFport = CLI_PORT_ISPSERVER;
    CliSndBuf.usLport = CLI_PORT_STUB;

    CliMsg.pReqMsgH->usFunCode = (USHORT)(CliMsg.usFunCode & CLI_RSTBACKUPFG);
    CliMsg.pReqMsgH->usSeqNo = 0;
    memcpy(CliSndBuf.auchData, CliMsg.pReqMsgH, CliMsg.usReqMsgHLen);

    CliMsg.usAuxLen = 0;
    CliMsg.usRetryCo = 0;
}
    
/*
*===========================================================================
** Synopsis:    VOID    CstComSendReceive(VOID);
*
** Return:      none.
*
** Description:
*   This function sends request to terget server and receives response.
*===========================================================================
*/
SHORT    CstComSendReceive(VOID)
{
    USHORT  usSendLen;
	SHORT   sRecvStatus, sRetryCount = 3;   // was 5 however that seems excessive.

	CliMsg.fCliMultiRcv = FALSE;

    do {
//        CstComChekInquiry();                    /* check inquiry condition */
        usSendLen = CstComMakeMultiData();      /* make request data */
		do {
			CliMsg.sError = STUB_SUCCESS;
			sRecvStatus = CstNetSend(usSendLen);    /*=== SEND MESSAGE ===*/
			if (0 < sRecvStatus) {
				sRecvStatus = CstNetReceive();          /*=== RECEIVE RESPONSE ===*/
			}
			sRetryCount--;
			if (sRecvStatus < 0)
				PifSleep (50);
		} while (sRecvStatus < 0 && sRetryCount > 0);
        CstComCheckError();                     /* error check */
        CstComRespHandle();                     /* response handling */
        CstComErrHandle();                      /* error handling */
        CstComSendConfirm();                    /* send confirm, if need */    
	} while (STUB_SUCCESS != CliMsg.sError);

	return CliMsg.sError;
}

/*
*===========================================================================
** Synopsis:    VOID    CstComChekInquiry(VOID);
*
** Return:      none.
*
** Description:
*   This function displays error message when this terminal or terget server
*   is inquiry state.
*===========================================================================
*/
VOID    CstComChekInquiry(VOID)
{
    SHORT   sError;
    USHORT  fsComStatus;

    if (CstComReadStatus() & (CLI_STS_STOP | CLI_STS_INQUIRY)) {
        CstSleep();
        fsComStatus = CstComReadStatus();
    } else {
        return;
    }
    while (fsComStatus & (CLI_STS_STOP | CLI_STS_INQUIRY)) {

        sError = STUB_DUR_INQUIRY;
        if (fsComStatus & CLI_STS_INQTIMEOUT) {
            if (CLI_IAM_MASTER) {
                sError = STUB_INQ_BM_DOWN;
            }
            if (CLI_IAM_BMASTER) {
                sError = STUB_INQ_M_DOWN;
            }
        }
        CstDisplayError(sError);
        fsComStatus = CstComReadStatus();
    }
}

/*
*===========================================================================
** Synopsis:    VOID    CstComMakeMultiData(VOID);
*
** Return:      data length to be sent.
*
** Description:
*   This function makes request data into send buffer.
*===========================================================================
*/
USHORT  CstComMakeMultiData(VOID)
{
    CLIREQCOM   *pSend;
    USHORT      usDataSiz=0, usDataMax;

    pSend = (CLIREQCOM *)CliSndBuf.auchData;

    if ((0 != CliMsg.usReqLen) && (! CliMsg.fCliMultiRcv) &&
        (0 == (pSend->usSeqNo & CLI_SEQ_SENDEND))) {

        pSend->usSeqNo ++;
        pSend->usSeqNo |= CLI_SEQ_SENDDATA;
        if (CliMsg.usAuxLen > CliMsg.usReqLen) {
            CliMsg.usAuxLen = CliMsg.usReqLen;
        }
        usDataSiz = (USHORT)(CliMsg.usReqLen - CliMsg.usAuxLen);
        usDataMax = (USHORT)(CLI_MAX_REQDATA - (CliMsg.usReqMsgHLen + 2));
        if (usDataSiz > usDataMax) {
            usDataSiz = usDataMax;
        } else {
            pSend->usSeqNo |= CLI_SEQ_SENDEND;
        }
        memcpy((UCHAR *)pSend + CliMsg.usReqMsgHLen, &usDataSiz, 2);
        memcpy((UCHAR *)pSend + CliMsg.usReqMsgHLen + 2,
               CliMsg.pauchReqData + CliMsg.usAuxLen,
               usDataSiz);
        CliMsg.usAuxLen += usDataSiz;
        usDataSiz += 2;

    } else if (0 != CliMsg.usResLen) {
        pSend->usSeqNo &= CLI_SEQ_CONT;
        ++ pSend->usSeqNo;
		CliMsg.fCliMultiRcv = TRUE;
    }
    if (CLI_TGT_MASTER == CliSndBuf.auchFaddr[CLI_POS_UA]) {
        if (0 == (CstComReadStatus() & CLI_STS_BM_UPDATE)) {
            pSend->usFunCode |= CLI_SETBMDOWN;
        }
    } else {
        if (0 == (CstComReadStatus() & CLI_STS_M_UPDATE)) {
            pSend->usFunCode &= ~CLI_FCWBACKUP;
        }
    }
    CliMsg.sError = STUB_SUCCESS;
    return ((USHORT)(CLI_MAX_IPDATA + CliMsg.usReqMsgHLen + usDataSiz));
}
        
/*
*===========================================================================
** Synopsis:    VOID    CstComCheckError(VOID);
*
** Return:      none.
*
** Description:
*   This function checks server error.
*===========================================================================
*/
VOID    CstComCheckError(VOID)
{
    CLIRESCOM   *pResp;

    if (STUB_SUCCESS == CliMsg.sError) {
        pResp = (CLIRESCOM *)CliRcvBuf.auchData;
        CliMsg.sError = pResp->sResCode;
    }
}

/*
*===========================================================================
** Synopsis:    VOID    CstComRespHandle(VOID);
*
** Return:      none.
*
** Description:
*   This function copies received message and data to specified buffer.
*===========================================================================
*/
VOID    CstComRespHandle(VOID)
{
    CLIREQCOM   *pSend;
    CLIRESCOM   *pResp;
    USHORT      usDataSiz;

    if ((STUB_SUCCESS != CliMsg.sError) &&
        (STUB_MULTI_SEND != CliMsg.sError)) {
        return;
    }
    pSend = (CLIREQCOM *)CliSndBuf.auchData;
    pResp = (CLIRESCOM *)CliRcvBuf.auchData;

    if ((CLI_TGT_BMASTER == CliSndBuf.auchFaddr[CLI_POS_UA]) &&
        (pSend->usFunCode & CLI_FCWBACKUP)) {
        if (0 > pResp->sReturn) {
            CliMsg.sError = STUB_BM_DOWN;
        }
        return;
    }
    memcpy(CliMsg.pResMsgH, pResp, CliMsg.usResMsgHLen);

    CliMsg.sRetCode = pResp->sReturn;
    if (pResp->usSeqNo & CLI_SEQ_RECVDATA) {
        if (pSend->usSeqNo & CLI_SEQ_SENDEND) {
            CliMsg.usAuxLen = 0;
        }
        memcpy(&usDataSiz, (UCHAR *)pResp + CliMsg.usResMsgHLen, 2);
        if ((usDataSiz + CliMsg.usAuxLen) <= CliMsg.usResLen) {
            memcpy(CliMsg.pauchResData + CliMsg.usAuxLen,
                   (UCHAR *)pResp + CliMsg.usResMsgHLen + 2,
                   usDataSiz);
            CliMsg.usAuxLen += usDataSiz;
        } else {
            if (CliMsg.usAuxLen < CliMsg.usResLen) {
                usDataSiz = (USHORT)(CliMsg.usResLen - CliMsg.usAuxLen);
                memcpy(CliMsg.pauchResData + CliMsg.usAuxLen,
                       (UCHAR *)pResp + CliMsg.usResMsgHLen + 2,
                       usDataSiz);
                CliMsg.usAuxLen += usDataSiz;
            }
        }            
    }                
}

/*
*===========================================================================
** Synopsis:    VOID    CstComErrHandle(VOID);
*
** Return:      none.
*
** Description:
*   This function executes error handling.
*===========================================================================
*/
VOID    CstComErrHandle(VOID)
{
    CLIREQCOM   *pSend;
    USHORT      usRetry;

    if ((STUB_SUCCESS == CliMsg.sError) ||
        (STUB_MULTI_SEND == CliMsg.sError) ||
        (STUB_MULTI_RECV == CliMsg.sError)) {
        return;
    }
    pSend = (CLIREQCOM *)CliSndBuf.auchData;
    pSend->usSeqNo = 0;
    CliMsg.usAuxLen = 0;

    switch(CliMsg.sError) {

    case    STUB_POWER_DOWN:
        CstNetClose();
        CstNetOpen();
     /* break; */                               /* not used */

    case    STUB_BUSY_MULTI:
    case    STUB_BUSY:
    case    STUB_TIME_OUT:
        ++ CliMsg.usRetryCo;
        usRetry = CstReadMDCPara(CLI_MDC_RETRY);
        if (CliMsg.usRetryCo > usRetry) {
            CstDisplayError(CliMsg.sError);
            CliMsg.usRetryCo = 0;
        } else {
            CstSleep();
        }
        break;

    default:
        if (CLI_IAM_MASTER) {
            CliMsg.sError = STUB_BM_DOWN;
        }
        CstDisplayError(CliMsg.sError);
        break;
    }

#if 0
    if (CLI_TGT_MASTER == CliSndBuf.auchFaddr[CLI_POS_UA]) {
        if (0 == (CstComReadStatus() & CLI_STS_M_UPDATE)) {
            CliMsg.sError = STUB_SUCCESS;
        }
    } else {
        if (0 == (CstComReadStatus() & CLI_STS_BM_UPDATE)) {
            CliMsg.sError = STUB_SUCCESS;
        }
    }
#endif
}

/*
*===========================================================================
** Synopsis:    VOID    CstComSendConfirm(VOID);
*
** Return:      none.
*
** Description:
*   This function sends confirmation message if it need.
*===========================================================================
*/
VOID    CstComSendConfirm(VOID)
{
    CLIREQCOM   *pSend;

    pSend = (CLIREQCOM *)CliSndBuf.auchData;
    if ((STUB_SUCCESS == CliMsg.sError) &&
        (pSend->usFunCode & CLI_FCWCONFIRM)) {
        pSend->usFunCode |= CLI_SETCONFIRM;
        ++ pSend->usSeqNo;
        CstNetSend((USHORT)(CLI_MAX_IPDATA + CliMsg.usReqMsgHLen));
    }
}

/*
*===========================================================================
** Synopsis:    USHORT  CstComReadStatus(VOID);
*
** Return:      current communication status
*
** Description:
*   This function get communication status.
*===========================================================================
*/
USHORT  CstComReadStatus(VOID)
{
    USHORT      fsComStatus;

    fsComStatus = 0;

    fsComStatus |= CLI_STS_M_UPDATE;
    fsComStatus |= CLI_STS_BM_UPDATE;

    return fsComStatus;
}

/*
*===========================================================================
** Synopsis:    VOID    CstDisplayError(sError);
*
*     Input:    sError  - Error Code for STUB
*
** Return:      none.
*
** Description:
*   This function supports display of error code in the Terminal
*   application, NeighborhoodPOS.  We don't do leadthru messages
*   in PEP and the PCIF so instead, we set the sRetCode to be the
*   error message and then return.
*   
*===========================================================================
*/
VOID    CstDisplayError(SHORT sError)
{
    CliMsg.sError   = STUB_SUCCESS;
    CliMsg.sRetCode = (sError + STUB_RETCODE);
}
        
/*
*===========================================================================
** Synopsis:    USHORT    CstReadMDCPara(UCHAR address);
*
*     Input:    address - MDC address
*
** Return:      MDC Value.
*
** Description:
*   This function reads MDC Parameter.
*===========================================================================
*/
USHORT   CstReadMDCPara(UCHAR address)
{
    USHORT  usValue = 0;

    switch(address) {

    case    CLI_MDC_CLIENT:         /* response timer */
        usValue = PCS_DEFMDC_CLIENT;
        break;

    case    CLI_MDC_RETRY:          /* retry counter */
        usValue = PCS_DEFMDC_RETRY;
        break;
    
    case    CLI_MDC_WAIT:           /* wait timer */
        usValue = PCS_DEFMDC_WAIT;
        break;

    default:
        break;
    }
    return usValue;
}

/*
*===========================================================================
** Synopsis:    VOID    CstSleep(VOID);
*
** Return:      none.
*
** Description:
*   This function sleeps.
*===========================================================================
*/
VOID    CstSleep(VOID)
{
    LONG    ulWait;
    USHORT  usMsec;

    ulWait = CstReadMDCPara(CLI_MDC_WAIT) * 1000L;

    while(0L < ulWait) {
        usMsec = (USHORT)((50000U < ulWait) ? (USHORT)50000 : (USHORT)ulWait);
        PifSleep(usMsec);
        ulWait -= usMsec;
    }
}

/*
*===========================================================================
** Synopsis:    VOID    CstNetOpen(VOID);
*
** Return:      none.
*
** Description:
*   This function opens NET for stub.
*===========================================================================
*/
VOID    CstNetOpen(VOID)
{
    SYSCONFIG CONST FAR *SysCon;
    XGHEADER        IPdata;
    SHORT           sHandle;

    CliNetConfig.fchStatus = 0;
    memset(&IPdata, 0, sizeof(XGHEADER));

    SysCon = PifSysConfig();

    if ((SysCon->auchLaddr[0] == 0) &&
        (SysCon->auchLaddr[1] == 0) &&
        (SysCon->auchLaddr[2] == 0)) {      /* comm. board not provide */
        
        if (SysCon->auchLaddr[3] != 1) {    /* not master ? */
            PifAbort(MODULE_STB_ABORT, FAULT_BAD_ENVIRONMENT);
        }                                   /* backup system ? */
        if (1 == SysCon->ausOption[CLI_POS_WBM]) {
            PifAbort(MODULE_STB_ABORT, FAULT_BAD_ENVIRONMENT);
        }
        return;
    }

    if (1 == SysCon->ausOption[CLI_POS_WBM]) {
        CliNetConfig.fchStatus |= CLI_NET_BACKUP;
    }

    IPdata.usLport = CLI_PORT_STUB;
    IPdata.usFport = ANYPORT;
    sHandle = PifNetOpen(&IPdata);

    if (0 <= sHandle) {
        CliNetConfig.usHandle = (USHORT)sHandle;
        CliNetConfig.fchStatus |= CLI_NET_OPEN;
        PifNetControl(CliNetConfig.usHandle, PIF_NET_SET_MODE, (PIF_NET_NMODE | PIF_NET_TMODE));
    } else {
        PifAbort(MODULE_STB_ABORT, FAULT_BAD_ENVIRONMENT);
    }
}

/*
*===========================================================================
** Synopsis:    VOID    CstNetClose(VOID);
*
** Return:      none.
*
** Description:
*   This function closes NET for stub.
*===========================================================================
*/
VOID    CstNetClose(VOID)
{
    if (CliNetConfig.fchStatus & CLI_NET_OPEN) {    /* V3.3 */
        PifNetClose(CliNetConfig.usHandle);
		CliNetConfig.usHandle = -1;
        CliNetConfig.fchStatus &= ~CLI_NET_OPEN;
    }                                               /* V3.3 */
}

/*
*===========================================================================
** Synopsis:    VOID   CstNetSend(USHORT usSize);
*
*     InPut:    usSize  - number of bytes to be sent
*
** Return:      none.
*
** Description:
*   This function send the Data to specified server.
*===========================================================================
*/
SHORT   CstNetSend(USHORT usSize)
{
    SHORT   sError = STUB_NET_NOT_OPENED,
            sRetry=5;

    if (CliNetConfig.fchStatus & CLI_NET_OPEN) {

        CliNetConfig.fchStatus |= CLI_NET_SEND;

        do {                                    /* send message */
			int  iWsaError;
			CHAR   xBuff[256];

			sError = PifNetSend(CliNetConfig.usHandle, (VOID *)&CliSndBuf, usSize);
			iWsaError = PifWsaLastError ();

			sprintf (xBuff, "  CstNetSend: sError %d sError %d iWsaError %d ", sError, CliMsg.sError, iWsaError);
			CstLogNetSend (xBuff);
            if (sError < 0) {
                PifSleep(100);
            }
		} while ((PIF_ERROR_NET_WRITE == sError) && (--sRetry > 0));

        CliNetConfig.fchStatus &= ~CLI_NET_SEND;
		if (0 > sError) {
			CliMsg.sError = STUB_TIME_OUT;
		}
    }

	return sError;
}

/*
*===========================================================================
** Synopsis:    VOID   CstNetReceive(VOID);
*
** Return:      none.
*
** Description:
*   This function receive the data from server.
*===========================================================================
*/
SHORT   CstNetReceive(VOID)
{
    CLIREQCOM   *pSend;
    CLIRESCOM   *pResp;
    SHORT       sError;
    USHORT      usTimer, usPrevErrorCo, usRetry;

    if (0 > CliMsg.sError) {
        return CliMsg.sError;
    }
    if (0 == (CliNetConfig.fchStatus & CLI_NET_OPEN)) {
        CliMsg.sError = STUB_NET_NOT_OPENED;
        return CliMsg.sError;
    }
    usTimer = (USHORT)(CstReadMDCPara(CLI_MDC_CLIENT) * 1000);
    usRetry = 1;
    /* avoid time out, at PLU DB reset, 06/27/01 */
    pSend = (CLIREQCOM *)CliSndBuf.auchData;
    if ( (pSend->usFunCode & CLI_RSTCONTCODE ) == CLI_FCTTLRESETTL ) { /* add R3.0 */
        usTimer = 30000;                     /* 60 seconds */
        usRetry = 10;
    }
    /* avoid time out, at SerOpPluFileUpdate, 02/12/02 */
    if ( (pSend->usFunCode & CLI_RSTCONTCODE ) == CLI_FCOPPLUFILEUPD ) { /* add R3.0 */
        usTimer = 30000;                     /* 60 seconds */
        usRetry = 3;
    }
	if (pSend->usFunCode == CLI_FCISPLOGON) {
		// if this is a logon attempt then try once with short time out.
		usTimer = 20000;                     /* 20 seconds */
		usRetry = 1;
	}

    PifNetControl(CliNetConfig.usHandle, PIF_NET_SET_TIME, usTimer);

    usPrevErrorCo = 0;
    do {
		int  iWsaError;
		SHORT  sErrorSave;
		CHAR   xBuff[256];

        CliNetConfig.fchStatus |= CLI_NET_RECEIVE;
        sError = PifNetReceive(CliNetConfig.usHandle, (VOID *)&CliRcvBuf, sizeof(XGRAM));
		iWsaError = PifWsaLastError ();
		sErrorSave = sError;
        CliNetConfig.fchStatus &= ~CLI_NET_RECEIVE;

        if (0 > sError) {
            if (PIF_ERROR_NET_POWER_FAILURE == sError) {
                sError = STUB_POWER_DOWN;

			}
			else if (PIF_ERROR_NET_TIMEOUT == sError) {
				/* avoid time out, at PLU DB reset, 06/27/01 */
				usRetry--;
				if (usRetry == 0) {
					sError = STUB_TIME_OUT;
				}
				else {
					sError = STUB_ILLEGAL;
				}
			} else if (PIF_ERROR_NET_ERRORS == sError) {
				sError = STUB_FAITAL;
            } else { 
                usPrevErrorCo ++;
                if (usPrevErrorCo > 3) {
                    PifLog(MODULE_STB_LOG, LOG_ERROR_STB_3TIME_ERROR);
                    sError = STUB_TIME_OUT;
                } else {
                    sError = STUB_ILLEGAL;
                }
            }
        } else {
            pSend = (CLIREQCOM *)CliSndBuf.auchData;
            pResp = (CLIRESCOM *)CliRcvBuf.auchData;
            if (CliSndBuf.auchFaddr[CLI_POS_UA] != CliRcvBuf.auchFaddr[CLI_POS_UA]) {
                sError = STUB_ILLEGAL;
            } else if (pSend->usFunCode != pResp->usFunCode) {
                sError = STUB_ILLEGAL;
            } else if ((pSend->usSeqNo & CLI_SEQ_CONT) != (pResp->usSeqNo & CLI_SEQ_CONT)) {
                sError = STUB_ILLEGAL;
            } else {
                sError = STUB_SUCCESS;
            }
        }
		sprintf (xBuff, "  CstNetReceive: sErrorSave %d sError %d iWsaError %d ", sErrorSave, sError, iWsaError);
		CstLogNetRecv (xBuff);
    } while (STUB_ILLEGAL == sError);

    CliMsg.sError = sError;

	return CliMsg.sError;
}

/*===== END OF SOURCE =====*/

