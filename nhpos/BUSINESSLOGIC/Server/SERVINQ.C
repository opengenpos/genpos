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
* Title       : SERVER module, Source File                        
* Category    : SERVER module, Inquiry Functions
* Program Name: SERVINQ.C                                            
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           SerRecvInq()            Receive Entry
*           SerRecvInquiry()        Receive Inquiry Request/ Response
*           SerRecvInqChgDate()     Receive Change Date Req./ Resp.
*           SerRecvInqReqInq()      Receive Request Inquiry
*
*           SerSendInquiry()        Send Inquiry Request
*           SerSendInqResp()        Send Inquiry Response
*           SerSendInqDate()        Send Inquiry Date
*           SerSendInqDateResp()    Send Inquiry Date Response
*           SerSendInqReqInq()      Send Request Inquiry
*
*           SerChekInqDate()        Check Inquiry Date
*           SerChekInqStatus()      Check and Decide Inquiry Status
*
*           SerConvertInqDate()     Convert struc. DATE_TIME -> CLIINQDATE
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* Jun-04-93:H.Yamaguchi: Modified same as HP Int'l
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
#include	<tchar.h>
#include    <stdio.h>
#include    <memory.h>

#include    <ecr.h>
#include    <pif.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    "servmin.h"

/*
*===========================================================================
** Synopsis:    VOID    SerRecvInq(VOID);
*
** Return:      none.
*
** Description: This function is a entry od all inqury function.
*===========================================================================
*/
VOID    SerRecvInq(VOID)
{
    CLIRESCOM   *pResp;
    
    pResp = (CLIRESCOM *)SerRcvBuf.auchData;

    switch(pResp->usFunCode) {
    case    CLI_FCINQINQUIRY:               /* Inquiry */
        SerRecvInquiry();
        break;

    case    CLI_FCINQCHGDATE:               /* change Date */
        SerRecvInqChgDate();
        break;

    case    CLI_FCINQREQINQ:                /* Request Inquiry from BM */
        SerRecvInqReqInq();
        break;

    default:                                /* not used */
        break;
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerRecvInquiry(VOID);
*
** Return:      none.
*
** Description:
*   This function executes handling of Inquiry Request (BM) and
*   Response (MT).
*===========================================================================
*/
VOID    SerRecvInquiry(VOID)
{
    CLIRESINQUIRY   *pResMsgH;
    CLIREQCOM       *pSend;

    if (SER_IAM_MASTER) {                   /** Response from BM **/
        SerChangeStatus(SER_STINQUIRY);     /* timer start */
        pSend = (CLIREQCOM *)auchSerTmpBuf;
        pResMsgH = (CLIRESINQUIRY *)SerRcvBuf.auchData;
        if (CLI_FCINQINQUIRY != pSend->usFunCode) {
            return;                         /* not expected response */
        } else if (STUB_SUCCESS != pResMsgH->sResCode) {
            PifSleep(SER_INQUIRY_TIMER);      /* delay */
            SerSendInquiry();
        } else if (SERV_SUCCESS != SerChekInqDate()) {
            PifSleep(SER_INQUIRY_TIMER);      /* delay */
            SerSendInquiry();               /* retry inquiry */
        } else if (SERV_SUCCESS != SerChekInqStatus()) {
            PifSleep(SER_INQUIRY_TIMER);      /* delay */
            SerSendInquiry();               /* retry inquiry */
        } else {
            SerSendInqDate();               /* send Change Inquiry Date */
        }
    } else if (SER_IAM_BMASTER) {           /** Request from Master **/
        SerChangeStatus(SER_STINQUIRY);     /* timer start */
        SerSendInqResp();                   /* send Inquiry Response */
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerRecvInqChgDate(VOID);
*
** Return:      none.
*
** Description:
*   This function executes handling of Change Date Request (BM) and 
*   Response (MT).
*===========================================================================
*/
VOID    SerRecvInqChgDate(VOID)
{
    CLIREQINQUIRY   *pReqMsgH;
    CLIREQCOM       *pSend;
    USHORT          fsSpeFlag;

    if (SER_IAM_MASTER) {                   /** Response from BM **/
        pSend = (CLIREQCOM *)auchSerTmpBuf;
        if (CLI_FCINQCHGDATE != pSend->usFunCode) {
            return;                         /* not expected response */
        }
    } else if (SER_IAM_BMASTER) {           /** request from Master **/
        SerSendInqDateResp();               
        pReqMsgH = (CLIREQINQUIRY *)SerRcvBuf.auchData;
        SstChangeInqStat(pReqMsgH->fsStatus);
        SstChangeInqDate(&pReqMsgH->Date);
    }
    if (SER_STBACKUP == usSerStatus) {      /* back up state now ? */
        SstReadFlag(&fsSpeFlag);
        if (fsSpeFlag & SER_SPESTS_AC42) {
            sSerExeError = STUB_SUCCESS;
            SstResetFlag(SER_SPESTS_AC42);
            PifReleaseSem(usSerCliSem);
        }
    }
    SerChangeStatus(SER_STNORMAL);          /* timer stop */
    SerSendNtbStart();                      /* start on-line information */
}

/*
*===========================================================================
** Synopsis:    VOID    SerRecvInqReqInq(VOID);
*
** Return:      none.
*
** Description:
*   This function executes handling of Request Inquiry Request (MT) and 
*   Response (BM, BUSY from Master).
*===========================================================================
*/
VOID    SerRecvInqReqInq(VOID)
{
    if (SER_IAM_MASTER) {                   /** Request from BM **/
        SerChangeStatus(SER_STINQUIRY);     /* timer start */
        SerSendInquiry();                   /* send inquiry to BM */
    } else if (SER_IAM_BMASTER) {           /** Response (BUSY) from MT **/
        SerSendInqReqInq();                 /* retry to send req. inquiry */
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerSendInquiry(VOID);
*
** Return:      none.
*
** Description: This function sends Inquiry Request to BM.
*===========================================================================
*/
VOID    SerSendInquiry(VOID)
{
	SHORT           sSerSendStatus = 0;
    CLIREQINQUIRY   ReqMsgH;
    CLIREQCOM       *pSend;
    SERINQSTATUS    InqData;
   
    SstReadInqStat(&InqData);
    pSend = (CLIREQCOM *)auchSerTmpBuf;
    memset(&ReqMsgH, 0, sizeof(CLIREQINQUIRY));
    pSend->usFunCode    = CLI_FCINQINQUIRY;
    ReqMsgH.usFunCode   = CLI_FCINQINQUIRY;
    ReqMsgH.fsStatus    = InqData.usStatus;
    memcpy(&ReqMsgH.Date, &InqData.CurDate, sizeof(CLIINQDATE));
    memcpy(ReqMsgH.ausTranNo, InqData.ausTranNo, 2*CLI_ALLTRANSNO);
    memcpy(ReqMsgH.ausPreTranNo, InqData.ausPreTranNo, 2*CLI_ALLTRANSNO);

    sSerSendStatus = SerSendRequest(CLI_TGT_BMASTER, (CLIREQCOM *)&ReqMsgH, sizeof(CLIREQINQUIRY), NULL, 0);
	if (sSerSendStatus < 0) {
		char xBuff [128];
		sprintf (xBuff, "SerSendInquiry(): sSerSendStatus = %d", sSerSendStatus);
		NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
	}
}

/*
*===========================================================================
** Synopsis:    VOID    SerSendInqResp(VOID);
*
** Return:      none.
*
** Description: This function sends Inquiry Response to Master.
*===========================================================================
*/
VOID    SerSendInqResp(VOID)
{
	SHORT           sSerSendStatus = 0;
    CLIRESINQUIRY   ResMsgH;
    SERINQSTATUS    InqData;
   
    SstReadInqStat(&InqData);
    memset(&ResMsgH, 0, sizeof(CLIRESINQUIRY));
    ResMsgH.usFunCode   = CLI_FCINQINQUIRY;
    ResMsgH.sResCode    = STUB_SUCCESS;
    ResMsgH.fsStatus    = InqData.usStatus;
    memcpy(&ResMsgH.Date, &InqData.CurDate, sizeof(CLIINQDATE));
    memcpy(ResMsgH.ausTranNo, InqData.ausTranNo, 2*CLI_ALLTRANSNO);
    memcpy(ResMsgH.ausPreTranNo, InqData.ausPreTranNo, 2*CLI_ALLTRANSNO);

    sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESINQUIRY), NULL, 0);
	if (sSerSendStatus < 0) {
		char xBuff [128];
		sprintf (xBuff, "SerSendInqResp(): sSerSendStatus = %d", sSerSendStatus);
		NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
	}
}

/*
*===========================================================================
** Synopsis:    VOID    SerSendInqDate(VOID);
*
** Return:      none.
*
** Description: This function sends Change Date request to BM.
*===========================================================================
*/
VOID    SerSendInqDate(VOID)
{
	SHORT           sSerSendStatus = 0;
    CLIREQINQUIRY   ReqMsgH;
    CLIREQCOM       *pSend;
    SERINQSTATUS    InqData;
   
    SstReadInqStat(&InqData);
    pSend = (CLIREQCOM *)auchSerTmpBuf;
    memset(&ReqMsgH, 0, sizeof(CLIREQINQUIRY));
    pSend->usFunCode    = CLI_FCINQCHGDATE;
    ReqMsgH.usFunCode   = CLI_FCINQCHGDATE;
    ReqMsgH.fsStatus    = InqData.usStatus;
    memcpy(&ReqMsgH.Date, &InqData.CurDate, sizeof(CLIINQDATE));
    memcpy(ReqMsgH.ausTranNo, InqData.ausTranNo, 2*CLI_ALLTRANSNO);
    memcpy(ReqMsgH.ausPreTranNo, InqData.ausPreTranNo, 2*CLI_ALLTRANSNO);

    sSerSendStatus = SerSendRequest(CLI_TGT_BMASTER, (CLIREQCOM *)&ReqMsgH, sizeof(CLIREQINQUIRY), NULL, 0);
	if (sSerSendStatus < 0) {
		char xBuff [128];
		sprintf (xBuff, "SerSendInqDate(): sSerSendStatus = %d", sSerSendStatus);
		NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
	}
}

/*
*===========================================================================
** Synopsis:    VOID    SerSendInqDateResp(VOID);
*
** Return:      none.
*
** Description: This function sends Change Date Response to Master.
*===========================================================================
*/
VOID    SerSendInqDateResp(VOID)
{
	SHORT           sSerSendStatus = 0;
    CLIRESINQUIRY   ResMsgH;

    memset(&ResMsgH, 0, sizeof(CLIRESINQUIRY));
    ResMsgH.usFunCode   = CLI_FCINQCHGDATE;
    ResMsgH.sResCode    = STUB_SUCCESS;

    sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESINQUIRY), NULL, 0);
	if (sSerSendStatus < 0) {
		char xBuff [128];
		sprintf (xBuff, "SerSendInqDateResp(): sSerSendStatus = %d", sSerSendStatus);
		NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
	}
}

/*
*===========================================================================
** Synopsis:    VOID    SerSendInqReqInq(VOID);
*
** Return:      none.
*
** Description: This function sends Request Inquiry Request to Master
*===========================================================================
*/
VOID    SerSendInqReqInq(VOID)
{
	SHORT           sSerSendStatus = 0;
    CLIREQINQUIRY   ReqMsgH;

    memset(&ReqMsgH, 0, sizeof(CLIREQINQUIRY));
    ReqMsgH.usFunCode   = CLI_FCINQREQINQ;

    sSerSendStatus = SerSendRequest(CLI_TGT_MASTER, (CLIREQCOM *)&ReqMsgH, sizeof(CLIREQINQUIRY), NULL, 0);
	if (sSerSendStatus < 0) {
		char xBuff [128];
		sprintf (xBuff, "SerSendInqDateResp(): sSerSendStatus = %d", sSerSendStatus);
		NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
	}
}

/*
*===========================================================================
** Synopsis:    SHORT   SerChekInqDate(VOID);
*
** Return:      SERV_SUCCESS:   successful comparation
*               SERV_ILLEGAL:   fail to compare
*
** Description: This function checks Inquiry Date between Master and BM.
*===========================================================================
*/
SHORT   SerChekInqDate(VOID)
{
    CLIRESINQUIRY   *pResMsgH;
    SERINQSTATUS    InqData;
   
    SstReadInqStat(&InqData);
    pResMsgH = (CLIRESINQUIRY *)SerRcvBuf.auchData;

    if ((0 == InqData.CurDate.usDay) && (0 == InqData.usStatus)) {
        return SERV_SUCCESS;
    }
    if ((0 == pResMsgH->Date.usDay) && (0 == pResMsgH->fsStatus)) {
        return SERV_SUCCESS;
    }
    if (0 == memcmp(&InqData.CurDate, &pResMsgH->Date, sizeof(CLIINQDATE))) { 
        return SERV_SUCCESS;
    }

	// If we do not have a date but the other terminal, Master or Backup, does
	// then we will assume that the other terminal is up to date.
	// This could be true with a Master Terminal replacement scenario
	// in which a Master Terminal is being replaced and the Backup Terminal
	// has the latest data.
	if ((0 == InqData.CurDate.usDay) && (0 != pResMsgH->Date.usDay)) {
		return SERV_SUCCESS;
	}
    return SERV_ILLEGAL;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerChekInqStatus(VOID);
*
** Return:      SERV_SUCCESS:   successful deciding new status
*               SERV_ILLEGAL:   fail to decide new status
*
** Description:
*   This function checks Inquiry Status between Master and BM, and decides
*   New Inquiry Status.
*===========================================================================
*/
SHORT   SerChekInqStatus(VOID)
{
    CLIRESINQUIRY   *pResMsgH;
    USHORT          usNewStatus, usTerm;
    SERINQSTATUS    InqData;
   
    SstReadInqStat(&InqData);
    pResMsgH = (CLIRESINQUIRY *)SerRcvBuf.auchData;
                                            
    if ((0 == InqData.usStatus) && (0 == pResMsgH->fsStatus)) {
        SstChangeInqStat(SER_INQ_M_UPDATE);         /* I/R Both */
        return SERV_SUCCESS;
    }
    if (0 == pResMsgH->fsStatus) {                  /* I/R BM */
        if (InqData.usStatus & SER_INQ_M_UPDATE) {
            SstChangeInqStat(SER_INQ_M_UPDATE);
            return SERV_SUCCESS;
        }
        if (InqData.usStatus & SER_INQ_BM_UPDATE) {
            SstChangeInqStat(SER_INQ_BM_UPDATE);
            return SERV_SUCCESS;
        }
        return SERV_ILLEGAL;
    }
    if (0 == InqData.usStatus) {                    /* I/R Master */
        if (pResMsgH->fsStatus & SER_INQ_BM_UPDATE) {
            SstChangeInqStat(SER_INQ_BM_UPDATE);
            return SERV_SUCCESS;
        }
        if (pResMsgH->fsStatus & SER_INQ_M_UPDATE) {
            SstChangeInqStat(SER_INQ_M_UPDATE);
            return SERV_SUCCESS;
        }
        return SERV_ILLEGAL;
    }

	// We will now determine whether the two terminals agree on the
	// current status or not.  Bitwise And this terminal's status with
	// the status reported by the other terminal.  If the result is zero
	// then the terminals do not agree on the status.
    usNewStatus = InqData.usStatus & pResMsgH->fsStatus;
    if (0 == usNewStatus) {
        return SERV_ILLEGAL;
    }
    if ((SER_INQ_M_UPDATE | SER_INQ_BM_UPDATE) == usNewStatus) {
        for (usTerm=0; usTerm < CLI_ALLTRANSNO; usTerm++) {
            if (InqData.ausTranNo[usTerm] == 
                pResMsgH->ausTranNo[usTerm]) {
                continue;
            }
            if (InqData.ausTranNo[usTerm] == 
                (pResMsgH->ausTranNo[usTerm] + 1)) {
                continue;
            }
            return SERV_ILLEGAL;
        }
    }
    SstChangeInqStat(usNewStatus);
    return SERV_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    VOID   SerConvertInqDate(DATE_TIME FAR *pSource,
*                                        CLIINQDATE *pDestine);
*
*     Input:    pSource - pointer to source
*    OutPut:    pDestine    - pointer to output
*
** Return:      none.
*
** Description:
*   This function converts DATE_TIME (PIF) structure to CLIINQDATE strcutre.
*===========================================================================
*/
VOID    SerConvertInqDate(DATE_TIME *pSource, CLIINQDATE *pDestine)
{
    pDestine->usYear    = pSource->usYear;
    pDestine->usMonth   = pSource->usMonth;
    pDestine->usDay     = pSource->usMDay;
    pDestine->usHour    = pSource->usHour;
    pDestine->usMin     = pSource->usMin;
    pDestine->usSec     = pSource->usSec;
}

/*===== END OF SOURCE =====*/


