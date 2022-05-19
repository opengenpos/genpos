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
* Title       : SERVER module, Common Functions Source File
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVCOM.C                                            
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           SerIamMaster()          Check am I Master Terminal ?
*           SerIamBMaster()         Check am I Backup Master terminal ?
*           SerIamSatellite()     A Check am I Satellite Terminal ?
*
*           SerSendResponse()       Send Response
*           SerSendRequest()        Send Request
*           SerSendError()          Send Error Response
*
*           SerChangeStatus()     * Change server status
*           SerComReadStatus()      Read Communication Status
*           SerReadMDCPara()        Read MDC para
*
*           SerNetOpen()            Open Net work
*           SerNetClose()           Close Net work
*           SerNetSend()            Send to Net work
*           SerNetReceive()         Receive from Net work
*
*           SerSendNtbStart()       Send Notice Board (start)
*
*           SerTimerStart()         Start Timer
*           SerTimerStop()          Stop Timer
*           SerTimerRead()          Read Timer
*
*           SerKpsTimerStart()      Start Kitchen Printer Timer
*           SerKpsTimerStop()       Stop Kitchen Printer Timer
*           SerKpsTimerStopAll()    Stop Kitchen Printer Timer (all timer) (V1.0.0.4)
*           SerKpsTimerRead()       Read Kitchen Printer Timer
*
*           SerBcasTimerStart()     Start Broadcast Timer
*           SerBcasTimerStop()      Stop Broadcast  Timer
*           SerBcasTimerRead()      Read Broadcast  Timer
*
*           Ser_Timer_Start()       Timer Start common
*           Ser_Timer_Read()        Timer Read common
*
*           SerSpsTimerStart()    A Start Shared Printer Timer
*           SerSpsTimerStop()     A Stop  Shared Printer Timer
*           SerSpsTimerRead()     A Read  Shared Printer Timer
*
*           SerSendResponseFH()     Send Response(File Handle I/F Version)
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* Jun-05-93:H.Yamaguchi: Modified Enhance                                    
* Mar-18-94:K.You      : Add flex GC file feature(add SerSendResponseFH)                                   
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Aug-04-00 : V1.0.0.4 : change num. of kps timer (1 -> No.of KP)
*
** GenPOS **
* Sep-08-17 : 02.02.02 : R.Chambers   : SerNetOpen() return status, SerInitialize() decides PifAbort().
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
#include	<tchar.h>
#include	<math.h>
#include    <memory.h>
#include	<stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <nb.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <appllog.h>
#include    "servmin.h"
#include    <rfl.h>

/*
*===========================================================================
** Synopsis:    SHORT    SerIamMaster(VOID);
*
** Return:      SERV_SUCCESS:   I am Master Terminal
*               SERV_ILLEGAL:   I am not Mater Terminal
*
** Description: This function checks that am I a Mater or not.
*===========================================================================
*/
SHORT   SerIamMaster(VOID)
{
    if (SerNetConfig.auchFaddr[CLI_POS_UA] == CLI_TGT_MASTER) {
        return SERV_SUCCESS;
    } 
    return SERV_ILLEGAL;
}

/*
*===========================================================================
** Synopsis:    SHORT    SerIamBMaster(VOID);
*
** Return:      SERV_SUCCESS:   I am Back Up Master Terminal
*               SERV_ILLEGAL:   I am not Back Up Mater Terminal
*
** Description:  This function checks that am I a Backup Mater or not.
*===========================================================================
*/
SHORT   SerIamBMaster(VOID)
{
    if ((SerNetConfig.auchFaddr[CLI_POS_UA] == CLI_TGT_BMASTER) &&
        (SerNetConfig.fchStatus & SER_NET_BACKUP)) {
        return SERV_SUCCESS;
    }
    return SERV_ILLEGAL;
}


/*
*===========================================================================
** Synopsis:    SHORT    SerIamDisconnectedSatellite(VOID);
*
** Return:      STUB_SUCCESS:   I am Disconnected Satellite Terminal
*               STUB_ILLEGAL:   I am not Satellite Terminal
*
** Description:  This function check "Am I satellite"
*===========================================================================
*/
SHORT   SerIamDisconnectedSatellite(VOID)
{
    if (SerNetConfig.fchSatStatus & SER_SAT_DISCONNECTED) {
        return STUB_SUCCESS;
    }
	return STUB_ILLEGAL;
}


/*
*===========================================================================
** Synopsis:    SHORT    SerIamDisconnectedUnjoinedSatellite(VOID);
*
** Return:      STUB_SUCCESS:   I am Disconnected Satellite Terminal that is not
*                               joined to a cluster.
*               STUB_ILLEGAL:   I am not Satellite Terminal
*
** Description:  This function check "Am I satellite"
*===========================================================================
*/
SHORT   SerIamDisconnectedUnjoinedSatellite(VOID)
{
	// If we are a disconnected Satellite and we are not joined to a cluster
	// then indicate success.
    if ((SerNetConfig.fchSatStatus & (SER_SAT_DISCONNECTED | SER_SAT_JOINED)) == SER_SAT_DISCONNECTED) {
        return STUB_SUCCESS;
    }
	return STUB_ILLEGAL;
}

