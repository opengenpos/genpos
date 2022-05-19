/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : ISP Server Module, Guest Check File Functions Source File
* Category    : ISP Server Module, US Hospitality Model
* Program Name: ISPGCF.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*           IspRecvGCF();         Entry point of Guest check function handling
*
*           IspCheckGCFUnLock();  Guest check file unlock function
*
* --------------------------------------------------------------------------
* Update Histories                                                         
* Date     :Ver.Rev.:   Name    : Description
* Jun-26-92:00.00.01:H.Yamaguchi: initial                                   
* Apr-24-95:03.00.00:hkato      : R3.0(GusReadLock)
* Jun-27-95:03.00.00:M.suzuki   : R3.0
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
#include	<stdio.h>
#include	<math.h>

#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <csgcs.h>
#include    <csstbgcf.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <storage.h>
#include    <isp.h>
#include    "ispcom.h"
#include    <appllog.h>

/*
*===========================================================================
** Synopsis:    VOID    IspRecvGCF(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function executes for GCF function request.
*===========================================================================
*/
VOID    IspRecvGCF(VOID)
{
    CLIREQGCF       *pReqMsgH;
    CLIRESGCF       ResMsgH;

    pReqMsgH = (CLIREQGCF *)IspRcvBuf.auchData;
    memset(&ResMsgH, 0, sizeof(CLIRESGCF));

    ResMsgH.usFunCode = pReqMsgH->usFunCode;
    ResMsgH.sResCode  = STUB_SUCCESS;
    ResMsgH.sReturn   = ISP_SUCCESS;
    ResMsgH.usSeqNo   = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.usGCN     = pReqMsgH->usGCN;
    ResMsgH.usGCType  = pReqMsgH->usGCType;

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {

    case    CLI_FCGCFREADLOCK:   /* Read and Lock */
        IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESGCF)];
        IspResp.pSavBuff->usDataLen = ISP_MAX_TMPSIZE - sizeof(CLIRESGCF) - 2;

        ResMsgH.sReturn = SerGusReadLock(pReqMsgH->usGCN, IspResp.pSavBuff->auchData, IspResp.pSavBuff->usDataLen, GCF_REORDER);
        if (0 <= ResMsgH.sReturn) {
            IspResp.pSavBuff->usDataLen = ResMsgH.sReturn;
            usIspLockedGCFNO = pReqMsgH->usGCN;      /* save GCF */
            fsIspLockedFC   |= ISP_LOCK_INDGCF;      /* set INDGCF falg */
            ResMsgH.sResCode = STUB_MULTI_SEND;
        } else {
            IspResp.pSavBuff->usDataLen = 0;    /* if error, then datalen = 0 */
        }
        break;

    case    CLI_FCGCFLOCK:   /* Lock a GCF */
        ResMsgH.sReturn = SerGusReadLock(pReqMsgH->usGCN, auchIspTmpBuf, 0, GCF_REORDER);  /* R3.0 */
        if (0 <= ResMsgH.sReturn) {
            usIspLockedGCFNO = pReqMsgH->usGCN;     /* Save GCF   */
            fsIspLockedFC   |= ISP_LOCK_INDGCF;     /* Set INDGCF */
        }
        break;

    case    CLI_FCGCFDELETE:  /* Delete a GCF */
        ResMsgH.sReturn = SerGusDelete(pReqMsgH->usGCN);
        if (0 <= ResMsgH.sReturn) {
            IspCheckGCFNo(pReqMsgH->usGCN);
        }

        /* pc if reset log, V3.3 */
        IspResetLog.uchAction = 3;
        IspResetLog.uchDayPtd = 0;
        IspResetLog.uchCurSave = 0;
        IspResetLog.uchReset = ISP_RESET_RESET;
        IspResetLog.ulNumber = (ULONG)pReqMsgH->usGCN;
        IspResetLog.uchError = (UCHAR)(GusConvertError(ResMsgH.sReturn));
        IspWriteResetLog();
        break;

    case    CLI_FCGCFASSIGN:  /* Assign new start GCN */
        if ( GCF_SUCCESS == (ResMsgH.sReturn = SerGusCheckExist())) {
            ResMsgH.sReturn = SerGusAssignNo(pReqMsgH->usGCN);
        }
        break;

    case    CLI_FCGCFRESETFG:  /* Reset read flag */
        ResMsgH.sReturn = SerGusResetReadFlag(pReqMsgH->usGCN);
        if (0 <= ResMsgH.sReturn) {
            IspCheckGCFNo(pReqMsgH->usGCN);
        }
        break;

    case CLI_FCGCFINDREAD :	  /* GCF read ind GCN */
        IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESGCF)];
        IspResp.pSavBuff->usDataLen = ISP_MAX_TMPSIZE - sizeof(CLIRESGCF) - 2;

        ResMsgH.sReturn = SerGusIndRead(pReqMsgH->usGCN, IspResp.pSavBuff->auchData, IspResp.pSavBuff->usDataLen);
        
        if (0 <= ResMsgH.sReturn) {
            IspResp.pSavBuff->usDataLen = ResMsgH.sReturn;
            ResMsgH.sResCode = STUB_MULTI_SEND;
        } else {
            IspResp.pSavBuff->usDataLen = 0;
        }

        /* pc if reset log, V3.3 */
        IspResetLog.uchAction = 3;
        IspResetLog.uchDayPtd = 0;
        IspResetLog.uchCurSave = 0;
        IspResetLog.uchReset = ISP_RESET_READ;
        IspResetLog.ulNumber = (ULONG)pReqMsgH->usGCN;
		IspResetLog.uchError = 0;
		if (ResMsgH.sReturn < 0) {
			IspResetLog.uchError = (UCHAR)(GusConvertError(ResMsgH.sReturn));
		}
        IspWriteResetLog();
        break;

    case CLI_FCGCFREADAGCNO	: /* GCF read all GCN # */
        IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESGCF)];
        IspResp.pSavBuff->usDataLen = ISP_MAX_TMPSIZE - sizeof(CLIRESGCF) - 2;
    
        ResMsgH.sReturn = SerGusAllIdRead(pReqMsgH->usGCType, (USHORT *)IspResp.pSavBuff->auchData, IspResp.pSavBuff->usDataLen);
        
        if (0 <= ResMsgH.sReturn) {
            IspResp.pSavBuff->usDataLen = ResMsgH.sReturn;
            ResMsgH.sResCode = STUB_MULTI_SEND;
        } else {
            IspResp.pSavBuff->usDataLen = 0;
        }
        break;

    case CLI_FCGCFALLLOCK :   /* GCF All Lock */
        if ( 0 == ( fsIspLockedFC & ISP_LOCK_ALLGCF) ) {
            ResMsgH.sReturn = SerGusAllLock();
            if (GCF_SUCCESS == ResMsgH.sReturn) {
                fsIspLockedFC |= ISP_LOCK_ALLGCF;   /* Set ALLGCF flag */
            }
        } else {
            ResMsgH.sReturn = GCF_DUR_LOCKED; /* if already locked, then error */
        }
        break;

    case CLI_FCGCFALLUNLOCK : /* GCF All Un Lock */
        if ( fsIspLockedFC & ISP_LOCK_ALLGCF ) {
            ResMsgH.sReturn = IspCleanUpLockGCF(ISP_LOCK_ALLGCF);
        }
        break;

    case CLI_FCGCFCHKEXIST :  /* GCF Check exist */
        ResMsgH.sReturn = SerGusCheckExist();
        break;

    case CLI_FCGCFALLIDBW :   /* GCF All ID Read By Waiter */
        IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESGCF)];
        IspResp.pSavBuff->usDataLen = ISP_MAX_TMPSIZE - sizeof(CLIRESGCF) - 2;

        ResMsgH.sReturn = SerGusAllIdReadBW(pReqMsgH->usGCType, pReqMsgH->ulWaiterNo, (USHORT *)IspResp.pSavBuff->auchData, IspResp.pSavBuff->usDataLen);
        if (0 <= ResMsgH.sReturn) {
            IspResp.pSavBuff->usDataLen = ResMsgH.sReturn;
            ResMsgH.sResCode = STUB_MULTI_SEND;
        } else {
            IspResp.pSavBuff->usDataLen = 0;
        }
        break;

    /************************* Add R3.0 *************************/
    case    CLI_FCGCFRESETDVQUE:  /* Reset delivery queue */

        ResMsgH.sReturn = SerGusResetDeliveryQueue();
        break;
    /************************* End Add R3.0 ***********************/

    default:               /* not used */
		PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_RCVGCF_INV_FUN);
        return;
        break;
    }

	if (ResMsgH.sReturn < 0) {
		char  xBuff[128];

		sprintf (xBuff, "==NOTE: IspRecvGCF() usFuncCode 0x%x, sReturn %d, sResCode %d",
			(pReqMsgH->usFunCode & CLI_RSTCONTCODE), ResMsgH.sReturn, ResMsgH.sResCode);
		NHPOS_NONASSERT_TEXT(xBuff);

		PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_GC_RQST_FAIL);
		PifLog(MODULE_DATA_VALUE(MODULE_ISP_LOG), (USHORT)(pReqMsgH->usFunCode & CLI_RSTCONTCODE));
		PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(ResMsgH.sReturn));
	}

    if (STUB_MULTI_SEND == ResMsgH.sResCode) {
        IspSendMultiple((CLIRESCOM *)&ResMsgH, sizeof(CLIRESGCF));
    } else {
        IspSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESGCF), NULL, 0);
    }
}

/*
*===========================================================================
** Synopsis:    VOID    IspCheckGCFUnLock(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function checks GCF should be unlock or not.
*===========================================================================
*/
VOID    IspCheckGCFUnLock(VOID)
{
    CLIRESGCF   *pResMsgH;

    pResMsgH = (CLIRESGCF *)auchIspTmpBuf;

    if ( CLI_FCGCFREADLOCK == (pResMsgH->usFunCode & CLI_RSTCONTCODE )) {
        IspCleanUpLockGCF(ISP_LOCK_INDGCF);  /* Unlock a GCF */
    }
}

/*
*===========================================================================
** Synopsis:    VOID    IspCheckGCFNo(USHORT usGCFNo);
*     Input:    usGCFNo 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function checks GCF No.
*===========================================================================
*/
VOID    IspCheckGCFNo(USHORT usGCFNo)
{
    if ( fsIspLockedFC & ISP_LOCK_INDGCF) {
        if ( usIspLockedGCFNO == usGCFNo ) { ;   /* Check a locked GCF  */
            IspCleanUpLockGCF(ISP_LOCK_INDGCF);  /* Unlock a GCF */
        }
    }
}


/*===== END OF SOURCE =====*/

