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
#include    <pc2172.h>

#include    <pif.h>
#include    <memory.h>
#include    <log.h>
#include    <appllog.h>
#include    "csstubin.h"
#include "mypifdefs.h"

/*
------------------------------------------
    Internal Work Area (initialized)
------------------------------------------
*/
XGRAM           CliSndBuf;              /* Send message buffer */       
XGRAM           CliRcvBuf;              /* Receive message buffer */
USHORT          usCliTranNo;            /* transaction # of my terminal */
USHORT          husCliWaitDone;         /* semapho, STUB - SERVER */
USHORT          husCliExeNet;           /* semapho, APPLI - ISP */
USHORT          husCliExePara;          /* Semapho, ISP - APPL, para */
CLICOMIF        CliMsg;                 /* common message structure */
CLINETCONFIG    CliNetConfig;           /* NET control structure */
BOOL			fCliMultiRcv;			/* Multiple receive control */

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
    CstComChekInquiry();
    if (0 == (CstComReadStatus() & CLI_STS_M_UPDATE)) {
        return STUB_M_DOWN;
    }
    if (CLI_IAM_MASTER) {
        return STUB_SELF;
    }
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
VOID    CstComSendReceive(VOID)
{
    USHORT  usSendLen;

	fCliMultiRcv = FALSE;

    do {
        CstComChekInquiry();                    /* check inquiry condition */
        usSendLen = CstComMakeMultiData();      /* make request data */
        CstNetSend(usSendLen);                  /*=== SEND MESSAGE ===*/
        CstNetReceive();                        /*=== RECEIVE RESPONSE ===*/
        CstComCheckError();                     /* error check */
        CstComRespHandle();                     /* response handling */
        CstComErrHandle();                      /* error handling */
        CstComSendConfirm();                    /* send confirm, if need */    

    } while (STUB_SUCCESS != CliMsg.sError);
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

    if ((0 != CliMsg.usReqLen) && (! fCliMultiRcv) &&
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
		fCliMultiRcv = TRUE;
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
            if (CLI_IAM_MASTER) {
                CliMsg.sError = STUB_BM_DOWN;
            }
            if (CLI_IAM_BMASTER) {
                if (CstComReadStatus() & CLI_STS_M_OFFLINE) {
                    CliMsg.sError = STUB_M_DOWN;
                }
            }
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

    if (CLI_TGT_MASTER == CliSndBuf.auchFaddr[CLI_POS_UA]) {
        if (0 == (CstComReadStatus() & CLI_STS_M_UPDATE)) {
            CliMsg.sError = STUB_SUCCESS;
        }
    } else {
        if (0 == (CstComReadStatus() & CLI_STS_BM_UPDATE)) {
            CliMsg.sError = STUB_SUCCESS;
        }
    }
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
*   This function supports to display error code.
*===========================================================================
*/
VOID    CstDisplayError(SHORT sError)
{
    CliMsg.sError   = STUB_SUCCESS;
    CliMsg.sRetCode = (SHORT)(sError + STUB_PC_ERROR);
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
        PifNetControl(CliNetConfig.usHandle, 
                      PIF_NET_SET_MODE, 
                      PIF_NET_NMODE + PIF_NET_TMODE);
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
    SHORT   sError = -1,
            sRetry=5;

    if (CliNetConfig.fchStatus & CLI_NET_OPEN) {

        CliNetConfig.fchStatus |= CLI_NET_SEND;

        do {                                    /* send message */
            if (5 > sRetry) {
                PifSleep(100);
            }
            sError = PifNetSend(CliNetConfig.usHandle,   
                                (VOID FAR *)&CliSndBuf,
                                usSize);
        } while ((PIF_ERROR_NET_WRITE == sError) && (--sRetry > 0));

        CliNetConfig.fchStatus &= ~CLI_NET_SEND;
    }

    if (0 > sError) {
        CliMsg.sError = STUB_TIME_OUT;
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
VOID   CstNetReceive(VOID)
{
    CLIREQCOM   *pSend;
    CLIRESCOM   *pResp;
    SHORT       sError;
    USHORT      usTimer, usPrevErrorCo, usRetry;

    if (STUB_TIME_OUT == CliMsg.sError) {
        return;
    }
    if (0 == (CliNetConfig.fchStatus & CLI_NET_OPEN)) {
        CliMsg.sError = STUB_NET_NOT_OPENED;
        return;
    }
    usTimer = (USHORT)(CstReadMDCPara(CLI_MDC_CLIENT) * 1000);
    usRetry = 1;
    /* avoid time out, at PLU DB reset, 06/27/01 */
    pSend = (CLIREQCOM *)CliSndBuf.auchData;
    if ( (pSend->usFunCode & CLI_RSTCONTCODE ) == CLI_FCTTLRESETTL ) { /* add R3.0 */
        usTimer = 60000;                     /* 60 seconds */
        usRetry = 40;
    }
    /* avoid time out, at SerOpPluFileUpdate, 02/12/02 */
    if ( (pSend->usFunCode & CLI_RSTCONTCODE ) == CLI_FCOPPLUFILEUPD ) { /* add R3.0 */
        usTimer = 60000;                     /* 60 seconds */
        usRetry = 10;
    }
    PifNetControl(CliNetConfig.usHandle,
                  PIF_NET_SET_TIME,
                  usTimer);

    usPrevErrorCo = 0;
    do {
        CliNetConfig.fchStatus |= CLI_NET_RECEIVE;
        sError = PifNetReceive(CliNetConfig.usHandle,
                               (VOID FAR *)&CliRcvBuf,
                               sizeof(XGRAM));
        CliNetConfig.fchStatus &= ~CLI_NET_RECEIVE;

        if (0 > sError) {
            if (PIF_ERROR_NET_POWER_FAILURE == sError) {
                sError = STUB_POWER_DOWN;

            } else if (PIF_ERROR_NET_TIMEOUT == sError) {
			    /* avoid time out, at PLU DB reset, 06/27/01 */
                usRetry --;
                if ( usRetry == 0 ) {
                    sError = STUB_TIME_OUT;
                } else {
                    sError = STUB_ILLEGAL;
                }

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
            if (CliSndBuf.auchFaddr[CLI_POS_UA] !=
                CliRcvBuf.auchFaddr[CLI_POS_UA]) {
                sError = STUB_ILLEGAL;
            
            } else if (pSend->usFunCode != pResp->usFunCode) {
                sError = STUB_ILLEGAL;
             
            } else if ((pSend->usSeqNo & CLI_SEQ_CONT) !=
                       (pResp->usSeqNo & CLI_SEQ_CONT)) {
                sError = STUB_ILLEGAL;

            } else {
                sError = STUB_SUCCESS;
            }
        }
    } while (STUB_ILLEGAL == sError);

    CliMsg.sError = sError;
}

/*===== END OF SOURCE =====*/