/*
*===========================================================================
** Synopsis:    SHORT    SerIamSatellite(VOID);
*
** Return:      SERV_SUCCESS:   I am Satellite Terminal
*               SERV_ILLEGAL:   I am not Satellite Terminal
*
** Description: This function check "Am I Satellite".
*===========================================================================
*/
SHORT   SerIamSatellite(VOID)
{
    if (SerNetConfig.auchFaddr[CLI_POS_UA] == CLI_TGT_MASTER) {
        return SERV_ILLEGAL;
    } 

    if ((SerNetConfig.auchFaddr[CLI_POS_UA] == CLI_TGT_BMASTER) && (SerNetConfig.fchStatus & SER_NET_BACKUP)) {
        return SERV_ILLEGAL;
    }

	if (STUB_SUCCESS == SerIamDisconnectedUnjoinedSatellite()) {
        return SERV_ILLEGAL;
    }
    return SERV_SUCCESS;
}

SHORT SerResponseTargetTerminal (VOID)
{
	SHORT  sRetStatus = -1;

#if 0
	// this check removed but kept for future usage if problems develop with this functionality.
	{
		char  xBuff[128];
		sprintf (xBuff, "SerResponseTargetTerminal(): %d, %d", SerRcvBuf.auchFaddr[0], SerRcvBuf.auchFaddr[3]);
		NHPOS_ASSERT_TEXT(0, xBuff);
	}
#endif

	if (SerRcvBuf.auchFaddr[0] == 192 && SerRcvBuf.auchFaddr[1] == 0) {
		if (SerRcvBuf.auchFaddr[3] > 0 && SerRcvBuf.auchFaddr[3] <= PIF_NET_MAX_IP) {
			sRetStatus = SerRcvBuf.auchFaddr[3];
		}
	}

	return sRetStatus;
}

/*
*===========================================================================
** Synopsis:    VOID    SerSendResponse(CLIRESCOM *pResMsgH,
*                                       USHORT usResMsgHLen,
*                                       UCHAR *puchReqData,
*                                       USHORT usReqLen);
*
*     InPut:    pResMsgH    - response message header (now responded)
*               usResMsgHLen    - response message header length
*               puchReqData     - send data pointer
*               usReqLen    - send data length
*
** Return:      none.      
*
** Description: This function sends response.
*===========================================================================
*/
SHORT   SerSendResponse(CLIRESCOM *pResMsgH,
                        USHORT usResMsgHLen,
                        UCHAR *puchReqData,
                        USHORT usReqLen)
{
    USHORT  usSendLen;
	SHORT   sError;
                                                /*=== EDIT HEADER ===*/       
//    memcpy(SerSndBuf.auchFaddr, SerNetConfig.auchFaddr, PIF_LEN_IP);
//    SerSndBuf.auchFaddr[CLI_POS_UA] = SerRcvBuf.auchFaddr[CLI_POS_UA];
	memcpy (SerSndBuf.auchFaddr, SerRcvBuf.auchFaddr, PIF_LEN_IP);
    SerSndBuf.usFport = SerRcvBuf.usFport;
    SerSndBuf.usLport = CLI_PORT_SERVER;
    usSendLen = PIF_LEN_IP + 4;
                                                /*=== EDIT MESSAGE ===*/
    memcpy(SerSndBuf.auchData, pResMsgH, usResMsgHLen);
    usSendLen += usResMsgHLen;

    if (NULL != puchReqData) {                  /*=== EDIT DATA ===*/
        memcpy(&SerSndBuf.auchData[usResMsgHLen], &usReqLen, 2);
        memcpy(&SerSndBuf.auchData[usResMsgHLen + 2], puchReqData, usReqLen);
        usSendLen += usReqLen + 2;
    }
                                                /*=== SEND RESPONSE ===*/
    sError = SerNetSend(usSendLen);
	if (sError < 0) {
		char xBuff[128];
		sprintf(xBuff, "SerNetSend(): sError = %d, 0x%x, usFunCode 0x%x, usSeqNo 0x%x", sError, *((ULONG *)&SerSndBuf.auchFaddr[0]), pResMsgH->usFunCode, pResMsgH->usSeqNo);
		NHPOS_ASSERT_TEXT((sError >= 0), xBuff);
	}

	return sError;
}

SHORT   SerSendResponseToIpAddress(UCHAR *auchFaddr, CLIRESCOM *pResMsgH,
                        USHORT usResMsgHLen,
                        UCHAR *puchReqData,
                        USHORT usReqLen)
{
    USHORT  usSendLen;
	SHORT   sError;
                                                /*=== EDIT HEADER ===*/       
    memcpy(SerSndBuf.auchFaddr, auchFaddr, PIF_LEN_IP);
    SerSndBuf.usFport = SerRcvBuf.usFport;
    SerSndBuf.usLport = CLI_PORT_SERVER;
    usSendLen = PIF_LEN_IP + 4;
                                                /*=== EDIT MESSAGE ===*/
    memcpy(SerSndBuf.auchData, pResMsgH, usResMsgHLen);
    usSendLen += usResMsgHLen;

    if (NULL != puchReqData) {                  /*=== EDIT DATA ===*/
        memcpy(&SerSndBuf.auchData[usResMsgHLen], &usReqLen, 2);
        memcpy(&SerSndBuf.auchData[usResMsgHLen + 2], puchReqData, usReqLen);
        usSendLen += usReqLen + 2;
    }
                                                /*=== SEND RESPONSE ===*/
    sError = SerNetSend(usSendLen);
	if (sError < 0) {
		char xBuff[128];
		sprintf(xBuff, "SerNetSend(): sError = %d, 0x%x, usFunCode 0x%x, usSeqNo 0x%x", sError, *((ULONG *)&SerSndBuf.auchFaddr[0]), pResMsgH->usFunCode, pResMsgH->usSeqNo);
		NHPOS_ASSERT_TEXT((sError >= 0), xBuff);
	}

	return sError;
}

