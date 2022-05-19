/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : SERVER module, Receive Message Handling Source File                        
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVRMH.C                                            
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*           SerRMHCheckData();      Check received data 
*           SerRMHInquiry();        Receive during inquiry status
*           SerRMHNormal();         Receive during normal status
*           SerRMHMulSnd();         Receive during multi. sending status 
*           SerRMHMulRcv();         Receive during multi. receiving status
*           SerRMHBackUp();         Receive during back up copy status
*
*           SerRecvNormal();     C  Call each function
*           SerRecvMultiple();      Check multiple reciving
*           SerRecvNextFrame();     Received next frame
*           SerSendMultiple();      Send multiple response
*           SerSendNextFrame();     Send next frame response
*           SerChekFunCode();    C  Check function code valid ?
*           SerChekResponse();      Check response condition
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* Jun-04-93:H.Yamaguchi: Modified same as HP Int'l
* Mar-16-94:K.You      : Add flex GC file feature(add SerCreateFile).
*          :           : (mod. SerRecvMultiple, SerSendMultiple, SerRecvNextFrame,
*          :           : SerSendBakMultiple). 
* Jan-17-95:M.Ozawa    : Modify SerRecvNormal(), SerChekFunCode() for Display on the fly
* Apr-26-95:M.Suzuki   : Modify SerChekFunCode()  R3.0
* Apr-02-96:T.Nakahata : R3.1 Initial (E/J Cluster Reset/ ProgramRpt)
* Aug-11-99:M.Teraki   : R3.5 remove WAITER_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Dec-03-99 : 01.00.00 : hrkato     : Saratoga
* Dec-16-02 : 01.04.00 : cwunn		: SerChekFunCode - added time in/out functionality GSU SR 8
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
#include    <memory.h>
#include	<stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <cpm.h>
#include    <eept.h>
#include    <csstbcpm.h>
#include    <csserm.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <appllog.h>
#include    "servmin.h"


