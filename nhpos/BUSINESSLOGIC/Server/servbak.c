/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : SERVER module, Back Up Copy Functions Source File
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVBAK.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*   
*           SerRecvBak();    C   Backup function handling
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* Jun-04-93:H.Yamaguchi: Modified same as HP Int'l
* Mar-31-94:K.You      : add flex GC file feature.(mod. SerRecvBak)
* Nov-30-95:T.Nakahata : R3.1 Initial (add Programmable Reprot)
* Aug-11-99:M.Teraki   : R3.5 remove WAITER_MODEL
* Feb-14-00:hrkato     : Saratoga
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
#include    <stdio.h>

#include    "ecr.h"
#include    "pif.h"
#include    "cscas.h"
#include    "cswai.h"
#include    "csop.h"
#include    "csgcs.h"
#include    "csttl.h"
#include    "plu.h"
#include    "csstbfcc.h"
#include    "csserm.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbbak.h"
#include    "csprgrpt.h"
#include    "servmin.h"

/*
*===========================================================================
** Synopsis:    VOID    SerRecvBak(VOID);
*
** Return:      none.
*
** Description: This function executes backup request
*===========================================================================
*/
VOID    SerRecvBak(VOID)
{
    CLIREQBACKUP    *pReqMsgH;
    CLIRESBACKUP    ResMsgH;
    CLIREQDATA      *pSavBuff, *pRcvBuff;
    USHORT          usDataLen;
	USHORT			usMaxTmpBuff;
    DATE_TIME       NewPifDate;
    CLIINQDATE      NewInqDate;
    SERINQSTATUS    InqData;
    CLIREQDATABAK   *pReqData;
	SHORT           sSerSendStatus;

    pReqMsgH = (CLIREQBACKUP *)SerRcvBuf.auchData;
    memset(&ResMsgH, 0, sizeof(CLIRESBACKUP));
    ResMsgH.usFunCode = pReqMsgH->usFunCode;
    ResMsgH.usSeqNo   = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.sResCode  = STUB_MULTI_SEND;

    usDataLen = SER_MAX_TMPBUF - sizeof(CLIRESBACKUP) - 2;
    pSavBuff = (CLIREQDATA *)&SerRcvBuf.auchData[sizeof(CLIREQBACKUP)];
    pRcvBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESBACKUP)];

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {

    case    CLI_FCBAKCASHIER:
        ResMsgH.sReturn = CasRcvFile(pSavBuff->auchData, pSavBuff->usDataLen, pRcvBuff->auchData, &usDataLen);
        if (0 > ResMsgH.sReturn) {
            usDataLen = 0;
        } 
        break;

    case    CLI_FCBAKGCF:
        SstReadFileFH(0, &usMaxTmpBuff, sizeof(usMaxTmpBuff), hsSerTmpFile);
        usDataLen = usMaxTmpBuff - sizeof(CLIRESBACKUP) - 2; 
        ResMsgH.sReturn = GusResBackUpFH(pSavBuff->auchData, pSavBuff->usDataLen, hsSerTmpFile, SERTMPFILE_DATASTART + sizeof(CLIRESCOM), &usDataLen);
        if (0 > ResMsgH.sReturn) {
            usDataLen = 0;
        } 
        break;

    case    CLI_FCBAKTOTAL:
        ResMsgH.sReturn = TtlRespBackup(pSavBuff->auchData, pSavBuff->usDataLen, pRcvBuff->auchData, &usDataLen);
        if (0 > ResMsgH.sReturn) {
            usDataLen = 0;
        } 
        break;

    case    CLI_FCBAKOPPARA:
        ResMsgH.sReturn = OpResBackUp(pSavBuff->auchData, pSavBuff->usDataLen, pRcvBuff->auchData, &usDataLen, 0);
        if (0 > ResMsgH.sReturn) {
            usDataLen = 0;
        } 
        break;

    case    CLI_FCBAKPLU:                                       /* Saratoga */
        ResMsgH.sReturn = CliResBackupMMM(pSavBuff->auchData, pSavBuff->usDataLen, pRcvBuff->auchData, &usDataLen);
        if (0 != ResMsgH.sReturn) {
            usDataLen = 0;
        }
        break;

    case    CLI_FCBAKETK:
        ResMsgH.sReturn = CliResBackupETK(pSavBuff->auchData, pSavBuff->usDataLen, pRcvBuff->auchData, &usDataLen);
        if (0 != ResMsgH.sReturn) {
            usDataLen = 0;
        } 

        break;
    /* === Add New Functions - Programmable Report (Release 3.1) BEGIN === */
    case    CLI_FCBAKPROGRPT:
        ResMsgH.sReturn = ProgRptRcvFile( pSavBuff->auchData, pSavBuff->usDataLen, pRcvBuff->auchData, &usDataLen);
        if (0 > ResMsgH.sReturn) {
            usDataLen = 0;
        } 
        break;
    /* === Add New Functions - Programmable Report (Release 3.1) END === */
    case    CLI_FCBAKFINAL:
        pReqData = (CLIREQDATABAK *)pRcvBuff->auchData;
        SstReadInqStat(&InqData);
        memcpy(pReqData->ausTransNo, InqData.ausTranNo, 2*CLI_ALLTRANSNO);
        memcpy(pReqData->ausPreTranNo, InqData.ausPreTranNo, 2*CLI_ALLTRANSNO);
        pReqData->fsBcasInf = InqData.fsBcasInf;
        ResMsgH.sReturn  = STUB_SUCCESS;
        ResMsgH.sResCode = STUB_SUCCESS;
        ResMsgH.usSeqNo |= (CLI_SEQ_RECVDATA + CLI_SEQ_RECVEND);
        sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESBACKUP), (UCHAR *)pRcvBuff->auchData, sizeof(CLIREQDATABAK));
		if (sSerSendStatus < 0) {
			char xBuff [128];
			sprintf (xBuff, "usFunCode = 0x%x, usSeqNo = 0x%x, sSerSendStatus = %d", pReqMsgH->usFunCode, pReqMsgH->usSeqNo, sSerSendStatus);
			NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
		}

        if (SER_IAM_MASTER) {
            PifSleep(SER_SLEEP_TIMER);      /* delay */
            PifGetDateTime(&NewPifDate);
            SerConvertInqDate(&NewPifDate, &NewInqDate);
            SstChangeInqStat(SER_INQ_M_UPDATE | SER_INQ_BM_UPDATE);
            SstChangeInqDate(&NewInqDate);
            SerSendInqDate();
        }
        return;
        break;

    default:                                    /* not used */
        return;
        break;
    }
    SerResp.pSavBuff = pRcvBuff;
    SerResp.pSavBuff->usDataLen = usDataLen;
    SerSendBakMultiple((CLIRESCOM *)&ResMsgH, sizeof(CLIRESBACKUP));
}

/*============= End of servbak.c ================*/