/*
*===========================================================================
** Synopsis:    VOID    SerSendRequest( UCHAR uchServer,
*                                       CLIREQCOM *pReqMsgH,
*                                       USHORT usReqMsgHLen,
*                                       UCHAR *puchReqData,
*                                       USHORT usReqLen);
*
*     InPut:    uchServer   - terget server unique address
*               pReqMsgH    - request message header (now responded)
*               usReqMsgHLen    - request message header length
*               puchReqData     - send data pointer
*               usReqLen    - send data length
*
** Return:      none.      
*
** Description: This function sends request to BM or MT.
*===========================================================================
*/
SHORT    SerSendRequest( UCHAR uchServer,
                        CLIREQCOM *pReqMsgH,
                        USHORT usReqMsgHLen,
                        UCHAR *puchReqData,
                        USHORT usReqLen)
{
    USHORT  usSendLen;
	SHORT   sError;
                                                /*=== EDIT HEADER ===*/       
    memcpy(SerSndBuf.auchFaddr, SerNetConfig.auchFaddr, PIF_LEN_IP);
    SerSndBuf.auchFaddr[CLI_POS_UA] = uchServer;
    SerSndBuf.usFport = CLI_PORT_SERVER;
    SerSndBuf.usLport = CLI_PORT_SERVER;
    usSendLen = PIF_LEN_IP + 4;
                                                /*=== EDIT MESSAGE ===*/
    memcpy(SerSndBuf.auchData, pReqMsgH, usReqMsgHLen);
    usSendLen += usReqMsgHLen;

    if (NULL != puchReqData) {                  /*=== EDIT DATA ===*/
        memcpy(&SerSndBuf.auchData[usReqMsgHLen], &usReqLen, 2);
        memcpy(&SerSndBuf.auchData[usReqMsgHLen + 2], puchReqData, usReqLen);
        usSendLen += usReqLen + 2;
    }
                                                /*=== SEND REQUEST ===*/
	sError = SerNetSend(usSendLen);
	if (sError < 0) {
		char xBuff[128];
		sprintf(xBuff, "SerSendRequest(): sError = %d, 0x%x, usFunCode 0x%x, usSeqNo 0x%x", sError, *((ULONG *)&SerSndBuf.auchFaddr[0]), pReqMsgH->usFunCode, pReqMsgH->usSeqNo);
		NHPOS_ASSERT_TEXT((sError >= 0), xBuff);
	}
	return sError;
}

/*
*===========================================================================
** Synopsis:    VOID    SerSendError(SHORT sError);
*
*     InPut:    sError  - Error Code
*
** Return:      none.      
*
** Description: This function sends error response.

				The basic protocol is to send a message that contains
				the following information in the following struct members
					pResMsgH->sReturn    indicates whether the message was processed by server
					                         contains STUB_SUCCESS if it was.
					                         same as pCliMsg->sRetCode
					pResMsgH->sResCode   indicates the response code for the message by the
					                         function used by the server such as 

				Take a look at function CstNetReceive () in file csstbcom.c
				within the Client subsystem for an example of a function that
				is expecting a response from the server.

				Look at function CstComSendReceive () in the same file for
				a complete review of the application layer protocol used
				between Server and Client for sending and receiving messages.
*===========================================================================
*/
SHORT   SerSendError(SHORT sError)
{
    CLIREQCOM   *pReqMsgH = (CLIREQCOM *)SerRcvBuf.auchData;
    CLIRESCOM   *pResMsgH = (CLIRESCOM *)SerSndBuf.auchData;
	SHORT        sMyError;
                                                /*=== EDIT HEADER ===*/       
    memcpy(SerSndBuf.auchFaddr, SerNetConfig.auchFaddr, PIF_LEN_IP);
    SerSndBuf.auchFaddr[CLI_POS_UA] = SerRcvBuf.auchFaddr[CLI_POS_UA];
    SerSndBuf.usFport = SerRcvBuf.usFport;
    SerSndBuf.usLport = CLI_PORT_SERVER;
                                                /*=== EDIT MESSAGE ===*/
    pResMsgH->usFunCode = pReqMsgH->usFunCode;
    pResMsgH->sResCode  = sError;
    pResMsgH->usSeqNo   = pReqMsgH->usSeqNo;
    pResMsgH->sReturn   = STUB_SUCCESS;

                                                /*=== SEND RESPONSE ===*/
    sMyError = SerNetSend(PIF_LEN_IP + 4 + sizeof(CLIRESCOM));
	if (sMyError < 0) {
		char xBuff[128];
		sprintf(xBuff, "SerSendError(): sMyError = %d, 0x%x, usFunCode 0x%x, usSeqNo 0x%x", sMyError, *((ULONG *)&SerSndBuf.auchFaddr[0]), pReqMsgH->usFunCode, pReqMsgH->usSeqNo);
		NHPOS_ASSERT_TEXT((sMyError >= 0), xBuff);
	}

	return sMyError;
}

