/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : ISP Server module, Receive Message Handling Source File                        
* Category    : ISP Serber , US Hospitality Model
* Program Name: ISPRMH.C                                            
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*       IspRMHPassWord();          Receive during password state
*       IspRMHNormal();         C  Receive during normal   state
*       IspRMHMulSnd();            Receive during multi.   sending state 
*       IspRMHMulRcv();            Receive during multi.   receiving state 
*                               
*       IspRecvNormal();           Call each function
*       IspRecvMultiple();         Check multiple reciving
*       IspRecvNextFrame();        Received next frame
*       IspSendMultiple();         Send multiple response
*       IspSendNextFrame();        Send next frame response
*       IspCheckFunCode();         Check function code valid ?
*       IspCheckResponse();     C  Check response condition
*       IspCheckNetRcvData();      Check received data length 
*       IspRecvSatCasWai();     A  Works as satellite
*
* --------------------------------------------------------------------------
* Update Histories                                                         
* Date     :Ver.Rev.:   Name    : Description
* Jun-22-92:00.00.01:H.Yamaguchi: initial                                   
* Sep-23-92:00.00.02:T.Kojima   : add to CLI_RMTFILE in "IspRecvNormal()"                                   
*          :        :           : add to RFS function code to IspCheckFunCode                                   
* Jul-05-93:00.00.01:H.Yamaguchi: Modified Enhance
* May-17-94:02.05.00:Yoshiko.Jim: add CLI_FCTTLWAITTLCHK, CLI_FCTTLCASTTLCHK                                  
* Jun-27-95:02.05.00:M.Suzuki   : R3.0                                  
* May-07-96:03.01.05:T.Nakahata : R3.1
* Aug-11-99:03.05.00:M.Teraki   : Remove WAITER_MODEL
* Dec-07-99:01.00.00:hrkato     : Saratoga
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
#include    <memory.h>

#include    <ecr.h>
#include    <pif.h>
#include	<regstrct.h>
#include    <plu.h>
#include	<paraequ.h>
#include	<para.h>
#include	<pararam.h>
#include    <cscas.h>
#include    <cswai.h>
#include    <rfl.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <storage.h>
#include    <isp.h>
#include    <cpm.h>
#include    <eept.h>
#include    <csstbcpm.h>
#include    <ej.h>
#include    "ispcom.h"
#include    <appllog.h>
#include    "rmt.h"             /* Remote File System   */
#include	"display.h"
#include	"csstbcas.h"
#include	<csstbej.h>


/*
*===========================================================================
** Synopsis:    VOID    IspRMHPassWord(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:  This function executes request handling during passward
*                state.
*===========================================================================
*/
VOID    IspRMHPassWord(VOID)
{
    CLIREQCOM   *pResp = (CLIREQCOM *)IspRcvBuf.auchData;

    if ( CLI_FCISPLOGON == (pResp->usFunCode & CLI_RSTCONTCODE) ) {
		SHORT  fsComStatus;
 
        fsComStatus = IspCheckResponse();           /* Read status */
        if ( (ISP_SUCCESS != fsComStatus ) && (ISP_ERR_NOT_MASTER != fsComStatus) ) {/* Check current terinal status */  
            IspSendError(fsComStatus);              /* Error response */
            return;
        }

        IspRecvPass();                              /* Check password */
	} else if ( CLI_FCOSYSTEMSTATUS == (pResp->usFunCode & CLI_RSTCONTCODE) ) {
		// we will allow retrieval of system status whether ISP is logged in or not
		// this allows ISP interface to provide a way to perform a quick system
		// status without having to perform a login and log out.
		IspSendSystemStatusResponse();
    } else {
        IspSendError(ISP_ERR_REQ_VERIFYPASS);   /* Error response */
    }
}