/*
*===========================================================================
** Synopsis:    SHORT    SerRMHCheckData(VOID);
*
** Return:      none.
*
** Description: This function checks received data.  The first step is to
*               determine that the amount of data received is what was
*               sent.  See function CstComMakeMultiDataFH() in csstbcom.c
*               that shows the calculation that is made to determine the
*               number of bytes to send in the message.
*===========================================================================
*/
SHORT    SerRMHCheckData(VOID)
{
    CLIREQCOM   *pResp = (CLIREQCOM *)SerRcvBuf.auchData;
    USHORT      usMsgHLen;

    if (0 == (usMsgHLen = SerChekFunCode(pResp->usFunCode))) {
        PifLog(MODULE_SER_LOG, LOG_ERROR_SER_ILLEGAL_DATA);
        PifLog(MODULE_DATA_VALUE(MODULE_SER_LOG), pResp->usFunCode);
        return SERV_ILLEGAL;
    }
    if (pResp->usSeqNo & CLI_SEQ_SENDDATA) {
		CLIREQDATA  *pRespData = (CLIREQDATA *)&SerRcvBuf.auchData[usMsgHLen];

        if (SerResp.usSize != (usMsgHLen + PIF_LEN_IP + 4 + 2 + pRespData->usDataLen)) {
			CHAR  xBuff[128];

			sprintf (xBuff, "==NOTE: SerRMHCheckData usFunCode 0x%x, usSeqNo 0x%x, addr 0x%x usDataLen %d", pResp->usFunCode, pResp->usSeqNo, *((ULONG *)SerRcvBuf.auchFaddr), pRespData->usDataLen);
			NHPOS_ASSERT_TEXT(0, xBuff);
            PifLog(MODULE_SER_LOG, LOG_ERROR_SER_CODE_01);
            PifLog(MODULE_DATA_VALUE(MODULE_SER_LOG), LOG_ERROR_SER_LOSS_DATA);
            PifLog(MODULE_DATA_VALUE(MODULE_SER_LOG), pResp->usFunCode);
            return SERV_ILLEGAL;
        }
    } else if (SerResp.usSize != (usMsgHLen + PIF_LEN_IP + 4)) {
		CHAR  xBuff[128];

		sprintf (xBuff, "==NOTE: SerRMHCheckData usFunCode 0x%x, usSeqNo 0x%x, addr 0x%x", pResp->usFunCode, pResp->usSeqNo, *((ULONG *)SerRcvBuf.auchFaddr));
		NHPOS_ASSERT_TEXT(0, xBuff);
        PifLog(MODULE_SER_LOG, LOG_ERROR_SER_CODE_02);
        PifLog(MODULE_DATA_VALUE(MODULE_SER_LOG), LOG_ERROR_SER_LOSS_DATA);
        PifLog(MODULE_DATA_VALUE(MODULE_SER_LOG), pResp->usFunCode);
        return SERV_ILLEGAL;
    }
    return SERV_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    VOID    SerRMHInquiry(VOID);
*
** Return:      none.
*
** Description: This function executes request handling during Inquiry state.
*===========================================================================
*/
VOID    SerRMHInquiry(VOID)
{
    CLIREQCOM   *pResp = (CLIREQCOM *)SerRcvBuf.auchData;

    if (CLI_FCINQ != (pResp->usFunCode & CLI_FCMSG_TYPE)) {
        /* SerSendError(STUB_BUSY); */  /* no response to satellite */
    } else {
        if (SER_IAM_SATELLITE) {

        }  else {
            SerRecvInq();
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerRMHNormal(VOID);
*
** Return:      none.
*
** Description: This function executes request handling during Normal state.
*===========================================================================
*/
VOID    SerRMHNormal(VOID)
{
    CLIREQCOM   *pResp = (CLIREQCOM *)SerRcvBuf.auchData;
    SHORT       sError;
 
    if (CLI_FCBAK == (pResp->usFunCode & CLI_BAKMSG_TYPE)) {
        ;                                   /* no response */
    } else {
        sError = SerRecvMultiple(pResp);
        if (STUB_SUCCESS != sError) {
            SerSendError(sError);
        } else {
            SerRecvNormal(pResp);
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerRMHMulSnd(VOID);
*
** Return:      none.
*
** Description:
*   This function executes request handling during Multi. sending state.
*===========================================================================
*/
VOID    SerRMHMulSnd(VOID)
{
    CLIREQCOM   *pResp = (CLIREQCOM *)SerRcvBuf.auchData;

    if (CLI_FCINQ == (pResp->usFunCode & CLI_FCMSG_TYPE)) {
        SerSendError(STUB_BUSY);
    } else if (CLI_FCBAK == (pResp->usFunCode & CLI_BAKMSG_TYPE)) {
        ;                                   /* no response */
    } else {
		SHORT       sSerSendStatus;

        sSerSendStatus = SerSendNextFrame(pResp);
        if (STUB_MULTI_SEND == sSerSendStatus) {
            ;
        } else if (STUB_SUCCESS == sSerSendStatus) {
            SerRecvNormal(pResp);
        } else {
            SerSendError(sSerSendStatus);
        }
    }
}
    
/*
*===========================================================================
** Synopsis:    VOID    SerRMHMulRcv(VOID);
*
** Return:      none.
*
** Description:
*   This function executes request handling during Multi. receiving state.
*===========================================================================
*/
VOID    SerRMHMulRcv(VOID)
{
    CLIREQCOM   *pResp = (CLIREQCOM *)SerRcvBuf.auchData;
    SHORT       sError;

    if (CLI_FCINQ == (pResp->usFunCode & CLI_FCMSG_TYPE)) {
        SerSendError(STUB_BUSY);
    } else if (CLI_FCBAK == (pResp->usFunCode & CLI_BAKMSG_TYPE)) {
        ;                                   /* no response */
    } else {
        sError = SerRecvNextFrame(pResp);
        if (STUB_SUCCESS == sError) {
            SerRecvNormal(pResp);
//        } else if (STUB_IGNORE == sError) {
//			;	/* ignore previous seq. no request */
        } else {
            SerSendError(sError);
        }
    }
}
            
/*
*===========================================================================
** Synopsis:    VOID    SerRMHBackUp(VOID);
*
** Return:      none.
*
** Description:
*   This function executes request handling during Back up copy state.
*===========================================================================
*/
VOID    SerRMHBackUp(VOID)
{
    CLIREQCOM   *pResp = (CLIREQCOM *)SerRcvBuf.auchData;

    if (SER_IAM_SATELLITE) {
        return;
    }

    if (CLI_FCINQ == (pResp->usFunCode & CLI_FCMSG_TYPE)) {
        SerRecvInq();
    } else if (CLI_FCBAK == (pResp->usFunCode & CLI_BAKMSG_TYPE)) {
		SHORT  sError;

		sError = SerSendBakNextFrame(pResp);
        if (STUB_MULTI_SEND == sError) {
            ;
        } else if (STUB_SUCCESS == sError) {
            SerRecvBak();
        } else {
            SerSendError(sError);
        }
        SerChangeStatus(SER_STBACKUP);
    } else {
        SerSendError(STUB_BUSY);
        if (SER_IAM_BMASTER) {              /* backup error */
            SerChangeStatus(SER_STNORMAL);
            sSerExeError = STUB_ILLEGAL;
            SstResetFlag(SER_SPESTS_AC42);
            PifReleaseSem(usSerCliSem);
            SerSendNtbStart();
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerRecvNormal(CLIREQCOM *pReqMsgH);
*
*     InPut:    pReqMsgH    - pointer of received request message
*
** Return:      none.
*
** Description: This function decides a function to call each modele.
*===========================================================================
*/
VOID    SerRecvNormal(CLIREQCOM *pReqMsgH)
{
	SHORT   sSerChekStatus;
    USHORT  usFun = (pReqMsgH->usFunCode & CLI_RSTCONTCODE);
	char    xBuff[256];

    if (CLI_FCINQ == (usFun & CLI_FCMSG_TYPE)) {
        SerRecvInq();
        return;
    }

	// Handle function code to request status from a terminal.  Functionality
	// already existed in the ISP Thread to allow a remote application such as CWS
	// to interrogate Master Terminal is On-line through the PCIF interface.
	if ( CLI_FCOSYSTEMSTATUS == usFun ) {
		CLIREQSYSTEMSTATUS      *pReqMsgH;
		CLIRESSYSTEMSTATUS      ResMsgH;
	   
		pReqMsgH = (CLIREQSYSTEMSTATUS *)SerRcvBuf.auchData ;
		memset(&ResMsgH, 0, sizeof(CLIRESSYSTEMSTATUS));

		ResMsgH.usFunCode     = pReqMsgH->usFunCode;
		ResMsgH.sResCode      = STUB_SUCCESS;
		ResMsgH.usSeqNo       = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
		ResMsgH.fsComStatus   = SerComReadStatus();
	    
		SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESSYSTEMSTATUS), NULL, 0);
        return;
	}

    if (SERV_SUCCESS != (sSerChekStatus = SerChekResponse())) {
        switch(usFun & CLI_FCMSG_TYPE) {  /* what kind of message ? */
        case    CLI_FCSPS:           /* Shared Printer Function */
            SerRecvSps();
            break;

        case    CLI_FCKPS:           /* Kitchen Printer Function */
            if ( CLI_FCDFLPRINT != usFun ) {
                SerRecvKps();        /* Kitchen Printer function */
            }
            break;

        case    CLI_FCEJ:           /* Electoric Journal Function, R3.1 */
            SerRecvEJ();
            break;

        default:                     /* not used */
			if (sSerChekStatus == SERV_NOT_MASTER) {
				sprintf (xBuff, "SerRecvNormal(): SERV_NOT_MASTER usUA %d, usFunCode 0x%x, usReqSeqNo 0x%x", SerRcvBuf.auchFaddr[CLI_POS_UA], pReqMsgH->usFunCode, pReqMsgH->usSeqNo);
				NHPOS_ASSERT_TEXT(!SERV_NOT_MASTER, xBuff);
				SerSendError(STUB_NOT_MASTER);
			} else {
				sprintf (xBuff, "SerRecvNormal(): STUB_BUSY usUA %d, usFunCode 0x%x, usReqSeqNo 0x%x", SerRcvBuf.auchFaddr[CLI_POS_UA], pReqMsgH->usFunCode, pReqMsgH->usSeqNo);
				NHPOS_ASSERT_TEXT(!STUB_BUSY,  xBuff);
				SerSendError(STUB_BUSY);
			}
            break;
        }
        return;
    }

    switch(usFun & CLI_FCMSG_TYPE) {  /* what kind of message ? */
    case    CLI_FCCAS:           /* Cashier Function */
        SerRecvCas();
        break;

    case    CLI_FCGCF:           /* Guest Check File Function */
        SerRecvGCF();
        break;
    
    case    CLI_FCTTL:           /* Total File Function */
        SerRecvTtl();
        break;
    
    case    CLI_FCOP:            /* Op. Para Function */
        SerRecvOpr();
        break;
    
    case    CLI_FCKPS:           /* Kitchen Printer Function */
        if ( CLI_FCDFLPRINT == usFun ) {
            SerRecvDfl();        /* Display on Fly Function */
        } else {
            SerRecvKps();        /* Kitchen Printer function */
        }
        break;

    case    CLI_FCSPS:           /* Shared Printer Function */
        SerRecvSps();
        break;

    case    CLI_FCETK:           /* Employee time keeping Function */
        SerRecvEtk();
        break;

    case    CLI_FCCPM:           /* Charge posting Function */
		SerRecvCpm();
		break;

    /* === New Functions - E/J Cluster Reset (Release 3.1) BEGIN === */
    case    CLI_FCEJ:           /* Electoric Journal Function, R3.1 */
        SerRecvEJ();
        break;
    /* === New Functions - E/J Cluster Reset (Release 3.1) END === */
	case    CLI_FCRMTFILE: //ESMITH LAYOUT
		SerRmtFileServer();		
		break;

    default:                     /* not used */
        break;
    }
}    

/*
*===========================================================================
** Synopsis:    SHORT   SerRecvMultiple(CLIREQCOM *pReqMsgH);
*
*     Input:    pReqMsgH    - pointer of received request message
*
** Return:      STUB_SUCCESS:   normal request received or all data received
*               STUB_MULTI_RECV:    multi. data received
*               STUB_FRAME_SEQERR:  frame sequence # is not 1
*
** Description: This function checks to receive multi. data.
*===========================================================================
*/
SHORT   SerRecvMultiple(CLIREQCOM *pReqMsgH)
{
    CLIREQDATA  *pReqBuff;
    USHORT      usReqMsgHLen;
    USHORT      usFun;

    if (0 == pReqMsgH->usSeqNo || 0 == (pReqMsgH->usSeqNo & CLI_SEQ_SENDDATA)) {
		// a multiple message will have the CLI_SEQ_SENDDATA flag set in the sequence number.
		// if this flag is not set then this is not a multiple send.
        return STUB_SUCCESS;
    }

	// Check to ensure that this is the first packet in the
	// sequence of packets to be processed.
    if (1 != (pReqMsgH->usSeqNo & CLI_SEQ_CONT)) {
		char xBuff[256];
		sprintf (xBuff, "SerRecvMultiple(): STUB_FRAME_SEQERR usUA %d, usFunCode 0x%x, usReqSeqNo 0x%x", SerRcvBuf.auchFaddr[CLI_POS_UA], pReqMsgH->usFunCode, pReqMsgH->usSeqNo);
		NHPOS_ASSERT_TEXT((1 == (pReqMsgH->usSeqNo & CLI_SEQ_CONT)), xBuff);
        return STUB_FRAME_SEQERR;
    }

    usReqMsgHLen = SerChekFunCode(pReqMsgH->usFunCode);
    pReqBuff = (CLIREQDATA *)((UCHAR *)pReqMsgH + usReqMsgHLen);
    SerResp.pSavBuff = (CLIREQDATA *)&auchSerTmpBuf[usReqMsgHLen];
    memcpy(auchSerTmpBuf, pReqMsgH, usReqMsgHLen);

    usFun = pReqMsgH->usFunCode & CLI_RSTCONTCODE;

	// If this is a guest check we are storing or a total update
	// that needs to be processed by the total update logic, then
	// we will write the data packet to the temporary file as these
	// contain large amounts of data.
    if ((usFun == CLI_FCGCFSTORE) || (usFun == CLI_FCTTLUPDATE)) {
		NHPOS_ASSERT((SERTMPFILE_DATASTART + usReqMsgHLen) < 0xffff);
		NHPOS_ASSERT((pReqBuff->usDataLen + SerResp.pSavBuff->usDataLen) < 0xffff);
        SstWriteFileFH(SERTMPFILE_DATASTART, pReqMsgH, usReqMsgHLen, hsSerTmpFile);
        SstWriteFileFH((SERTMPFILE_DATASTART + usReqMsgHLen), (VOID *)pReqBuff->auchData, pReqBuff->usDataLen, hsSerTmpFile);
	} else if (usFun == CLI_FCCOPONNENGINEMSGFH) {
        SstWriteFileFH(SERTMPFILE_DATASTART, (VOID *)pReqBuff->auchData, pReqBuff->usDataLen, hsSerTmpFile);
    } else {
        memcpy(SerResp.pSavBuff->auchData, pReqBuff->auchData, pReqBuff->usDataLen);
    }

    SerResp.pSavBuff->usDataLen = pReqBuff->usDataLen;

    if (0 == (pReqMsgH->usSeqNo & CLI_SEQ_SENDEND)) {
		// check to see if this is the last frame/packet in the multiple message stream.
		// if the sequence end flag is not set then we need to change or stay in multi receive
		// and begin/continue to process a sequence of messages from the sending terminal.
        SerResp.uchPrevUA = SerRcvBuf.auchFaddr[CLI_POS_UA];
        SerResp.usPrevMsgHLen = usReqMsgHLen;
        SerChangeStatus(SER_STMULRCV);
        return STUB_MULTI_RECV;
	} else {
		SerResp.uchPrevUA = 0;
	}
    return STUB_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerRecvNextFrame(CLIREQCOM *pReqMsgH);
*
*     Input:    pReqMsgH    - pointer of received request message
*
** Return:      STUB_SUCCESS:   normal request received or all data received
*               STUB_MULTI_RECV:    multi. data received
*               STUB_BUSY:      multi request received from another terminal
*               STUB_FAITAL:    another function code detected
*               STUB_FRAME_SEQERR:  frame sequence error detected    
*
** Description:
*   This function checks to receive during multi. receiving state.
*
*   If the packet received is has a zero sequence number indicating that it
*   is a standalone packet, then we will return STUB_SUCCESS to the caller.
*   We also return STUB_SUCCESS if this is the last packet in the stream
*   so that the function SerRecvNormal() will then process the packet
*   stream stored in the temporary file.
*
*   If we receive a multi-send packet that is from a terminal other than
*   the terminal sending the current multi-packet stream, then we will
*   respond with a STUB_BUSY_MULTI to indicate to that terminal to try again
*   in a moment.  We have only a single temporary file and we do not want
*   to mix streams of data from different terminals.
*===========================================================================
*/
SHORT   SerRecvNextFrame(CLIREQCOM *pReqMsgH)
{
    CLIREQCOM   *pSavMsgH = (CLIREQCOM *)auchSerTmpBuf;
    USHORT      usFun;

	// If this message is not part of a stream of multiple packets
	// then we indicate it should be processed as a standard
	// single packet message.
    if (0 == pReqMsgH->usSeqNo) {               /* another req. received */
        return STUB_SUCCESS;
    }

#if 0
	// this seems to be a good thing to do.
	{
		SHORT  sSerChekStatus = SerChekResponse();

		if (SERV_SUCCESS != sSerChekStatus) {
			SerChangeStatus(SER_STNORMAL);
			NHPOS_NONASSERT_TEXT("==NOTE: SerRecvNextFrame - SerChekResponse() fail STUB_NOT_MASTER.");
			return STUB_NOT_MASTER;
		}
	}
#endif

	/*
		another UA ?
		Check that this new received message is from the same terminal
		with which we are having a multi-receive conversation.

		If it is not, then we indicate we are busy and to try again later.

		If this message is from the same terminal, then we next check that:
			message contains the same message function code
			message sequence number is correct (did we miss any messages?)
	 */

    if (SerResp.uchPrevUA && SerRcvBuf.auchFaddr[CLI_POS_UA] != SerResp.uchPrevUA) {
		char xBuff[256];
		sprintf (xBuff, "SerRecvNextFrame(): STUB_BUSY_MULTI  usUA %d, usPrevUA %d, usFunCode 0x%x, usSeqNo 0x%x", SerRcvBuf.auchFaddr[CLI_POS_UA], SerResp.uchPrevUA, pReqMsgH->usFunCode, pReqMsgH->usSeqNo);
		NHPOS_ASSERT_TEXT((SerRcvBuf.auchFaddr[CLI_POS_UA] == SerResp.uchPrevUA), xBuff);
        return STUB_BUSY_MULTI;
    }

    /* same function code ? */
    usFun = pReqMsgH->usFunCode & CLI_RSTCONTCODE;
    if (usFun != (pSavMsgH->usFunCode & CLI_RSTCONTCODE)) {
		char xBuff[256];
		sprintf (xBuff, "SerRecvNextFrame(): STUB_FAITAL  usUA %d, usFunCode 0x%x, usPrevFunCode 0x%x, usSeqNo 0x%x", SerRcvBuf.auchFaddr[CLI_POS_UA], pReqMsgH->usFunCode, pSavMsgH->usFunCode, pReqMsgH->usSeqNo);
		NHPOS_ASSERT_TEXT((usFun == (pSavMsgH->usFunCode & CLI_RSTCONTCODE)), xBuff);
        SerChangeStatus(SER_STNORMAL);
        return STUB_FAITAL;
    }                                           /* sequence # OK ? */
    if ((pReqMsgH->usSeqNo & CLI_SEQ_CONT) != ((pSavMsgH->usSeqNo & CLI_SEQ_CONT) + 1)) {
	    if ((pReqMsgH->usSeqNo & CLI_SEQ_CONT) == ((pSavMsgH->usSeqNo & CLI_SEQ_CONT) )) {
			char xBuff[256];
			sprintf (xBuff, "SerRecvNextFrame(): STUB_IGNORE usUA %d, usFunCode 0x%x, usReqSeqNo 0x%x, usSavSeqNo 0x%x", SerRcvBuf.auchFaddr[CLI_POS_UA], pReqMsgH->usFunCode, pReqMsgH->usSeqNo, pSavMsgH->usSeqNo);
			NHPOS_ASSERT_TEXT(((pReqMsgH->usSeqNo & CLI_SEQ_CONT) != ((pSavMsgH->usSeqNo & CLI_SEQ_CONT) )), xBuff);
			// fall through and treat as if we are doing this rather than have already done it.
			// this is in case we get a duplicate message due to retransmit.
//			return STUB_IGNORE;	/* ignore previous seq. no request */
			if (0 == (pReqMsgH->usSeqNo & CLI_SEQ_SENDEND)) {
				SerChangeStatus(SER_STMULRCV);
				return STUB_MULTI_RECV;
			}
		} else {
			char xBuff[256];
			sprintf(xBuff, "SerRecvNextFrame(): STUB_FRAME_SEQERR usUA %d, usFunCode 0x%x, usReqSeqNo 0x%x, usSavSeqNo 0x%x", SerRcvBuf.auchFaddr[CLI_POS_UA], pReqMsgH->usFunCode, pReqMsgH->usSeqNo, pSavMsgH->usSeqNo);
			NHPOS_ASSERT_TEXT( ((pReqMsgH->usSeqNo & CLI_SEQ_CONT) == ((pSavMsgH->usSeqNo & CLI_SEQ_CONT) + 1)), xBuff);
			SerChangeStatus(SER_STNORMAL);
			return STUB_FRAME_SEQERR;
		}
	} else {
		CLIREQDATA  *pReqBuff = (CLIREQDATA *)((UCHAR *)pReqMsgH + SerResp.usPrevMsgHLen);
	    
		if ((usFun == CLI_FCGCFSTORE) || (usFun == CLI_FCTTLUPDATE)) {
			NHPOS_ASSERT((SERTMPFILE_DATASTART + SerResp.usPrevMsgHLen + SerResp.pSavBuff->usDataLen) < 0xffff);
			NHPOS_ASSERT((pReqBuff->usDataLen + SerResp.pSavBuff->usDataLen) < 0xffff);
			SstWriteFileFH((SERTMPFILE_DATASTART + SerResp.usPrevMsgHLen + SerResp.pSavBuff->usDataLen), (VOID *)pReqBuff->auchData, pReqBuff->usDataLen, hsSerTmpFile);  /* ### Mod (2171 for Win32) V1.0 */
		} else if (usFun == CLI_FCCOPONNENGINEMSGFH) {
			SstWriteFileFH(SERTMPFILE_DATASTART+SerResp.pSavBuff->usDataLen, (VOID *)pReqBuff->auchData, pReqBuff->usDataLen, hsSerTmpFile);
		} else {
			memcpy(SerResp.pSavBuff->auchData + SerResp.pSavBuff->usDataLen, pReqBuff->auchData, pReqBuff->usDataLen);
		}
		SerResp.pSavBuff->usDataLen += pReqBuff->usDataLen;

		// Now lets see if this is the last message in the multiple messages
		// the sending terminal is sending to us or are there more forthcoming.
		// Check to see if the end of transmission flag is set.  If not,
		// then we go back to SER_STNORMAL state from the SER_STMULRCV state.
		if (0 == (pReqMsgH->usSeqNo & CLI_SEQ_SENDEND)) {
			++ pSavMsgH->usSeqNo;
			SerChangeStatus(SER_STMULRCV);
			return STUB_MULTI_RECV;
		}
	}
    SerChangeStatus(SER_STNORMAL);
    return STUB_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    VOID    SerSendMultiple(CLIRESCOM *pResMsgH,
*                                       USHORT usResMsgHLen);
*
*     InPut:    pResMsgH    - response message header (now responded)
*               usResMsgHLen    - response message header length
*
** Return:      none.
*
** Description: This function sends response to be multiple.
*===========================================================================
*/
SHORT    SerSendMultiple(CLIRESCOM *pResMsgH, USHORT usResMsgHLen)
{
    USHORT      usDataMax, usDataLen;
    USHORT      usFunCode;
	SHORT       sSerSendStatus;
	char        xBuff[256];

    if (SER_STNORMAL == usSerStatus) {
        SerResp.usPrevDataOff = 0;
        /* SerResp.usPrevDataOffSav = 0; */
        SerResp.usPrevMsgHLen = usResMsgHLen;
        SerResp.uchPrevUA     = SerRcvBuf.auchFaddr[CLI_POS_UA];
    }
    usDataLen = SerResp.pSavBuff->usDataLen - SerResp.usPrevDataOff;
    usDataMax = PIF_LEN_UDATA - PIF_LEN_IP - usResMsgHLen - sizeof(XGHEADER) - 10;
	/* Leave room for IP address, headers, and additional data (see functions
	 * SerSendResponse and PifNetSendG/D) */
    if (usDataLen > usDataMax) {                /* still exist send data */
        usDataLen = usDataMax;
        SerChangeStatus(SER_STMULSND);
    } else {                                    /* all data send */
        if (pResMsgH->usFunCode & CLI_FCWCONFIRM) {
            SerChangeStatus(SER_STMULSND);
        } else {
            SerChangeStatus(SER_STNORMAL);
        }
        pResMsgH->sResCode = STUB_SUCCESS;
        pResMsgH->usSeqNo |= CLI_SEQ_RECVEND;
    }
    pResMsgH->usSeqNo |= CLI_SEQ_RECVDATA;
    memcpy(auchSerTmpBuf, pResMsgH, usResMsgHLen);       /** Send Response **/    
    
    usFunCode = pResMsgH->usFunCode;
    usFunCode &= CLI_RSTCONTCODE;
    if ((usFunCode == CLI_FCGCFREADLOCK) ||
        (usFunCode == CLI_FCGCFINDREAD) ||
        (usFunCode == CLI_FCGCFCHECKREAD)) {
        sSerSendStatus = SerSendResponseFH(pResMsgH, usResMsgHLen, SerResp.usPrevDataOff, usDataLen);
		if (sSerSendStatus < 0) {
			sprintf(xBuff, "SerSendResponseFH(): sSerSendStatus = %d, 0x%x, usFunCode 0x%x, usSeqNo 0x%x", sSerSendStatus, *((ULONG *)SerRcvBuf.auchFaddr), pResMsgH->usFunCode, pResMsgH->usSeqNo);
			NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
		}
    } else {
        sSerSendStatus = SerSendResponse(pResMsgH, usResMsgHLen, SerResp.pSavBuff->auchData + SerResp.usPrevDataOff, usDataLen);
		if (sSerSendStatus < 0) {
			sprintf(xBuff, "SerSendResponse(): sSerSendStatus = %d, 0x%x, usFunCode 0x%x, usSeqNo 0x%x", sSerSendStatus, *((ULONG *)SerRcvBuf.auchFaddr), pResMsgH->usFunCode, pResMsgH->usSeqNo);
			NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
		}
    }
    /* SerResp.usPrevDataOffSav = SerResp.usPrevDataOff; */
    SerResp.usPrevDataOff += usDataLen;

	return sSerSendStatus;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerSendNextFrame(CLIREQCOM *pReqMsgH);
*
*     Input:    pReqMsgH    - pointer of received request message
*
** Return:      STUB_SUCCESS:   normal request received
*               STUB_MULTI_SEND:    multi. data send
*               STUB_BUSY:      multi request received from another terminal
*               STUB_FAITAL:    another function code detected
*               STUB_FRAME_SEQERR:  frame sequence error detected    
*
** Description: This function checks to receive during multi. sending state.
*===========================================================================
*/
SHORT   SerSendNextFrame(CLIREQCOM *pReqMsgH)
{
    CLIRESCOM   *pSavMsgH = (CLIRESCOM *)auchSerTmpBuf;  // SerSendNextFrame()
	SHORT       sSerSendStatus;
	char        xBuff [256];
                                            
    if (0 == pReqMsgH->usSeqNo) {               /* another req. received */
        return STUB_SUCCESS;
    }                                           /* another UA ? */
    if (SerResp.uchPrevUA && SerRcvBuf.auchFaddr[CLI_POS_UA] != SerResp.uchPrevUA) {
		char xBuff[128];
		sprintf (xBuff, "SerSendNextFrame(): STUB_BUSY_MULTI  usUA %d, usPrevUA %d, usFunCode 0x%x", SerRcvBuf.auchFaddr[CLI_POS_UA], SerResp.uchPrevUA, pReqMsgH->usFunCode);
		NHPOS_ASSERT_TEXT((SerRcvBuf.auchFaddr[CLI_POS_UA] == SerResp.uchPrevUA), xBuff);
        return STUB_BUSY_MULTI;
    }                                           
    /* same function code ? */
    if ((pReqMsgH->usFunCode & CLI_RSTCONTCODE) != (pSavMsgH->usFunCode & CLI_RSTCONTCODE)) {
        SerChangeStatus(SER_STNORMAL);
		sprintf (xBuff, "SerSendNextFrame(): STUB_FAITAL 0x%x, usReqFunCode = 0x%x, usSavFunCode = 0x%x, usReqSeqNo = 0x%x", *((ULONG *)SerRcvBuf.auchFaddr), pReqMsgH->usFunCode, pSavMsgH->usFunCode, pReqMsgH->usSeqNo);
		NHPOS_ASSERT_TEXT(((pReqMsgH->usFunCode & CLI_RSTCONTCODE) == (pSavMsgH->usFunCode & CLI_RSTCONTCODE)), xBuff);
        return STUB_FAITAL;
    }                                           /* sequence # OK ? */
    if ((pReqMsgH->usSeqNo & CLI_SEQ_CONT) != ((pSavMsgH->usSeqNo & CLI_SEQ_CONT) + 1)) {
        SerChangeStatus(SER_STNORMAL);
        SerChekGCFUnLock();
		sprintf (xBuff, "SerSendNextFrame(): STUB_FRAME_SEQERR 0x%x, usReqFunCode = 0x%x, usReqSeqNo 0x%x, usSavSeqNo 0x%x", *((ULONG *)SerRcvBuf.auchFaddr), pReqMsgH->usFunCode, pReqMsgH->usSeqNo, pSavMsgH->usSeqNo);
		NHPOS_ASSERT_TEXT(((pReqMsgH->usSeqNo & CLI_SEQ_CONT) == ((pSavMsgH->usSeqNo & CLI_SEQ_CONT) + 1)), xBuff);
        return STUB_FRAME_SEQERR;
/***
	    if ((pReqMsgH->usSeqNo & CLI_SEQ_CONT) == 
    	    ((pSavMsgH->usSeqNo & CLI_SEQ_CONT) )) {
			
			/ just retry /
		    -- pSavMsgH->usSeqNo;
    		SerResp.usPrevDataOff = SerResp.usPrevDataOffSav;
			
		} else {
	        SerChangeStatus(SER_STNORMAL);
    	    SerChekGCFUnLock();
        	return STUB_FRAME_SEQERR;
        }
***/
    }                                           /* confirmation ? */
    if (pReqMsgH->usFunCode & CLI_SETCONFIRM) {
        SerChangeStatus(SER_STNORMAL);
        return STUB_MULTI_SEND;
    }
    ++ pSavMsgH->usSeqNo;
    sSerSendStatus = SerSendMultiple(pSavMsgH, SerResp.usPrevMsgHLen); 
	if (sSerSendStatus < 0) {
		char xBuff [128];
		sprintf (xBuff, "SerSendNextFrame(): 0x%x, sSerSendStatus = %d, usReqFunCode = 0x%x, usReqSeqNo = 0x%x", *((ULONG *)SerRcvBuf.auchFaddr), sSerSendStatus, pReqMsgH->usFunCode, pReqMsgH->usSeqNo);
		NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
	}

    return STUB_MULTI_SEND;
}

/*
*===========================================================================
** Synopsis:    VOID    SerSendBakMultiple(CLIRESCOM *pResMsgH,
*                                           USHORT usResMsgHLen);
*
*     InPut:    pResMsgH    - response message header (now responded)
*               usResMsgHLen    - response message header length
*
** Return:      none.
*
** Description: This function sends backup response to be multiple.
*===========================================================================
*/
VOID    SerSendBakMultiple(CLIRESCOM *pResMsgH, USHORT usResMsgHLen)
{
    USHORT      usDataMax, usDataLen;
    USHORT      usFunCode;
	char        xBuff[256];

    if (0 == SerResp.usPrevMsgHLen) {
        SerResp.usPrevDataOff = 0;
        /* SerResp.usPrevDataOffSav = 0; */
        SerResp.usPrevMsgHLen = usResMsgHLen;
    }
    usDataLen = SerResp.pSavBuff->usDataLen - SerResp.usPrevDataOff;
    usDataMax = PIF_LEN_UDATA - PIF_LEN_IP - usResMsgHLen - sizeof(XGHEADER) - 10;
	/* Leave room for IP address, headers, and additional data (see functions
	 * SerSendResponse and PifNetSendG/D) */
    if (usDataLen > usDataMax) {                /* still exist send data */
        usDataLen = usDataMax;
    } else {                                    /* all data send */
        SerResp.usPrevMsgHLen = 0;
        pResMsgH->sResCode = STUB_SUCCESS;
        pResMsgH->usSeqNo |= CLI_SEQ_RECVEND;
    }
    pResMsgH->usSeqNo |= CLI_SEQ_RECVDATA;
    memcpy(auchSerTmpBuf, pResMsgH, usResMsgHLen);
                                                /** Send Response **/    
    usFunCode = pResMsgH->usFunCode;
    usFunCode &= CLI_RSTCONTCODE;
    if ((usFunCode == CLI_FCGCFREADLOCK) ||
        (usFunCode == CLI_FCGCFINDREAD)  ||
        (usFunCode == CLI_FCGCFCHECKREAD)  ||
        (usFunCode == CLI_FCBAKGCF)) {
		SHORT  sError;
        sError = SerSendResponseFH(pResMsgH, usResMsgHLen, SerResp.usPrevDataOff, usDataLen);
		if (sError < 0) {
			sprintf(xBuff, "SerSendResponseFH(): 0x%x, sError = %d, usFunCode = 0x%x, usSeqNo = 0x%x", *((ULONG *)SerRcvBuf.auchFaddr), sError, pResMsgH->usFunCode, pResMsgH->usSeqNo);
			NHPOS_ASSERT_TEXT((sError >= 0), xBuff);
		}
    } else {
        SHORT  sError;
        sError = SerSendResponse(pResMsgH, usResMsgHLen, SerResp.pSavBuff->auchData + SerResp.usPrevDataOff, usDataLen);
		if (sError < 0) {
			sprintf(xBuff, "SerSendResponse(): 0x%x, sError = %d, usFunCode = 0x%x, usSeqNo = 0x%x", *((ULONG *)SerRcvBuf.auchFaddr), sError, pResMsgH->usFunCode, pResMsgH->usSeqNo);
			NHPOS_ASSERT_TEXT((sError >= 0), xBuff);
		}
    }
    /* SerResp.usPrevDataOffSav = SerResp.usPrevDataOff; */
    SerResp.usPrevDataOff += usDataLen;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerSendBakNextFrame(CLIREQCOM *pReqMsgH);
*
*     Input:    pReqMsgH    - pointer of received request message
*
** Return:      STUB_SUCCESS:   normal request received
*               STUB_MULTI_SEND:    multi. data send
*               STUB_FAITAL:    another function code detected
*               STUB_FRAME_SEQERR:  frame sequence error detected    
*
** Description: This function checks to receive during backup multi. sending.
*===========================================================================
*/
SHORT   SerSendBakNextFrame(CLIREQCOM *pReqMsgH)
{
    CLIRESCOM   *pSavMsgH = (CLIRESCOM *)auchSerTmpBuf;  // SerSendBakNextFrame()
	char        xBuff[256];
              
    if (0 == SerResp.usPrevMsgHLen) {
        if (1 < (pReqMsgH->usSeqNo & CLI_SEQ_CONT)) {
            return STUB_FRAME_SEQERR;
        }
        return STUB_SUCCESS;
    }
    /* same function code ? */
    if ((pReqMsgH->usFunCode & CLI_RSTCONTCODE) != (pSavMsgH->usFunCode & CLI_RSTCONTCODE)) {
        SerResp.usPrevMsgHLen = 0;
		sprintf (xBuff, "SerSendBakNextFrame(): STUB_FAITAL usUA %d, usFunCode = 0x%x, pSavMsgH->usFunCode = 0x%x, usSeqNo = 0x%x", SerRcvBuf.auchFaddr[CLI_POS_UA], pReqMsgH->usFunCode, pSavMsgH->usFunCode, pReqMsgH->usSeqNo);
		NHPOS_ASSERT_TEXT(((pReqMsgH->usFunCode & CLI_RSTCONTCODE) == (pSavMsgH->usFunCode & CLI_RSTCONTCODE)), xBuff);
        return STUB_FAITAL;
    }                                           /* sequence # OK ? */
    if ((pReqMsgH->usSeqNo & CLI_SEQ_CONT) != ((pSavMsgH->usSeqNo & CLI_SEQ_CONT) + 1)) {
        SerResp.usPrevMsgHLen = 0;
		sprintf (xBuff, "SerSendBakNextFrame(): STUB_FRAME_SEQERR usUA %d, usFunCode = 0x%x, usSeqNo = 0x%x, pSavMsgH->usSeqNo = 0x%x", SerRcvBuf.auchFaddr[CLI_POS_UA], pReqMsgH->usFunCode, pReqMsgH->usSeqNo, pSavMsgH->usSeqNo);
		NHPOS_ASSERT_TEXT(((pReqMsgH->usSeqNo & CLI_SEQ_CONT) == ((pSavMsgH->usSeqNo & CLI_SEQ_CONT) + 1)), xBuff);
        return STUB_FRAME_SEQERR;
/****
	    if ((pReqMsgH->usSeqNo & CLI_SEQ_CONT) == 
    	    ((pSavMsgH->usSeqNo & CLI_SEQ_CONT) )) {
			
			/ just retry /
		    -- pSavMsgH->usSeqNo;
    		SerResp.usPrevDataOff = SerResp.usPrevDataOffSav;
			
		} else {
	        SerResp.usPrevMsgHLen = 0;
    	    return STUB_FRAME_SEQERR;
    	}
****/
    }                                           /* confirmation ? */
    ++ pSavMsgH->usSeqNo;
    SerSendBakMultiple(pSavMsgH, SerResp.usPrevMsgHLen); 

    return STUB_MULTI_SEND;
}

/*
*===========================================================================
** Synopsis:    USHORT  SerChekFunCode(USHORT usFunCode);
*
*     Input:    usFunCode   - function code of received request
*
** Return:     >0:  request message length of that function code 
*               0:  illegal function code
*                  
** Description: This function checks function code.
*  Updates:
	12.16.2002	cwunn	CLI_FCETKINDJOBREAD case added to allow time in/out
						from any terminal GSU SR 8
*===========================================================================
*/
USHORT  SerChekFunCode(USHORT usFunCode)
{
    USHORT      usReqMsgHLen;

    switch(usFunCode & CLI_RSTCONTCODE) {
    case CLI_FCINQINQUIRY:                  /* Inquiry functions */
    case CLI_FCINQCHGDATE:
        if (SER_IAM_MASTER) {
            usReqMsgHLen = sizeof(CLIRESINQUIRY);
        } else {
            usReqMsgHLen = sizeof(CLIREQINQUIRY);
        }
        break;

    case CLI_FCINQREQINQ:       
        if (SER_IAM_MASTER) {
            usReqMsgHLen = sizeof(CLIREQINQUIRY);
        } else {
            usReqMsgHLen = sizeof(CLIRESINQUIRY);
        }
        break;

    case CLI_FCOSYSTEMSTATUS:       
        usReqMsgHLen = sizeof(CLIREQSYSTEMSTATUS);
        break;

    case CLI_FCCASSIGNIN:                   /* Cashier functions */
    case CLI_FCCASSIGNOUT:  
    case CLI_FCCASASSIGN:       
    case CLI_FCCASDELETE:       
    case CLI_FCCASINDCLOSE: 
    case CLI_FCCASCLRSCODE: 
    case CLI_FCCASPRESIGNOUT:   
    case CLI_FCCASOPENPICLOA:               /* Saratoga */
    case CLI_FCCASCLOSEPICLOA:              /* Saratoga */
	case CLI_FCCASDELAYBALON:
	case CLI_FCCASALLIDREAD : /* Cashier All Id Read */
	case CLI_FCCASINDREAD :   /* Cashier individual Read */
        usReqMsgHLen = sizeof(CLIREQCASHIER);
        break;

    case CLI_FCGCFMASSIGN:                  /* GCF Functions */
    case CLI_FCGCRETURNSCLEAR:              /* GCF Returns unlock */
    case CLI_FCGCFSTORE:        
    case CLI_FCGCFREADLOCK: 
    case CLI_FCGCFLOCK:     
    case CLI_FCGCFDELETE:       
    case CLI_FCGCFAGENGCN:
    case CLI_FCGCFABAKGCN:
    case CLI_FCGCFREADAGCNO:
    case CLI_FCGCFASSIGN:       
    case CLI_FCGCFRESETFG:  
    case CLI_FCGCFINDREAD:  
    case CLI_FCGCFINFOQUE:                      /* Add R3.0 */  
    case CLI_FCGCFDELDLIVQUE:                   /* Add R3.0 */
    case CLI_FCGCFINDREADRAM:                   /* Add R3.0 */
    case CLI_FCGCFSEARCHQUE:                    /* Add R3.0 */
    case CLI_FCGCFCHECKREAD:                    /* Add R3.0 */
    case CLI_FCGCFRESETDVQUE:                   /* Add R3.0 */
    case CLI_FCGCFDVINDREAD:                    /* Add R3.0 */
    case CLI_FCGCFDVINDREADBAK:                 /* Add R3.0 */
    case CLI_FCGCFALLIDBW:                      /* V3.3 */
    case CLI_FCGCRTRVFIRST2:
        usReqMsgHLen = sizeof(CLIREQGCF);
        break;

    case CLI_FCTTLUPDATE:                   /* Total, update function */
    case CLI_FCRESETTRANSNO:
        usReqMsgHLen = sizeof(CLIREQTOTAL);
        break;  

	case CLI_FCGCREADFILTERED:
	case CLI_FCGCREADUNIQUEID:
        usReqMsgHLen = sizeof(CLIREQGCFFILTERED);
		break;
    case CLI_FCTTLREADTL:                   /* Saratoga */
    case CLI_FCTTLRESETTL:                  /* Total, other function */
    case CLI_FCTTLRESETCAS: 
    case CLI_FCTTLRESETWAI:   
    case CLI_FCTTLRESETINDFIN:
    case CLI_FCTTLISSUE:        
    case CLI_FCTTLISSUERST:     
    case CLI_FCTTLPLUOPTIMIZE:	/* 06/26/01 */
    case CLI_FCTTLDELETETTL:
	case CLI_FCTTLREADTLINCR:
	case CLI_FCTTLREADTLPLUEX:
        usReqMsgHLen = sizeof(CLIREQRESETTL);
        break;  

    case CLI_FCTTLREADTLCASTEND:            /* Saratoga */
        usReqMsgHLen = sizeof(CLIREQCASTEND);
        break;

    case CLI_FCOPREQALL:                    /* Ope. para functions */
    case CLI_FCOPREQALLFIN:
    case CLI_FCOPREQSUP:        
    case CLI_FCOPREQSUPFIN:
    case CLI_FCOPREQPLU:        
    case CLI_FCOPREQPLUFIN:
	case CLI_FCOPREQLAYFIN:					/* ESMITH LAYOUT */
	case CLI_FCOPPLUREADBDP :   /* OP PLU  Read by dept number    */
    case CLI_FCOPPLUREADBRC :   /* OP PLU  Read by report code    */
        usReqMsgHLen = sizeof(CLIREQOPPARA);
        break;  

	case    CLI_FCCOPONNENGINEMSG:
        usReqMsgHLen = sizeof(CLIREQOPCONNENGINE);
		break;

    case    CLI_FCCOPONNENGINEMSGFH:
        usReqMsgHLen = sizeof(CLIREQOPCONNENGINE);
		break;

	case CLI_FCOPJOINCLUSTER:          // process request from terminal to join cluster
        usReqMsgHLen = sizeof(CLIREQJOIN);
        break;  

	case CLI_FCOPUNJOINCLUSTER:          // process request from terminal to remove from cluster
        usReqMsgHLen = sizeof(CLIREQJOIN);
        break;  

	case CLI_FCOPREQMSGBUFFER:             // process request from terminal to display message to operator
        usReqMsgHLen = sizeof(CLIREQMSGBUFFER);
        break;  

	case CLI_FCOPTRANSFERTOTALS:         // process request from terminal to join cluster
        usReqMsgHLen = sizeof(CLIREQTRANSFER);
        break;  

	case CLI_FCOPUPDATETOTALSMSG:         // process request from terminal to update totals, TtlTUMUpdateTotalsMsg(), SerOpUpdateTotalsMsg().
        usReqMsgHLen = sizeof(CLIREQSFTOTALS);
        break;  

    case CLI_FCOPPLUINDREAD:                /* Read,        Saratoga */
        usReqMsgHLen = sizeof(CLIREQSCNPLU);
        break;

    case CLI_FCOPPLUUPDATE:                 /* Update a PLU,         Saratoga */
    case CLI_FCOPPLUASSIGN:                 /* Add,         Saratoga */
    case CLI_FCOPPLUDELETE:                 /* Delete,      Saratoga */
        usReqMsgHLen = sizeof(CLIREQMNTPLU);
        break;

	case CLI_FCOPPLUOEPREAD:
        usReqMsgHLen = sizeof(CLIREQOEPPLU);
        break;

    case CLI_FCPLUENTERCRITMD:              /* Enter Crit,  Saratoga */
        usReqMsgHLen = sizeof(CLIREQPLUECM);
        break;

    case CLI_FCPLUEXITCRITMD:               /* Exit Crit,   Saratoga */
        usReqMsgHLen = sizeof(CLIREQPLUCOM);
        break;  

    case CLI_FCKPSPRINT:                    /* Kitchin, Print function */
        usReqMsgHLen = sizeof(CLIREQKITPRINT);
        break;
    
    case CLI_FCKPSALTMANKP:                 /* Kitchin, alternate function */
        usReqMsgHLen = sizeof(CLIREQALTKP);
        break;

    case CLI_FCDFLPRINT:                    /* DFL Display on flay     */
        usReqMsgHLen = sizeof(CLIREQDFL);
        break;

    case CLI_FCSPSPRINT:                    /* SPS print  */
    case CLI_FCSPSTERMLOCK:                 /* SPS Lock   */
    case CLI_FCSPSTERMUNLOCK:               /* SPS Unlock */
    case CLI_FCSPSPOLLING:                  /* SPS Polling R3.0 */
    case CLI_FCSPSPRINTKP:                  /* SPS print KP R3.0 */
	case CLI_FCSPSPRINTSTAT:                // SPS printer status request
        usReqMsgHLen = sizeof(CLIREQSHR);
        break;

	case CLI_FCETKCURINDREAD:           /* ETK cur ind id read   */
	case CLI_FCETKCURALLIDRD:				/* ETK all cur id read   */
    case CLI_FCETKTIMEIN:                   /* ETK time in */
    case CLI_FCETKTIMEOUT:                  /* ETK time out */
    case CLI_FCETKASSIGN:                   /* ETK assign   */
    case CLI_FCETKDELETE:                   /* ETK delete   */
    case CLI_FCETKALLRESET:                 /* ETK all reset  */
    case CLI_FCETKISSUEDSET:                /* ETK issued set */
    case CLI_FCETKSTATREAD:                 /* ETK status read  */
    case CLI_FCETKCURINDWRITE:              /* ETK cur ind write */
	case CLI_FCETKINDJOBREAD:				/* Read last entry in someone's ETK file GSU SR 8 cwunn*/
        usReqMsgHLen = sizeof(CLIREQETK);
        break;

    case CLI_FCCPMSENDMESS:                 /* CPM Function */
        usReqMsgHLen = sizeof(CLIREQCPM);
        break;

    case CLI_FCBAKCASHIER:                  /* Backup copy function */
    case CLI_FCBAKWAITER:       
    case CLI_FCBAKTOTAL:        
    case CLI_FCBAKGCF:
    case CLI_FCBAKOPPARA:
    case CLI_FCBAKETK:
    case CLI_FCBAKPLU:                      /* Saratoga */
    case CLI_FCBAKPROGRPT:                  /* program report R3.1 */
    case CLI_FCBAKFINAL:
        usReqMsgHLen = sizeof(CLIREQBACKUP);
        break;

    /* === New Functions - EJ Cluster Reset (Release 3.1) BEGIN === */
    case CLI_FCEJCLEAR:
    case CLI_FCEJREADFILE:
    case CLI_FCEJREAD:
    case CLI_FCEJLOCK:      /* V3.3 */
    case CLI_FCEJUNLOCK:    /* V3.3 */
        usReqMsgHLen = sizeof(CLIREQEJ);
        break;    
    /* === New Functions - EJ Cluster Reset (Release 3.1) END === */
	case CLI_FCRMTREADFILE:
		usReqMsgHLen = sizeof(CLIREQREAD); //ESMITH LAYOUT
		break;

    default:
        usReqMsgHLen = 0;
        break;
    }
    return usReqMsgHLen;
}

/*
*===========================================================================
** Synopsis:    SHORT    SerChekResponse(VOID);
*
** Return:      SERV_SUCCESS:   OK 
*               SERV_BUSY:      I am Busy
*               SERV_NOT_MASTER:    I am not As Master
*
** Description: This function checks function control code of request.
*===========================================================================
*/
SHORT   SerChekResponse(VOID)
{
    CLIRESCOM   *pResp = (CLIRESCOM *)SerRcvBuf.auchData;
    USHORT      fsComStatus;
    
    fsComStatus = SerComReadStatus();
    if (SER_IAM_MASTER) {
        if (0 == (fsComStatus & SER_COMSTS_M_UPDATE)) {
            return SERV_NOT_MASTER;             /* I am Out Of Service */
        }

		// If we have a Backup Master Terminal provisioned in the cluster
		// then we check to see if the function code has the CLI_SETBMDOWN
		// mask turned on.  If so, it indicates that the terminal that sent
		// this message believes that the Backup Master Terminal is down.
		// See the functions CstComMakeMultiData() and CstComMakeMultiDataFH()
		// depending on the value of CstComReadStatus() & CLI_STS_BM_UPDATE
		// if the target terminal is the Master Terminal.
		if (usBMOption == 1) {
			if (0 == (fsComStatus & SER_COMSTS_BM_UPDATE)) {
				if (0 == (pResp->usFunCode & CLI_SETBMDOWN)) {
					return SERV_BUSY;               /* Status Unmatched */
				}
			} else {
				if (pResp->usFunCode & CLI_SETBMDOWN) {
					return SERV_BUSY;               /* Status Unmatched */
				}
			}
		}
    }
    if (SER_IAM_BMASTER) {
        if (0 == (fsComStatus & SER_COMSTS_BM_UPDATE)) {
            return SERV_NOT_MASTER;
        }
        if (0 == (fsComStatus & SER_COMSTS_M_UPDATE)) {
            if (pResp->usFunCode & CLI_FCWBACKUP) {
                return SERV_BUSY;               /* Status Unmatched */
            }
        } else {
            if (0 == (pResp->usFunCode & CLI_FCWBACKUP)) {
                return SERV_BUSY;               /* Status Unmatched */
            }
        }
    }
    return SERV_SUCCESS;
}



/*===== END OF SOURCE =====*/