/*
*===========================================================================
** Synopsis:    VOID    SerChangeStatus(USHORT usStatus);
*
*     InPut:    usStatus    - New Status
*
** Return:      none.
*
** Description: This function changes server status.
				The SerChangeStatus () function is used in various places within the
				Server thread to change the server state from its current state to
				a new state.
*===========================================================================
*/
VOID    SerChangeStatus(USHORT usStatus)
{
    USHORT  usTermFlag;

    if (SER_IAM_MASTER) {
        usTermFlag = NB_MTBAKCOPY_FLAG;
    } else {
        usTermFlag = NB_BMBAKCOPY_FLAG;
    }

    if (SER_IAM_SATELLITE) {       /* If iam satellite, then execute */

        switch(usStatus) {
        case    SER_STMULSND:                   /* into MULTI SEND state */
        case    SER_STMULRCV:                   /* into MULTI RECEIVE state */
            SerTimerStart();                    /* start timer */
            usSerStatus = usStatus;
            break;
        
        default:                                /* not used */
        /* case    SER_STNORMAL:  */            /* into NORMAL state */
            SerTimerStop();                     /* stop timer */
            SerResp.usTimeOutCo = 0;            /* clear time out counter */
            SstResetFlag(SER_SPESTS_INQUIRY);   /* reset spe. flg. INQ */
            SstResetFlag(SER_SPESTS_INQTIMEOUT);/* reset spe. flg. INQ, TIMEOUT */
            usSerStatus = SER_STNORMAL;
            break;
        }

    }  else {   /*  If terminal is Master or B-Master */

        switch(usStatus) {
        case    SER_STNORMAL:                   /* into NORMAL state */
            SerTimerStop();                     /* stop timer */
            SerResp.usTimeOutCo = 0;            /* clear time out counter */
            SstResetFlag(SER_SPESTS_INQUIRY);   /* reset spe. flg. INQ */
            SstResetFlag(SER_SPESTS_INQTIMEOUT);/* reset spe. flg. INQ, TIMEOUT */
            NbWriteInfFlag(usTermFlag, 0);
            usSerStatus = SER_STNORMAL;
            break;
        
        case    SER_STINQUIRY:                  /* into INQUIRY state */
			if (usSerStatus != SER_STINQUIRY) {
				PifLog(MODULE_SER_LOG, LOG_ERROR_SER_INQUIRY_START);
			}
            SerTimerStop();                     /* stop timer */
            SerResp.usTimeOutCo = 0;            /* clear time out counter */
            SstSetFlag(SER_SPESTS_INQUIRY);     /* set spe. flg. INQ */
            SstResetFlag(SER_SPESTS_INQTIMEOUT);/* reset spe. flg. INQ, TIMEOUT */
            NbWriteInfFlag(usTermFlag, NB_STOPALLFUN);
            NbStartInquiry();
            usSerStatus = SER_STINQUIRY;
            break;
        
        case    SER_STBACKUP:                   /* into BACK UP COPY state */
            SerTimerStart();                    /* start timer */
            NbWriteInfFlag(usTermFlag, NB_STOPALLFUN);
            usSerStatus = usStatus;
            break;
            
        case    SER_STMULSND:                   /* into MULTI SEND state */
        case    SER_STMULRCV:                   /* into MULTI RECEIVE state */
            SerTimerStart();                    /* start timer */
            usSerStatus = usStatus;
            break;
        
        default:                                /* not used */
            break;
        }
    }
}

/*
*===========================================================================
** Synopsis:    USHORT  SerComReadStatus(VOID);
*
** Return:      Communication Status.
*
** Description: This function reads comminication (condition) status.
*===========================================================================
*/
USHORT  SerComReadStatus(VOID)
{
    USHORT      fsComStatus;
    USHORT      fsServerFlag;
    NBMESSAGE   NbInf;

    fsComStatus = 0;

    NbReadAllMessage(&NbInf);
    if (NbInf.fsSystemInf & NB_MTUPTODATE) {
        fsComStatus |= SER_COMSTS_M_UPDATE;
    }
    if (NbInf.fsSystemInf & NB_BMUPTODATE) {
        fsComStatus |= SER_COMSTS_BM_UPDATE;
    }
    if (0 == (NbInf.fsSystemInf & NB_SETMTONLINE)) {
        fsComStatus |= SER_COMSTS_M_OFFLINE;
    }
    if (0 == (NbInf.fsSystemInf & NB_SETBMONLINE)) {
        fsComStatus |= SER_COMSTS_BM_OFFLINE;
    }
    if (0 == (NbInf.fsSystemInf & NB_WITHOUTBM)) {
        fsComStatus |= SER_COMSTS_BM_ENABLED;
    }
    SstReadFlag(&fsServerFlag);
    if (fsServerFlag & SER_SPESTS_INQUIRY) {
        fsComStatus |= SER_COMSTS_INQUIRY;
    }
    if (fsServerFlag & SER_SPESTS_INQTIMEOUT) {
        fsComStatus |= SER_COMSTS_INQTIMEOUT;
    }
    return fsComStatus;
}