/*
*===========================================================================
** Synopsis:    VOID    IspRMHNormal(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function executes request handling during Normal state.
*===========================================================================
*/
VOID    IspRMHNormal(VOID)
{
    CLIREQCOM   *pResp = (CLIREQCOM *)IspRcvBuf.auchData;
 
    if (CLI_FCBAK == (pResp->usFunCode & CLI_BAKMSG_TYPE)) {
        IspSendError(ISP_ERR_INVALID_MESSAGE);  /* Error response */
    } else {
		SHORT  sError;

        sError = IspRecvMultiple(pResp);
        if (STUB_SUCCESS != sError) {
            IspSendError(sError);         /* Error response */
        } else {
            IspRecvNormal(pResp);         /* Normal response */
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID    IspRMHMulSnd(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:
*   This function executes request handling during Multi. sending state.
*===========================================================================
*/
VOID    IspRMHMulSnd(VOID)
{
    CLIREQCOM   *pResp = (CLIREQCOM *)IspRcvBuf.auchData;

    if (CLI_FCINQ == (pResp->usFunCode & CLI_FCMSG_TYPE)) {
        IspSendError(ISP_ERR_INVALID_MESSAGE);   /* Error response */
    } else if (CLI_FCBAK == (pResp->usFunCode & CLI_BAKMSG_TYPE)) {
        IspSendError(ISP_ERR_INVALID_MESSAGE);   /* Error response */
    } else {
		SHORT  sError;

        sError = IspSendNextFrame(pResp);        /* Check next frame data */
        if (STUB_MULTI_SEND == sError) {
                                                 /* Current is blank */
        } else if (STUB_SUCCESS == sError) {
            IspRecvNormal(pResp);             
        } else {
            IspSendError(sError);                /* Error response  */
            if (ISP_ST_NORMAL == usIspState ) {  /* if NORMAL state */
                IspCheckGCFUnLock();             /* Unlock a GCF    */
            }
        }
    }
}
    
/*
*===========================================================================
** Synopsis:    VOID    IspRMHMulRcv(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:
*   This function executes request handling during Multi. receiving state.
*===========================================================================
*/
VOID    IspRMHMulRcv(VOID)
{
    CLIREQCOM   *pResp = (CLIREQCOM *)IspRcvBuf.auchData;

    if (CLI_FCINQ == (pResp->usFunCode & CLI_FCMSG_TYPE)) {
        IspSendError(ISP_ERR_INVALID_MESSAGE);  /* Error response */
    } else if (CLI_FCBAK == (pResp->usFunCode & CLI_BAKMSG_TYPE)) {
        IspSendError(ISP_ERR_INVALID_MESSAGE);  /* Error response */
    } else {
		SHORT  sError;

        sError = IspRecvNextFrame(pResp);      /*  Receive next frame data */
        if (STUB_SUCCESS == sError) {
            IspRecvNormal(pResp);              /*  Execute a received function */
        } else {
            IspSendError(sError);              /* Error response */
        }
    }
}
            

/*
*===========================================================================
** Synopsis:    VOID    IspRecvNormal(CLIREQCOM *pReqMsgH);
*     InPut:    pReqMsgH    - Pointer of received request message
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function decides a function to call each modele.
*===========================================================================
*/
VOID    IspRecvNormal(CLIREQCOM *pReqMsgH)
{
    SHORT      fsComStatus;
    USHORT     usFun = (pReqMsgH->usFunCode & CLI_FCMSG_TYPE);   /* What kind of message ? */

    /* --- clean up ej status, after the comm. down of ej reset, R2.0 GCA */
    if (CLI_FCEJ != usFun)
    {
        IspCleanUpEJ();                         /* Clean-Up E/J Status,   V3.3 */
        EJUnLock(EJLOCKFG_ISP);                 /* Unlock ISP Reset Flag, V3.3 */
    }

    if (CLI_FCINQ == usFun) {
        IspSendError(ISP_ERR_INVALID_MESSAGE);      /* Error response */
        return;
    }

	if ( CLI_FCOSYSTEMSTATUS == usFun ) {
		IspSendSystemStatusResponse();
        return;
	}

    fsComStatus = IspCheckResponse();               /* Read status */

    if (ISP_SUCCESS != fsComStatus ) {   /* Works As Satellite */
        if (ISP_ERR_NOT_MASTER != fsComStatus) { /* Check current terinal status */  
            IspSendPassResponse(fsComStatus);    /* Send Err response */
            return;
        }

        switch( usFun ) {  /* What kind of message ? */
        case    CLI_FCCAS:              /* Cashier Function */
            IspRecvSatCasWai((USHORT)(pReqMsgH->usFunCode & CLI_RSTCONTCODE));
            break;
        
        case    CLI_FCOP:               /* Op. Para and Parameter Function */
            IspRecvOpr();
            break;
        
        case    CLI_FCISP:              /* ISP Function */
            IspRecvPass();
            break;
        
        case    CLI_FCRMTFILE:          /* Remote File System */
            RmtFileServer();
            break;
        
        case    CLI_FCEJ:               /* EJ  commom   */
            IspRecvEj();
            break;
        
        default:                        /* not used */
            IspSendPassResponse(ISP_ERR_NOT_MASTER);    /* Send Err response */
            break;
        }
    }  else {          /* Works As Master  */
        switch( usFun ) {  /* What kind of message ? */
        case    CLI_FCCAS:              /* Cashier Function */
            IspRecvCas();
            break;

        case    CLI_FCGCF:              /* Guest Check File Function */
            IspRecvGCF();
            break;
        
        case    CLI_FCTTL:              /* Total File Function */
            IspRecvTtl();
            break;
        
        case    CLI_FCOP:               /* Op. Para and Parameter Function */
            IspRecvOpr();
            break;
        
        case    CLI_FCISP:              /* ISP Function */
            IspRecvPass();
            break;
        
        case    CLI_FCRMTFILE:          /* Remote File System */
            RmtFileServer();
            break;
        
        case    CLI_FCEJ:               /* EJ  commom   */
            IspRecvEj();
            break;
        
        case    CLI_FCETK:              /* ETK commom   */
            IspRecvEtk();
            break;
        
        case    CLI_FCCPM:              /* CPM commom   */
            IspRecvCpm();
            break;

        default:                        /* not used */
            IspSendPassResponse(ISP_ERR_INVALID_MESSAGE);    /* Send Err response */
            break;
        }
    }
}    

/*
*===========================================================================
** Synopsis:    SHORT   IspRecvMultiple(CLIREQCOM *pReqMsgH);
*     Input:    pReqMsgH    - Pointer of received request message
*     Output:   nothing
*     InOut:    nothing
*
** Return:      STUB_SUCCESS:      Normal request received or all data received
*               STUB_MULTI_RECV:   Multi data received
*               STUB_FRAME_SEQERR: Frame sequence # is not 1
*
** Description: This function checks to receive multi. data.
*===========================================================================
*/
SHORT   IspRecvMultiple(CLIREQCOM *pReqMsgH)
{
    CLIREQDATA  *pReqBuff;
    USHORT      usReqMsgHLen;

    if (0 == pReqMsgH->usSeqNo) {    
        return (STUB_SUCCESS);       /* Received without data function */
    }
    if (1 != (pReqMsgH->usSeqNo & CLI_SEQ_CONT)) {
        return (STUB_FRAME_SEQERR + STUB_RETCODE);                  /* Must Seq No = 1 */
    }
    if (0 == (pReqMsgH->usSeqNo & CLI_SEQ_SENDDATA)) {
        return (STUB_SUCCESS);                       /* Received 1 frame data */
    }                                                

    usReqMsgHLen     = IspCheckFunCode(pReqMsgH->usFunCode);
    pReqBuff         = (CLIREQDATA *)((UCHAR *)pReqMsgH + usReqMsgHLen);
    IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[usReqMsgHLen];

    memcpy(auchIspTmpBuf, pReqMsgH, usReqMsgHLen);       /* Save header */
    IspResp.pSavBuff->usDataLen = pReqBuff->usDataLen;
    memcpy(IspResp.pSavBuff->auchData, pReqBuff->auchData, pReqBuff->usDataLen);
    
    if (0 == (pReqMsgH->usSeqNo & CLI_SEQ_SENDEND)) {    /* if not last data */
        IspResp.uchPrevUA     = IspRcvBuf.auchFaddr[CLI_POS_UA];
        IspResp.usPrevMsgHLen = usReqMsgHLen;
        IspChangeStatus(ISP_ST_MULTI_RECV);     /* Set MULTI_RECV state */     
        return (STUB_MULTI_RECV);
    }
    return (STUB_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   IspRecvNextFrame(CLIREQCOM *pReqMsgH);
*     Input:    pReqMsgH    - Pointer of received request message
*     Output:   nothing
*     InOut:    nothing
*
** Return:      STUB_SUCCESS:       Normal request received or all data received
*               STUB_MULTI_RECV:    Multi data received
*               STUB_BUSY:          Multi request received from another terminal
*               STUB_FAITAL:        Another function code detected
*               STUB_FRAME_SEQERR:  Frame sequence error detected    
*
** Description: This function checks to receive during multi. receiving state.
*===========================================================================
*/
SHORT   IspRecvNextFrame(CLIREQCOM *pReqMsgH)
{
    CLIREQCOM   *pSavMsgH = (CLIREQCOM *)auchIspTmpBuf;
    CLIREQDATA  *pReqBuff;        
                                
    if (0 == pReqMsgH->usSeqNo) {               /* Another req. received */
        IspChangeStatus(ISP_ST_NORMAL);
        return (STUB_FAITAL + STUB_RETCODE);
    }                                           /* Another UA ? */
    if (IspRcvBuf.auchFaddr[CLI_POS_UA] != IspResp.uchPrevUA) {
        IspChangeStatus(ISP_ST_NORMAL);
        return (STUB_FAITAL + STUB_RETCODE);
    }                                           
    /* Same function code ? */
    if ((pReqMsgH->usFunCode & CLI_RSTCONTCODE) != 
        (pSavMsgH->usFunCode & CLI_RSTCONTCODE)) {
        IspChangeStatus(ISP_ST_NORMAL);
        return (STUB_FAITAL + STUB_RETCODE);
    }                                           /* sequence # OK ? */
    if ((pReqMsgH->usSeqNo & CLI_SEQ_CONT) != 
        ((pSavMsgH->usSeqNo & CLI_SEQ_CONT) + 1)) {
        IspChangeStatus(ISP_ST_NORMAL);
        return (STUB_FRAME_SEQERR + STUB_RETCODE);
    }
    pReqBuff = (CLIREQDATA *)((UCHAR *)pReqMsgH + IspResp.usPrevMsgHLen);
    memcpy(IspResp.pSavBuff->auchData + IspResp.pSavBuff->usDataLen, pReqBuff->auchData, pReqBuff->usDataLen);

    IspResp.pSavBuff->usDataLen += pReqBuff->usDataLen;
    
    if (0 == (pReqMsgH->usSeqNo & CLI_SEQ_SENDEND)) {  /* if not last data */
        ++ pSavMsgH->usSeqNo;               /* Increment a saved seq. no */
        IspChangeStatus(ISP_ST_MULTI_RECV);           
        return (STUB_MULTI_RECV);
    }
    IspChangeStatus(ISP_ST_NORMAL);          /* Received a last frame data */
    return (STUB_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID    IspSendMultiple(CLIRESCOM *pResMsgH,
*                                       USHORT usResMsgHLen);
*
*     InPut:    pResMsgH        - Response message header (now responded)
*               usResMsgHLen    - Response message header length
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function sends multiple response.
*===========================================================================
*/
VOID    IspSendMultiple(CLIRESCOM *pResMsgH, USHORT usResMsgHLen)
{
    USHORT      usDataMax, usDataLen;

    if (ISP_ST_NORMAL == usIspState) {         /* Is current NORMAL ? */
        IspResp.usPrevDataOff = 0;
        IspResp.usPrevMsgHLen = usResMsgHLen;
        IspResp.uchPrevUA     = IspRcvBuf.auchFaddr[CLI_POS_UA];
    }
    usDataLen = IspResp.pSavBuff->usDataLen - IspResp.usPrevDataOff;
    usDataMax = PIF_LEN_UDATA - PIF_LEN_IP - usResMsgHLen - sizeof(XGHEADER) - 9;
	/* Leave room for IP address, headers, and additional data (see functions
	 * IspSendResponse and PifNetSendG/D) */
    if (usDataLen > usDataMax) {                /* still exist send data */
        usDataLen = usDataMax;
        IspChangeStatus(ISP_ST_MULTI_SEND);
    } else {                                    /* all data send */
        if (pResMsgH->usFunCode & CLI_FCWCONFIRM) {
            IspChangeStatus(ISP_ST_MULTI_SEND);   /* Next send confirm msg */
        } else {
            IspChangeStatus(ISP_ST_NORMAL);       /* Last data */
        }
        pResMsgH->sResCode = STUB_SUCCESS;
        pResMsgH->usSeqNo |= CLI_SEQ_RECVEND;     /* Set last data indicator */
    }
    pResMsgH->usSeqNo |= CLI_SEQ_RECVDATA;
    memcpy(auchIspTmpBuf, pResMsgH, usResMsgHLen);
                                                /** Send Response **/    
    IspSendResponse(pResMsgH, usResMsgHLen, IspResp.pSavBuff->auchData + IspResp.usPrevDataOff, usDataLen);

    IspResp.usPrevDataOff += usDataLen;       /* Advance next pointer */
}

/*
*===========================================================================
** Synopsis:    SHORT   IspSendNextFrame(CLIREQCOM *pReqMsgH);
*     Input:    pReqMsgH    - Pointer of received request message
*     Output:   nothing
*     InOut:    nothing
*
** Return:      STUB_SUCCESS:       Normal request received
*               STUB_MULTI_SEND:    Multi data send
*               STUB_BUSY:          Multi request received from another terminal
*               STUB_FAITAL:        Another function code detected
*               STUB_FRAME_SEQERR:  Frame sequence error detected    
*
** Description: This function checks to receive during multi. sending state.
*===========================================================================
*/
SHORT   IspSendNextFrame(CLIREQCOM *pReqMsgH)
{
    CLIRESCOM   *pSavMsgH = (CLIRESCOM *)auchIspTmpBuf;
                                            
    if (0 == pReqMsgH->usSeqNo) {               /* another req. received */
        IspChangeStatus(ISP_ST_NORMAL);
        return (STUB_FAITAL + STUB_RETCODE);
    }                                           /* another UA ? */
    if (IspRcvBuf.auchFaddr[CLI_POS_UA] != IspResp.uchPrevUA) {
        IspChangeStatus(ISP_ST_NORMAL);
        return (STUB_FAITAL + STUB_RETCODE);
    }                                           
    /* same function code ? */
    if ((pReqMsgH->usFunCode & CLI_RSTCONTCODE) != 
        (pSavMsgH->usFunCode & CLI_RSTCONTCODE)) {
        IspChangeStatus(ISP_ST_NORMAL);
        return (STUB_FAITAL + STUB_RETCODE);
    }                                           /* sequence # OK ? */
    if ((pReqMsgH->usSeqNo & CLI_SEQ_CONT) != 
        ((pSavMsgH->usSeqNo & CLI_SEQ_CONT) + 1)) {
        IspChangeStatus(ISP_ST_NORMAL);
        return (STUB_FRAME_SEQERR + STUB_RETCODE);
    }                                           /* confirmation ? */
    if (pReqMsgH->usFunCode & CLI_SETCONFIRM) {
        IspChangeStatus(ISP_ST_NORMAL);
        return (STUB_MULTI_SEND);
    }
    ++ pSavMsgH->usSeqNo;
    IspSendMultiple(pSavMsgH, IspResp.usPrevMsgHLen); 

    return (STUB_MULTI_SEND);
}

/*
*===========================================================================
** Synopsis:    USHORT  IspCheckFunCode(USHORT usFunCode);
*     Input:    usFunCode   - Function code of received request
*     Output:   nothing
*     InOut:    nothing
*
** Return:     >0:  Request message length of that function code 
*               0:  Illegal function code
*                  
** Description: This function checks function code.
*===========================================================================
*/
USHORT  IspCheckFunCode(USHORT usFunCode)
{
    USHORT  usReqMsgHLen;

    switch(usFunCode & CLI_RSTCONTCODE) {
    case CLI_FCCASASSIGN :    /* Cashier Assign */
    case CLI_FCCASDELETE :    /* Cashier Delete */
    case CLI_FCCASINDCLOSE :  /* Cashier Ind. Close */
    case CLI_FCCASCLRSCODE :  /* Cashier Clear secret code */
    case CLI_FCCASALLIDREAD : /* Cashier All Id Read */
    case CLI_FCCASINDREAD :   /* Cashier Individual Read */
    case CLI_FCCASINDLOCK :   /* Cashier Individual Lock */
    case CLI_FCCASINDUNLOCK : /* Cashier Individual Un Lock */
    case CLI_FCCASALLLOCK :   /* Cashier All Lock */
    case CLI_FCCASALLUNLOCK : /* Cashier All UnLock */
    case CLI_FCCASINDTMLOCK:  /* Cashier Ind Terminal Lock R3.1 */
    case CLI_FCCASINDTMUNLOCK:/* Cashier Ind Terminal Unlock R3.1 */
    case CLI_FCCASALLTMLOCK:  /* Cashier All Terminal Lock R3.1 */
    case CLI_FCCASALLTMUNLOCK:/* Cashier All Terminal Unlock R3.1 */
    case CLI_FCCASCHKSIGNIN : /* Cashier Check exist sign-in */
    case CLI_FCCASOPENPICLOA: /* Cashier sign-in for pickup loan,   Saratoga */
    case CLI_FCCASCLOSEPICLOA:/* Cashier sign-out for pickup loan,  Saratoga */
	case CLI_FCCASDELAYBALON:
        usReqMsgHLen = sizeof(CLIREQCASHIER);
        break;

    case CLI_FCGCFREADLOCK :  /* GCF Read and Lock */
    case CLI_FCGCFDELETE :    /* GCF Delete */
    case CLI_FCGCFINDREAD :   /* GCF Read ind GCN */
    case CLI_FCGCFREADAGCNO : /* GCF Read all GCN # */
    case CLI_FCGCFALLLOCK :   /* GCF All Lock */
    case CLI_FCGCFALLUNLOCK : /* GCF All Un Lock */
    case CLI_FCGCFASSIGN :    /* GCF Sssign */
    case CLI_FCGCFRESETFG :   /* GCF Reset read flag */
    case CLI_FCGCFCHKEXIST :  /* GCF Check exist */
    case CLI_FCGCFALLIDBW :   /* GCF All ID Read By Waiter */
    case CLI_FCGCFRESETDVQUE :  /* GCF reset delivery queue Add R3.0 */
        usReqMsgHLen = sizeof(CLIREQGCF);
        break;

    case CLI_FCTTLREADTL :   /* TTL Read */
    case CLI_FCTTLRESETTL :  /* TTL Total reset (EOD/RESET) function */
    case CLI_FCTTLRESETCAS : /* TTL Total reset CAS */
    case CLI_FCTTLRESETINDFIN: /* TTL Total reset Ind-Financial */
    case CLI_FCTTLISSUERST : /* Reset Issued flag */
    case CLI_FCTTLISSUECHK : /* TTL Total issue check */
    case CLI_FCTTLCASTTLCHK :                   /* TTL cashier total check  */
    case CLI_FCTTLREADTLPLU :                   /* TTL plural PLU read,  R3.0  */
    case CLI_FCTTLREADTLPLUEX :                   /* TTL plural PLU read, Saratoga  */
    case CLI_FCTTLPLUOPTIMIZE:	/* 06/26/01 */
    case CLI_FCTTLDELETETTL:
	case CLI_FCTTLDELAYBALUPD:
        usReqMsgHLen = sizeof(CLIREQRESETTL);
        break;  
	case CLI_FCTTLCLOSESAVTTL:
        usReqMsgHLen = sizeof(CLIREQTOTAL);
        break;  
	case CLI_FCTTLUPDATE :   /* TTL update, saratoga */
        usReqMsgHLen = sizeof(CLIREQTOTAL);
        break;  

    case CLI_FCOPPLUINDREAD :   /* OP PLU  Individual read */
    case CLI_FCOPDEPTINDREAD :  /* OP DEPT Individual read */
    case CLI_FCOPPLUASSIGN :    /* OP PLU  Assign          */
    case CLI_FCOPDEPTASSIGN :   /* OP DEPT Assign          */
    case CLI_FCOPDEPTDELETE :   /* OP DEPT Delete, saratoga  */
    case CLI_FCOPPLUDELETE :    /* OP PLU  Delete          */
    case CLI_FCOPPLUFILEUPD :   /* OP PLU  File update sub index  */
    case CLI_FCOPPLUALLREAD :   /* OP PLU  All  read              */
    case CLI_FCOPPLUREADBRC :   /* OP PLU  Read by report code    */
    case CLI_FCOPPLUREADBDP :   /* OP PLU  Read by dept number    */
    case CLI_FCOPLOCK :         /* OP Lock    */
    case CLI_FCOPUNLOCK :       /* OP Unlock  */
    case CLI_FCOPDEPTREADBMD :  /* OP Read dept by majort dept #  */
    case CLI_FCOPCPNASSIGN :    /* OP Coupon assign  Add R3.0 */
    case CLI_FCOPCPNINDREAD :   /* OP Coupon individual read Add R3.0 */
    case CLI_FCOPCSTRASSIGN :   /* OP Control String assign Add R3.0 */
    case CLI_FCOPCSTRDELETE :   /* OP Control String delete Add R3.0 */
    case CLI_FCOPCSTRINDREAD :  /* OP Control String individual read Add R3.0 */
    case CLI_FCOPOEPPLUREAD :   /* OP OEP PLU read Add R3.0 */
    case CLI_FCOPPPIASSIGN :    /* OP PPI assign Add R3.1 */
    case CLI_FCOPPPIDELETE :    /* OP PPI delete Add R3.1 */
    case CLI_FCOPPPIINDREAD :   /* OP PPI read Add R3.1 */
    case CLI_FCOPMLDASSIGN :    /* OP Mld Mnemonics assign, V3.3 */
    case CLI_FCOPMLDINDREAD :   /* OP Mld Mnemoncis read, V3.3 */
    case CLI_FCOPRSNASSIGN :    /* OP Reason Code Mnemonics assign, V3.3 */
    case CLI_FCOPRSNINDREAD :   /* OP Reason Code Mnemoncis read, V3.3 */
    case CLI_FCOPPLUUPDATE :    /* OP PLU  assign          */
        usReqMsgHLen = sizeof(CLIREQOPPARA);
        break;  

    case CLI_FCPLUENTERCRITMD:  /* PLU enter crite mode,    Saratoga */
    case CLI_FCPLUEXITCRITMD:   /* PLU exit crite mode,     Saratoga */
        usReqMsgHLen = sizeof(CLIREQPLUECM);
        break;

    case CLI_FCPARAALLREAD :   /* PARA All read  rameter */
    case CLI_FCPARAALLWRITE :  /* PARA All write rameter */
        usReqMsgHLen = sizeof(CLIREQOPPARA);
        break;

    case CLI_FCISPLOGON :        /* ISP Log on */
    case CLI_FCISPCHANGEPASS :   /* ISP Change password */
    case CLI_FCISPLOCKKEYBD :    /* ISP Lock key board */
    case CLI_FCISPUNLOCKKEYBD :  /* ISP Unlock key board */
    case CLI_FCISPLOGOFF :       /* ISP Log off */
    case CLI_FCISPCHKTERM:       /* ISP Check Terminal */
    case CLI_FCISPCHKTOTALS:     /* ISP Check Totals */
    case CLI_FCISPBROADCAST:     /* ISP Parameter Broadcast */
    case CLI_FCISPSATELLITE:     /* ISP Read No. of Satellite */
    case CLI_FCISPSYSCONF :      /* ISP Read System Parameter (V3.3)*/
	case CLI_FCISPCHKMODE:		 /* ISP Check Mode on Terminal */
        usReqMsgHLen = sizeof(CLIREQISP);
        break;

    case CLI_FCISPRESETLOG:      /* ISP Print Reset Log,    V3.3 */
        usReqMsgHLen = sizeof(CLIREQISPLOG);
        break;

    case CLI_FCISPRESETLOG2:     /* ISP Print Reset Log 2,  V3.3 */
        usReqMsgHLen = sizeof(CLIREQISPLOG2);
        break;

    case CLI_FCISPRESETLOG3:     /* ISP Print Reset Log 2,  V3.3 */
        usReqMsgHLen = sizeof(CLIREQISPLOG3);
        break;

	case CLI_FCISPTODREAD :      /* ISP Read  Date/Time (V3.3)*/
    case CLI_FCISPTODWRITE :     /* ISP Write Date/Time (V3.3)*/
        usReqMsgHLen = sizeof(CLIREQISPTOD);
        break;

    /* Remote File System   */
    case CLI_FCRMTCLOSEFILE:                /* Close File */
	case CLI_FCRMTRELOADLAY:
	case CLI_FCRMTRELOADOEP:
        usReqMsgHLen = sizeof(RMTREQCLOSE);
        break;

    case CLI_FCRMTDELETEFILE:               /* Delete File */
        usReqMsgHLen = sizeof(RMTREQDELETE);
        break;

    case CLI_FCRMTOPENFILE:                 /* Open File */
        usReqMsgHLen = sizeof(RMTREQOPEN);
        break;

    case CLI_FCRMTREADFILE:                 /* Read File */
        usReqMsgHLen = sizeof(RMTREQREAD);
        break;

    case CLI_FCRMTSEEKFILE:                 /* Seek File */
        usReqMsgHLen = sizeof(RMTREQSEEK);
        break;

    case CLI_FCRMTWRITEFILE:                /* Write File */
        usReqMsgHLen = sizeof(RMTREQWRITE);
        break;

	case CLI_FCRMTDBGFILENM:
		usReqMsgHLen = sizeof(RMTREQNAME);
		break;

	case CLI_FCRMTSAVFILENM:
		usReqMsgHLen = sizeof(RMTREQSAVNAME);
		break;

    case CLI_FCEJCLEAR:                     /* EJ  clear    */
    case CLI_FCEJREADFILE:                  /* EJ read file */
    /* V3.3 */
    case CLI_FCEJNOOFTRANS:                 /* EJ get # of transaction */
    case CLI_FCEJREADRESET_OVER:            /* EJ read/reset(override type) */
    case CLI_FCEJREADRESET_START:           /* EJ read/reset(not override type) */
    case CLI_FCEJREADRESET:                 /* EJ read/reset */
    case CLI_FCEJREADRESET_END:             /* EJ read/reset(not override type) */
        usReqMsgHLen = sizeof(CLIREQEJ);
        break;

    case CLI_FCETKASSIGN:               /* ETK assign   */
    case CLI_FCETKINDJOBREAD:           /* ETK ind job read  */
    case CLI_FCETKDELETE:               /* ETK delete        */
    case CLI_FCETKCURALLIDRD:           /* ETK all cur id read   */
    case CLI_FCETKSAVALLIDRD:           /* ETK all sav id read   */
    case CLI_FCETKCURINDREAD:           /* ETK cur ind id read   */
    case CLI_FCETKSAVINDREAD:           /* ETK sav ind id read   */
    case CLI_FCETKALLRESET:             /* ETK all reset         */
    case CLI_FCETKALLLOCK:              /* ETK all lock          */
    case CLI_FCETKALLUNLOCK:            /* ETK all unlock        */
    case CLI_FCETKISSUEDCHK:            /* ETK issued check      */
    case CLI_FCETKISSUEDSET:            /* ETK issued set        */
    case CLI_FCETKSTATREAD:             /* ETK status read       */
    case CLI_FCETKCURINDWRITE:          /* ETK cur ind write     */
    case CLI_FCETKCREATTLIDX:           /* ETK create total index  (ISP unique)*/
        usReqMsgHLen = sizeof(CLIREQETK);
        break;

	case CLI_FCOPREQDISPMSG :  /* OP display a message to the operator  */
        usReqMsgHLen = sizeof(CLIREQMSGDISP);
		break;

	case CLI_FCOSYSTEMSTATUS :  /* OP display a message to the operator  */
        usReqMsgHLen = sizeof(CLIREQSYSTEMSTATUS);
		break;

    case CLI_FCCPMRDRSTTLY:             /* CPM  Read/Reset Tally */
        usReqMsgHLen = sizeof(CLIREQCPM_TLY);
        break;

    default:
        usReqMsgHLen = 0;
        PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_INVALID_FUNCODE);  /* Write log */
        PifLog(MODULE_DATA_VALUE(MODULE_ISP_LOG), (usFunCode & CLI_RSTCONTCODE));  /* Write log */
        break;
    }

    return (usReqMsgHLen);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspCheckResponse(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:           OK and as Master
*               ISP_ERR_NOT_MASTER:    I am not As Master
*               ISP_ERR_DUR_BACKUP:    During backup
*
** Description: This function checks function control code of request.
*===========================================================================
*/
SHORT   IspCheckResponse(VOID)
{
    USHORT      fsComStatus;
    
    fsComStatus = IspComReadStatus();   /* Read current status */

    if (0 == ( fsIspCleanUP & ISP_LOCKED_KEYBOARD) ) {  /* if skip not lockkeyboard */
        if ( (fsComStatus & ISP_COMSTS_STOP ) )  {
            PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_ERR_DUR_BACKUP_01);      /* Write Log */
            return (ISP_ERR_DUR_BACKUP);
        }
    }

    if ( fsComStatus & (   ISP_COMSTS_INQUIRY          /* During backup or and so on */
                          | ISP_COMSTS_INQTIMEOUT
                          | ISP_COMSTS_AC42
                          | ISP_COMSTS_AC85       ) )  {

        PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_ERR_DUR_BACKUP_02);      /* Write Log */
        return (ISP_ERR_DUR_BACKUP);
    }

    if (ISP_IAM_MASTER) {   /* if I am Master, then checks next */
        if (0 == (fsComStatus & ISP_COMSTS_M_UPDATE)) {
            return (ISP_ERR_NOT_MASTER);             /* I am Out Of Service */
        }
    } else if (ISP_IAM_BMASTER) {  /* if I am B-Master, then checks next */
        if (0 == (fsComStatus & ISP_COMSTS_BM_UPDATE)) {
            return (ISP_ERR_NOT_MASTER);
        }
        if ((fsComStatus & ISP_COMSTS_M_UPDATE)) {
            return (ISP_ERR_NOT_MASTER);             /* I am not Master */
        }
    } else {
        return (ISP_ERR_NOT_MASTER);             /* I am not Master */
    }
    return (ISP_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    VOID     IspCheckNetRcvData(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      If check is OK! : Keep IspResp.sError 
*               If check is fail: IspResp.sError = ISP_ERR_INVALID_MESSAGE
*                  
** Description: This function checks a received data length.
*===========================================================================
*/
SHORT    IspCheckNetRcvData(VOID)      
{
    if (ISP_SUCCESS == IspResp.sError) {    /* Receive Success ? */
		CLIREQCOM       *pReqM = (CLIREQCOM *)IspRcvBuf.auchData;
		CLIREQDATA      *pBuff;        
		USHORT          usDataLen;

        if ( 0 == ( usDataLen = IspCheckFunCode( pReqM->usFunCode ))) {
            IspResp.sError = ISP_ERR_INVALID_MESSAGE ;   /* Set invalid message */
            PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_CHECKFUNCODE_FAIL);      /* Write Log */
            return IspResp.sError;
        }

        pBuff = (CLIREQDATA *)&IspRcvBuf.auchData[usDataLen];

        usDataLen += PIF_LEN_IP + 4;                /* Add Header length */

        if (pReqM->usSeqNo & CLI_SEQ_SENDDATA) {   /* Check data include or not */
            usDataLen += pBuff->usDataLen + 2;     /* Add data length */
            if ( IspResp.usRcvdLen  != usDataLen )   {
                IspResp.sError = ISP_ERR_INVALID_MESSAGE ;   /* Set invalid message */
                PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_INVALID_DATA_LEN);      /* Write Log */
            }
        } else if ( IspResp.usRcvdLen != usDataLen ) {   /* Without data request */
            IspResp.sError = ISP_ERR_INVALID_MESSAGE ;   /* Set invalid message */
            PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_INVALID_DATA_LEN);      /* Write Log */
        } else {
            pBuff->usDataLen = 0;           /* Clear data length */
        }
    }

	return IspResp.sError;
}

/*
*===========================================================================
** Synopsis:    VOID    IspRecvSatCasWai(USHORT usFun);
*     Input:    usFun 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function executes for cashier function request.
*===========================================================================
*/
VOID    IspRecvSatCasWai(USHORT usFun)
{
    CLIREQCASHIER   *pReqMsgH = (CLIREQCASHIER *)IspRcvBuf.auchData;
    CLIRESCASHIER   ResMsgH = {0};
    CASIF           Casif = {0};
    SHORT           sError;

	//----------------------------------------------------------------------------------
	// Asserts for invariant conditions needed for successful use of this function

		NHPOS_ASSERT(sizeof(IspRcvBuf.auchData) >= sizeof(CLIREQCASHIER));
		NHPOS_ASSERT(sizeof(ResMsgH) >= sizeof(CLIRESCASHIER));
		NHPOS_ASSERT(sizeof(Casif) >= sizeof(CASIF));

        // test for IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESCASHIER)];
		NHPOS_ASSERT(sizeof(auchIspTmpBuf) >= sizeof(CLIREQDATA)+sizeof(CLIRESCASHIER)+2);
		NHPOS_ASSERT(sizeof(auchIspTmpBuf) >= sizeof(CLIRESCASHIER)+2*CAS_NUMBER_OF_MAX_CASHIER+2);


		// Asserts for array bounds are large enough
		NHPOS_ASSERT_ARRAYSIZE(ResMsgH.auchCasName, CLI_CASHIERNAME);
		NHPOS_ASSERT_ARRAYSIZE(pReqMsgH->auchCasName, CLI_CASHIERNAME);
		NHPOS_ASSERT_ARRAYSIZE(Casif.auchCashierName, CLI_CASHIERNAME);

		NHPOS_ASSERT_ARRAYSIZE(pReqMsgH->fbStatus, CLI_CASHIERSTATUS);
		NHPOS_ASSERT_ARRAYSIZE(ResMsgH.fbStatus, CLI_CASHIERSTATUS);
		NHPOS_ASSERT_ARRAYSIZE(Casif.fbCashierStatus, CLI_CASHIERSTATUS);

		// Asserts for array data types are the same for memcpy, memset, _tcsncpy, _tcsnset, etc
		NHPOS_ASSERT(sizeof(ResMsgH.auchCasName[0]) == sizeof(pReqMsgH->auchCasName[0]));
		NHPOS_ASSERT(sizeof(Casif.fbCashierStatus[CAS_CASHIERSTATUS_0]) == sizeof(pReqMsgH->fbStatus[0]));
	//-----------------------------------------------------------------------------------

    ResMsgH.usFunCode     = pReqMsgH->usFunCode;
    ResMsgH.sResCode      = STUB_SUCCESS;
    ResMsgH.sReturn       = ISP_SUCCESS;
    ResMsgH.usSeqNo       = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.ulCashierNo   = pReqMsgH->ulCashierNo;
    ResMsgH.ulSecretNo    = pReqMsgH->ulSecretNo;

    _tcsncpy(ResMsgH.auchCasName, pReqMsgH->auchCasName, CLI_CASHIERNAME);
    memcpy(ResMsgH.fbStatus, pReqMsgH->fbStatus, CLI_CASHIERSTATUS);

    ResMsgH.usStartGCN    = pReqMsgH->usStartGCN;
    ResMsgH.usEndGCN      = pReqMsgH->usEndGCN;
    ResMsgH.uchChgTipRate = pReqMsgH->uchChgTipRate;
    ResMsgH.uchTeamNo     = pReqMsgH->uchTeamNo;
    ResMsgH.uchTerminal   = pReqMsgH->uchTerminal;
	ResMsgH.usSupervisorID		= pReqMsgH->usSupervisorID;	//new in version 2.2.0
	ResMsgH.usCtrlStrKickoff	= pReqMsgH->usCtrlStrKickoff;	//new in version 2.2.0
	ResMsgH.usStartupWindow		= pReqMsgH->usStartupWindow;	//new in version 2.2.0
	ResMsgH.ulGroupAssociations = pReqMsgH->ulGroupAssociations;

    Casif.ulCashierNo      = pReqMsgH->ulCashierNo;
    Casif.ulCashierSecret  = pReqMsgH->ulSecretNo;
    Casif.usUniqueAddress  = (USHORT)IspRcvBuf.auchFaddr[CLI_POS_UA];

    _tcsncpy(Casif.auchCashierName, pReqMsgH->auchCasName, CLI_CASHIERNAME);
    memcpy(Casif.fbCashierStatus, pReqMsgH->fbStatus, CLI_CASHIERSTATUS);

    Casif.usGstCheckStartNo = pReqMsgH->usStartGCN;
    Casif.usGstCheckEndNo   = pReqMsgH->usEndGCN;
    Casif.uchChgTipRate     = pReqMsgH->uchChgTipRate;
    Casif.uchTeamNo         = pReqMsgH->uchTeamNo;
    Casif.uchTerminal       = pReqMsgH->uchTerminal;
	Casif.usSupervisorID	= pReqMsgH->usSupervisorID;		//new in version 2.2.0
	Casif.usCtrlStrKickoff	= pReqMsgH->usCtrlStrKickoff;	//new in version 2.2.0
	Casif.usStartupWindow	= pReqMsgH->usStartupWindow;	//new in version 2.2.0
	Casif.ulGroupAssociations	= pReqMsgH->ulGroupAssociations;	//new in version 2.2.0
    sError = ISP_ERR_NOT_MASTER;

    switch(usFun) {

    case CLI_FCCASALLLOCK :   /* Cashier All Lock */
    case CLI_FCCASALLUNLOCK : /* Cashier All UnLock */
        sError = CAS_FILE_NOT_FOUND;
        break;

	case CLI_FCCASDELAYBALON:
		ResMsgH.sReturn = SerCasDelayedBalance(Casif.uchTerminal, MAINT_DBON);

		if(ResMsgH.sReturn == ISP_SUCCESS)
		{
			/*The following PM_CNTRL was not being used, so we use this one
			to display the D-B message on the display*/
			flDispRegDescrControl |= PM_CNTRL;
			flDispRegKeepControl |= PM_CNTRL;

			uchMaintDelayBalanceFlag |= MAINT_DBON;

			//We set the class so that we can send the dummy data,
			//and trigger the discriptor to show D-B to inform the user that
			//the terminal is in delay balance mode
			RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_0); 	//SR 236 Dummy Data for calling the descriptor

			PifSaveFarData();
			PifLog(MODULE_ISP_LOG, LOG_EVENT_DB_FLAG_ON_2);

			EJWriteDelayBalance(uchMaintDelayBalanceFlag);

			if (STUB_MULTI_SEND == ResMsgH.sResCode) {
				IspSendMultiple((CLIRESCOM *)&ResMsgH, sizeof(CLIRESCASHIER));
			} else {
				IspSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESCASHIER), NULL, 0);
			}
		}
		break;

    default:                 /* not used */
		PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_RECVCAS_INV_FUN);
        break;
    }
    IspSendPassResponse(sError);    /* Send sError response */
}

/*===== END OF SOURCE =====*/