/*
*===========================================================================
** Synopsis:    USHORT    SerReadMDCPara(UCHAR address);
*
*     Input:    address - MDC address
*
** Return:      MDC Value.
*
** Description: This function reads MDC Parameter.
*===========================================================================
*/
USHORT   SerReadMDCPara(UCHAR address)
{
    USHORT  usValue;

    switch(address) {

    case    CLI_MDC_SERVER:         /* response timer */
        usValue = (USHORT)CliParaMDCCheck(MDC_CLI1_ADR, 0xff);
        if (0 == usValue) {
            usValue = CLI_DEFMDC_SERVER;
        }
        if (60 < usValue) {
            usValue = 60;
        }
        break;

    case    CLI_MDC_MULTI:          /* retry counter */
        usValue = (USHORT)CliParaMDCCheck(MDC_CLI7_ADR, 0xff);
        if (0 == usValue) {
            usValue = CLI_DEFMDC_MULTI;
        }
        if (60 < usValue) {
            usValue = 60;
        }
        break;

    case    CLI_MDC_BACKUP:          /* retry counter */
		//we set this value to CLI_DEFMDC_BACKUP so that
		//we have enough time to let the terminal restore
		//the database JHHJ
        usValue = CLI_DEFMDC_BACKUP;
        
        break;
    
    case    CLI_MDC_SHARED:          /* Shared printer */
        usValue = (USHORT)CliParaMDCCheck(MDC_SHR_MON_TIME_ADR, 0x0f);
        if ( 0 == fsSerShrStatus) {      /* Shred printer status  */
            /* Prevent from momentarily Power up/down at satellite */  
            usValue = CLI_DEFMDC_MULTI + CLI_DEFMDC_MULTI;  
        } else {
            if ( 0 == usValue ) {
                usValue = CLI_DEFMDC_SHARED;    /* Default is 5 minutes */
            } else {
                usValue &= 0x0f;
            }
            usValue *= 60;      /* 1 = 60 seconds */
        }
        break;

    default:                        /* not used */
        usValue = 0;
        break;
    }
    return usValue;
}

/*
*===========================================================================
** Synopsis:    VOID    SerNetOpen(VOID);
*
** Return:      none.
*
** Description: This function opens NET.
*===========================================================================
*/
SHORT  SerNetOpen(VOID)
{
    SYSCONFIG CONST *SysCon = PifSysConfig();
	XGHEADER        IPdata = {0};
    SHORT           sHandle;
	SHORT           sRetStatus = 0;

    memset(&SerNetConfig, 0, sizeof(SERNETCONFIG));

	if (SysCon->usTerminalPosition & PIF_CLUSTER_DISCONNECTED_SAT)
		SerNetConfig.fchSatStatus |= SER_SAT_DISCONNECTED;

	if (SysCon->usTerminalPosition & PIF_CLUSTER_JOINED_SAT)
		SerNetConfig.fchSatStatus |= SER_SAT_JOINED;

    memcpy(SerNetConfig.auchFaddr, SysCon->auchLaddr, PIF_LEN_IP);

    if (((0 == SysCon->auchLaddr[0]) &&
         (0 == SysCon->auchLaddr[1]) &&
         (0 == SysCon->auchLaddr[2])) ||    /* comm. board not provide */
        (0 == SysCon->auchLaddr[3])) {

        PifEndThread();                     /* end of thread */
    }
    if (1== usBMOption) {    /* saratoga */
        SerNetConfig.fchStatus |= SER_NET_BACKUP;
    }

    IPdata.usLport = CLI_PORT_SERVER;
    IPdata.usFport = ANYPORT;
    sRetStatus = sHandle = PifNetOpen(&IPdata);
    if (0 <= sHandle) {
        SerNetConfig.usHandle = (USHORT)sHandle;
        SerNetConfig.fchStatus |= SER_NET_OPEN;
        PifNetControl(SerNetConfig.usHandle, PIF_NET_SET_MODE, PIF_NET_NMODE | PIF_NET_TMODE);
	}

	return sRetStatus;
}

/*
*===========================================================================
** Synopsis:    VOID    SerNetClose(VOID);
*
** Return:      none.
*
** Description: This function closes NET.
*===========================================================================
*/
VOID    SerNetClose(VOID)
{
    PifNetClose(SerNetConfig.usHandle);
    SerNetConfig.fchStatus &= ~SER_NET_OPEN;
}

/*
*===========================================================================
** Synopsis:    VOID    SerNetSend(USHORT usSendData);
*
*     Input:    usSendData  - Data length to send now
*
** Return:      none.
*
** Description: This function sends thru my port.
				This function is used in a large number of places to send
				a message.
*===========================================================================
*/
SHORT    SerNetSend(USHORT usSendData)
{
    SHORT   sError = PIF_OK;
	SHORT   sRetry=3;

    SerNetConfig.fchStatus |= SER_NET_SEND;
	// Set a timeout for the send so that if the server is
	// busy, we will get notified immediately.  Look at
	// SerRecvNextFrame () in servrmh.c in the Server subsystem
	// for an example of Server responding with STUB_BUSY.
	// We are using a time out of 2500 milliseconds in order to 
	// accomodate use with MWS polling over the Internet with its
	// latency and delays.
	PifNetControl(SerNetConfig.usHandle, PIF_NET_SET_STIME, 3000);
    sError = PifNetSend(SerNetConfig.usHandle, &SerSndBuf, usSendData);
    SerNetConfig.fchStatus &= ~SER_NET_SEND;

	return sError;
}

/*
*===========================================================================
** Synopsis:    VOID    SerNetReceive(VOID);
*
** Return:      none.
*
** Description: This function receive thru. my port.
*===========================================================================
*/
SHORT   SerNetReceive(VOID)
{
    SHORT       sError;
    USHORT      usTimer, usPrevErrorCo;

    usTimer = SerReadMDCPara(CLI_MDC_SERVER);
    usTimer *= 1000;
    PifNetControl(SerNetConfig.usHandle, PIF_NET_SET_TIME, usTimer);

    usPrevErrorCo = 0;
    do {
        SerNetConfig.fchStatus |= SER_NET_RECEIVE;
        sError = PifNetReceive(SerNetConfig.usHandle, &SerRcvBuf, sizeof(XGRAM));
        SerNetConfig.fchStatus &= ~SER_NET_RECEIVE;

        if (0 > sError) {
            if (PIF_ERROR_NET_POWER_FAILURE == sError) {
                SerResp.sError = SERV_POWER_DOWN;
            } else if (PIF_ERROR_NET_TIMEOUT == sError || PIF_ERROR_NET_DISCOVERY == sError || PIF_ERROR_NET_CLEAR == sError) {
                SerResp.sError = SERV_TIME_OUT;
            } else {
                usPrevErrorCo ++;
                if (usPrevErrorCo > 2) {
                    PifLog(MODULE_SER_LOG, LOG_ERROR_SER_3TIME_ERROR);
                    PifLog(MODULE_SER_LOG, (USHORT)abs(sError));
                    SerResp.sError = SERV_TIME_OUT;
                } else {
                    SerResp.sError = SERV_ILLEGAL;
                }
            }
        } else {
            SerResp.usSize = (USHORT)sError;
            SerResp.sError = SERV_SUCCESS;
        }
    } while (SERV_ILLEGAL == SerResp.sError);

	if (0 > sError && PIF_ERROR_NET_TIMEOUT != sError && PIF_ERROR_NET_DISCOVERY != sError) {
		char  xBuff[128];
		sprintf (xBuff, "SerNetReceive(): sError = %d", sError);
		NHPOS_ASSERT_TEXT((sError != PIF_ERROR_NET_TIMEOUT), xBuff);
	}
	return sError;
}

/*
*===========================================================================
** Synopsis:    VOID    SerSendNtbStart(VOID);
*
** Return:      none.
*
** Description:
*   This function requests to start As Master or On-line information.
*===========================================================================
*/
VOID    SerSendNtbStart(VOID)
{
    USHORT          fsFlag=0;
    SERINQSTATUS    InqData;

    SstReadInqStat(&InqData);
    if (InqData.usStatus & SER_INQ_M_UPDATE) {
        fsFlag |= NB_MTUPTODATE;
    }
    if (InqData.usStatus & SER_INQ_BM_UPDATE) {
        fsFlag |= NB_BMUPTODATE;
    }
    if (SER_IAM_MASTER) {
        NbWriteInfFlag(NB_MTBAKCOPY_FLAG, 0);
        if (fsFlag & NB_MTUPTODATE) {
            NbStartAsMaster(fsFlag);            /* Start As Master */
        } else {
            NbStartOnline(fsFlag);              /* Start As Satelleite */
        }
    }
    if (SER_IAM_BMASTER) {                      
        NbWriteInfFlag(NB_BMBAKCOPY_FLAG, 0);
        if (fsFlag & NB_MTUPTODATE) {          
            NbStartOnline(fsFlag);              /* Start As Back Up or ST */
        } else {
            NbStartAsMaster(fsFlag);            /* Start As Master */
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerTimerStart(VOID);
*
** Return:      none.
*
** Description: This function starts Timer.
*===========================================================================
*/
VOID    SerTimerStart(VOID)
{
    Ser_Timer_Start(&SerTimer, 0);
}

/*
*===========================================================================
** Synopsis:    VOID    SerTimerStop(VOID);
*
** Return:      none.
*
** Description: This function stops Timer.
*===========================================================================
*/
VOID    SerTimerStop(VOID)
{
}

/*
*===========================================================================
** Synopsis:    SHORT    SerTimerRead(VOID);
*
** Return:      SERV_SUCCESS:   Timer Not expired.
*               SERV_TIMER_EXPIRED: Timer Expired.
*
** Description: This function reads and checks Timer.
*===========================================================================
*/
SHORT    SerTimerRead(UCHAR uchAddress)
{
    ULONG       ulMAX;

    ulMAX = (ULONG)SerReadMDCPara(uchAddress);

    if (Ser_Timer_Read(&SerTimer, ulMAX) < 0) {
        return SERV_TIMER_EXPIRED;
    }

    return SERV_SUCCESS;
}
    
/*
*===========================================================================
** Synopsis:    VOID    SerKpsTimerStart(UCHAR);
*
** Input:       uchKpsNo KP # (1 - KPS_NUMBER_OF_PRINTER)
*
** Return:      none.
*
** Description: This function starts Kitchin Printer Timer.
*===========================================================================
*/
VOID    SerKpsTimerStart(UCHAR uchKpNo)   /* V1.0.0.4 */
{
    UCHAR  uchKpIdx = uchKpNo - 1;

	if (0 < uchKpNo && uchKpNo <= SER_MAX_KP) {
		Ser_Timer_Start(&SerKpsTimer[uchKpIdx], uchKpNo);
	}
}

/*
*===========================================================================
** Synopsis:    VOID    SerKpsTimerStop(UCHAR);
*
** Input:       uchKpsNo KP # (1 - KPS_NUMBER_OF_PRINTER)
*
** Return:      none.
*
** Description: This function stops Kitchin Printer Timer.
*===========================================================================
*/
VOID    SerKpsTimerStop(UCHAR uchKpNo)    /* V1.0.0.4 */
{
    UCHAR  uchKpIdx = uchKpNo - 1;

	if (0 < uchKpNo && uchKpNo <= SER_MAX_KP) {
		if (SerKpsTimerRead (uchKpNo) == SERV_TIMER_EXPIRED) {
			PifLog (MODULE_SER_LOG, LOG_EVENT_KPS_TIMERSTOP_EXPIRED);
		}
		SerKpsTimer[uchKpIdx].uchHour = 0;    
		SerKpsTimer[uchKpIdx].uchMin = 0;
		SerKpsTimer[uchKpIdx].uchSec = 0;
		SerKpsTimer[uchKpIdx].uchTermNo = 0;
	}
}

/*
*===========================================================================
** Synopsis:    VOID    SerKpsTimerStopAll(VOID);
*
** Input:       none.
*
** Return:      none.
*
** Description: This function stops all of Kitchin Printer Timer.
*               This called also from Kps module
*===========================================================================
*/
VOID    SerKpsTimerStopAll(VOID)    /* V1.0.0.4 */
{
    UCHAR uchKpIdx;

    for (uchKpIdx = 0; uchKpIdx < SER_MAX_KP; uchKpIdx++) {
        SerKpsTimer[uchKpIdx].uchHour = 0;
        SerKpsTimer[uchKpIdx].uchMin = 0;
        SerKpsTimer[uchKpIdx].uchSec = 0;
		SerKpsTimer[uchKpIdx].uchTermNo = 0;
    }
}

/*
*===========================================================================
** Synopsis:    SHORT    SerKpsTimerRead(UCHAR);
*
** Input:       uchKpsNo KP # (1 - SER_MAX_KP)
*
** Return:      SERV_SUCCESS:   Timer Not expired.
*               SERV_TIMER_EXPIRED: Timer Expired.
*
** Description: This function reads and checks Timer.
*===========================================================================
*/
SHORT    SerKpsTimerRead(UCHAR uchKpNo)    /* V1.0.0.4 */
{
    ULONG  ulMAX;
    UCHAR  uchKpIdx = uchKpNo - 1;

    if ((0 == SerKpsTimer[uchKpIdx].uchHour) &&              /* 00:00:00 ? */
        (0 == SerKpsTimer[uchKpIdx].uchMin) &&
        (0 == SerKpsTimer[uchKpIdx].uchSec)) {
        return SERV_SUCCESS;
    }

    ulMAX = (ULONG)SerReadMDCPara(CLI_MDC_MULTI);

    if (Ser_Timer_Read(&(SerKpsTimer[uchKpIdx]), ulMAX) < 0) {
        return SERV_TIMER_EXPIRED;
    }

    return SERV_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    VOID    SerBcasTimerStart(VOID);
*
** Return:      none.
*
** Description: This function starts Timer.
*===========================================================================
*/
VOID    SerBcasTimerStart(VOID)
{
    Ser_Timer_Start(&SerBcasTimer, 0);
}

/*
*===========================================================================
** Synopsis:    VOID    SerBcasTimerStop(VOID);
*
** Return:      none.
*
** Description: This function stops Timer.
*===========================================================================
*/
VOID    SerBcasTimerStop(VOID)
{
}

/*
*===========================================================================
** Synopsis:    SHORT    SerBcasTimerRead(VOID);
*
** Return:      SERV_SUCCESS:   Timer Not expired.
*               SERV_TIMER_EXPIRED: Timer Expired.
*
** Description: This function reads and checks Timer.
*===========================================================================
*/
SHORT    SerBcasTimerRead(VOID)
{
    ULONG       ulMAX;

    if (0 == SstChkBcasing()) {
        return SERV_TIMER_EXPIRED;
    }

    ulMAX = (ULONG)SerReadMDCPara(CLI_MDC_MULTI);

    if (Ser_Timer_Read(&SerBcasTimer, ulMAX) < 0) {
        return SERV_TIMER_EXPIRED;
    }

    return SERV_SUCCESS;
}
    
/*
*===========================================================================
** Synopsis:    VOID    Ser_Timer_Start(SERTIMER *pTim);
*
*     InPut:    pTim    - pointer to Timer structure
*
** Return:      none.
*
** Description: This function starts Kitchin Printer Timer.
*===========================================================================
*/
VOID    Ser_Timer_Start(SERTIMER *pTim, UCHAR uchTermNo)
{
    DATE_TIME   Timer;

    PifGetDateTime(&Timer);

    pTim->uchHour = (UCHAR)Timer.usHour;
    pTim->uchMin  = (UCHAR)Timer.usMin;
    pTim->uchSec  = (UCHAR)Timer.usSec;
	pTim->uchTermNo = uchTermNo;

    if ((0 == Timer.usHour) &&              /* 00:00:00 ? */
        (0 == Timer.usMin) &&
        (0 == Timer.usSec)) {
        pTim->uchSec = 1;                   /* -> 00:00:01 */
    }
}

/*
*===========================================================================
** Synopsis:    SHORT    Ser_Timer_Read(SERTIMER *pTim, ULONG ulMAX);
*
*     InPut:    pTim    - pointer to Timer structure
*               ulMAX   - timer value expired
*
** Return:      SERV_SUCCESS:   Timer Not expired.
*               SERV_TIMER_EXPIRED: Timer Expired.
*
** Description: This function reads and checks Timer.
*===========================================================================
*/
SHORT   Ser_Timer_Read(SERTIMER *pTim, ULONG ulMAX)
{
    DATE_TIME   Timer;
    SERTIMER    CurTime;
    ULONG       ulOLD, ulNEW;
    
    PifGetDateTime(&Timer);
                    
    CurTime.uchHour = (UCHAR)Timer.usHour;
    CurTime.uchMin  = (UCHAR)Timer.usMin;
    CurTime.uchSec  = (UCHAR)Timer.usSec;

    ulOLD = (pTim->uchHour * 3600L) + (pTim->uchMin * 60L) + pTim->uchSec;
    ulNEW = (CurTime.uchHour * 3600L) + (CurTime.uchMin * 60L) + CurTime.uchSec;
    
    if (ulOLD > ulNEW) {
        ulNEW += 24 * 3600L;
    }

    if (ulMAX < (ulNEW - ulOLD)) {
        return SERV_TIMER_EXPIRED;
    }
    return SERV_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    VOID    SerSpsTimerStart(UCHAR uchAddr);
*
*   InPut:     uchAddr    - Terminal Address
** Return:      none.
*
** Description: This function starts Shared Printer Timer.
*===========================================================================
*/
VOID    SerSpsTimerStart(UCHAR uchAddr)
{
    Ser_Timer_Start(&SerSpsTimer, uchAddr);
}

UCHAR   SerSpsTimerTermNo(VOID)
{
    return SerSpsTimer.uchTermNo ;
}

/*
*===========================================================================
** Synopsis:    VOID    SerSpsTimerStop(VOID);
*
** Return:      none.
*
** Description: This function stops Shared Printer Timer.
*===========================================================================
*/
VOID    SerSpsTimerStop(VOID)
{
	if (SerSpsTimerRead () == SERV_TIMER_EXPIRED) {
		PifLog (MODULE_SER_LOG, LOG_EVENT_PRINT_TIMERSTOP_EXPIRED);
	}
    SerSpsTimer.uchHour = 0;    
    SerSpsTimer.uchMin = 0;    
    SerSpsTimer.uchSec = 0;
	SerSpsTimer.uchTermNo = 0;
    fsSerShrStatus     = 0;  /* Clear shared printer status */
}

/*
*===========================================================================
** Synopsis:    SHORT    SerSpsTimerRead(VOID);
*
** Return:      SERV_SUCCESS:       Timer Not expired.
*               SERV_TIMER_EXPIRED: Timer Expired.
*
** Description: This function reads and checks Timer.
*===========================================================================
*/
SHORT    SerSpsTimerRead(VOID)
{
    ULONG       ulMAX;

    if ((0 == SerSpsTimer.uchHour) &&              /* 00:00:00 ? */
        (0 == SerSpsTimer.uchMin) &&
        (0 == SerSpsTimer.uchSec)) {
        return SERV_SUCCESS;
    }

    ulMAX = (ULONG)SerReadMDCPara(CLI_MDC_SHARED);   /* Read shared MDC */

    if (Ser_Timer_Read(&SerSpsTimer, ulMAX) < 0) {
        return SERV_TIMER_EXPIRED;
    }
    return SERV_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    VOID    SerSendResponseFH(CLIRESCOM *pResMsgH,
*                                         USHORT usResMsgHLen,
*                                         USHORT usOffsetPoint,
*                                         USHORT usReqLen);
*
*     InPut:    pResMsgH    - response message header (now responded)
*               usResMsgHLen    - response message header length
*               puchReqData     - offset of read file
*               usReqLen    - send data length
*
** Return:      none.      
*
** Description: This function sends response(file handle I/F version).
*===========================================================================
*/
SHORT    SerSendResponseFH(CLIRESCOM *pResMsgH,
                          USHORT usResMsgHLen,
                          USHORT usOffsetPoint,
                          USHORT usReqLen)
{
    USHORT  usSendLen;
	SHORT   sError;

	/*=== BUILD/EDIT MESSAGE ===*/
    usSendLen = 0;
    memcpy(&SerSndBuf.auchData[usSendLen], pResMsgH, usResMsgHLen);
    usSendLen += usResMsgHLen;

    memcpy(&SerSndBuf.auchData[usSendLen], &usReqLen, sizeof(USHORT));
    usSendLen += sizeof(USHORT);

    SstReadFileFH((SERTMPFILE_DATASTART + usOffsetPoint + usResMsgHLen), (VOID *)&SerSndBuf.auchData[usSendLen], usReqLen, hsSerTmpFile);
    usSendLen += usReqLen;

	/*=== EDIT HEADER ===*/       
	// fill in the XGRAM header indicating receiver of the message we are building.
    memcpy(SerSndBuf.auchFaddr, SerNetConfig.auchFaddr, PIF_LEN_IP);
    SerSndBuf.auchFaddr[CLI_POS_UA] = SerRcvBuf.auchFaddr[CLI_POS_UA];
    usSendLen += sizeof(SerSndBuf.auchFaddr);
    SerSndBuf.usFport = SerRcvBuf.usFport;
    usSendLen += sizeof(SerSndBuf.usFport);
    SerSndBuf.usLport = CLI_PORT_SERVER;
    usSendLen += sizeof(SerSndBuf.usLport);

	/*=== SEND RESPONSE ===*/
    sError = SerNetSend(usSendLen);
	if (sError < 0) {
		char xBuff[128];
		sprintf(xBuff, "SerNetSend(): sError = %d, 0x%x, usFunCode 0x%x, usSeqNo 0x%x", sError, *((ULONG *)&SerSndBuf.auchFaddr[0]), pResMsgH->usFunCode, pResMsgH->usSeqNo);
		NHPOS_ASSERT_TEXT((sError >= 0), xBuff);
	}

	return sError;
}

/*===== END OF SOURCE =====*/
