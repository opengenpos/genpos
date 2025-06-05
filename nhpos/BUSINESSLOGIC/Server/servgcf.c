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
* Title       : SERVER module, Guest Check File Functions Source File
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVGCF.C                                            
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*       2020  -> OpenGenPOS Rel 2.4.0 (Windows 10, Datacap removed) Open source
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2019 with Rel 2.4.0
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:
*
*           SerRecvGCF();       Guest check function handling
*           SerChekGCFUnLock(); Guest check file unlock function
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jun-04-92:H.Nakashima: initial                                   
* Apr-07-94:K.You      : add flex GC file feature(mod. SerRecvGCF)
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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

#include    <ecr.h>
#include    <paraequ.h>
#include    <appllog.h>
#include    <pif.h>
#include    <plu.h>
#include    <csgcs.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    "servmin.h"
/*
*===========================================================================
** Synopsis:    VOID    SerRecvGCF(VOID);
*
** Return:      none.
*
** Description: This function executes for GCF function request.
				This function is called when the Server thread receives a
				message for doing a Guest Check File action.
*===========================================================================
*/
VOID    SerRecvGCF(VOID)
{
    TrnFileSize     usMaxTmpBuff;
    CLIREQGCF       * const pReqMsgH = (CLIREQGCF *)SerRcvBuf.auchData;  // pointer to the GCF request part of the received message
    CLIRESGCF       ResMsgH = { 0 };    // struct for the Server's response to the GCF request
	ULONG			ulActualBytesRead;//RPH SR# 201
	SHORT           sSerSendStatus;

	// Fill in the header of the Server's response message using the header info
	// from the Client's request.
    ResMsgH.usFunCode = pReqMsgH->usFunCode;
    ResMsgH.sResCode  = STUB_SUCCESS;
    ResMsgH.usSeqNo   = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.usGCN     = pReqMsgH->usGCN;
    ResMsgH.usGCType  = pReqMsgH->usGCType;
	ResMsgH.usGCorderNo[0] = 0;
	ResMsgH.usGCorderNo[1] = 0;

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {

    case    CLI_FCGCFMASSIGN:
        ResMsgH.sReturn = GusManAssignNo(pReqMsgH->usGCN);
        break;

    case    CLI_FCGCFSTORE:
        ResMsgH.sReturn = GusStoreFileFH(pReqMsgH->usGCType, pReqMsgH->usGCN, hsSerTmpFile, SERTMPFILE_DATASTART + sizeof(CLIREQGCF), SerResp.pSavBuff->usDataLen);
        break;

    case    CLI_FCGCFREADLOCK:
        SerResp.pSavBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESGCF)]; 
        SerResp.ulSavBuffSize = sizeof(auchSerTmpBuf) - sizeof(CLIRESGCF);
        SstReadFileFH(0, &usMaxTmpBuff, sizeof(usMaxTmpBuff), hsSerTmpFile);
        SerResp.pSavBuff->usDataLen = usMaxTmpBuff - (SERTMPFILE_DATASTART + sizeof(CLIRESGCF));
        ResMsgH.sReturn = GusReadLockFH(pReqMsgH->usGCN, hsSerTmpFile, SERTMPFILE_DATASTART + sizeof(CLIRESGCF), SerResp.pSavBuff->usDataLen, pReqMsgH->usGCType, &ulActualBytesRead );
		
        if (0 <= ResMsgH.sReturn) {
            SerResp.pSavBuff->usDataLen = ulActualBytesRead;
        } else {
            SerResp.pSavBuff->usDataLen = 0;
        }
        ResMsgH.sResCode = STUB_MULTI_SEND;
        break;

    case    CLI_FCGCFLOCK:
        ResMsgH.sReturn = GusReadLock(pReqMsgH->usGCN, auchSerTmpBuf, 0, pReqMsgH->usGCType );
        break;

    case    CLI_FCGCRETURNSCLEAR:
        ResMsgH.sReturn = GusReturnsLockClear(pReqMsgH->usGCN);
        break;

    case    CLI_FCGCFDELETE:
        ResMsgH.sReturn = GusDelete(pReqMsgH->usGCN);
        if (GCF_NOT_IN == ResMsgH.sReturn) {
            ResMsgH.sReturn = GCF_SUCCESS;
        }
        break;

    case    CLI_FCGCFAGENGCN:
        ResMsgH.sReturn = GusGetAutoNo(&ResMsgH.usGCN);
        break;

    case    CLI_FCGCFABAKGCN:
        ResMsgH.sReturn = GusBakAutoNo(ResMsgH.usGCN);
        break;

    case    CLI_FCGCFREADAGCNO:
        SerResp.pSavBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESGCF)];
        SerResp.ulSavBuffSize = sizeof(auchSerTmpBuf) - sizeof(CLIRESGCF);
        SstReadFileFH(0, &usMaxTmpBuff, sizeof(usMaxTmpBuff), hsSerTmpFile);/* ### ADD (2171 for Win32) V1.0 */
        SerResp.pSavBuff->usDataLen = usMaxTmpBuff - (SERTMPFILE_DATASTART + sizeof(CLIRESGCF));
        ResMsgH.sReturn = GusAllIdRead(pReqMsgH->usGCType, (USHORT *)SerResp.pSavBuff->auchData, SerResp.pSavBuff->usDataLen);
        if (0 <= ResMsgH.sReturn) {
            SerResp.pSavBuff->usDataLen = ResMsgH.sReturn;
        } else {
            SerResp.pSavBuff->usDataLen = 0;
        }
        ResMsgH.sResCode = STUB_MULTI_SEND;
        break;
    case    CLI_FCGCFASSIGN:
        ResMsgH.sReturn = GusAssignNo(pReqMsgH->usGCN);
        break;

    case    CLI_FCGCFRESETFG:
        ResMsgH.sReturn = GusResetReadFlag(pReqMsgH->usGCN, pReqMsgH->usGCType);
        break;

    case    CLI_FCGCFINDREAD:
        SerResp.pSavBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESGCF)]; 
        SerResp.ulSavBuffSize = sizeof(auchSerTmpBuf) - sizeof(CLIRESGCF);
        SstReadFileFH(0, &usMaxTmpBuff, sizeof(usMaxTmpBuff), hsSerTmpFile);
        SerResp.pSavBuff->usDataLen = usMaxTmpBuff - (SERTMPFILE_DATASTART + sizeof(CLIRESGCF));
        
		//RPH SR# 201
		ResMsgH.sReturn = GusIndReadFH(pReqMsgH->usGCN, hsSerTmpFile, SERTMPFILE_DATASTART + sizeof(CLIRESGCF), SerResp.pSavBuff->usDataLen, &ulActualBytesRead);
		
        if (0 <= ResMsgH.sReturn) {
            SerResp.pSavBuff->usDataLen = ResMsgH.sReturn;
        } else {
            SerResp.pSavBuff->usDataLen = 0;
        }
        ResMsgH.sResCode = STUB_MULTI_SEND;
        break;

    /*********************** Add R3.0 *************************/

    case    CLI_FCGCFINFOQUE:
        ResMsgH.sReturn = GusInformQueue(pReqMsgH->usQueue, pReqMsgH->usGCType, pReqMsgH->usGCN, (UCHAR)pReqMsgH->usForBack);
        break;

    case    CLI_FCGCRTRVFIRST2:
        ResMsgH.sReturn = GusRetrieveFirstQueue(pReqMsgH->usQueue, pReqMsgH->usGCType, ResMsgH.usGCorderNo);
        break;

    case    CLI_FCGCFDELDLIVQUE:
        ResMsgH.sReturn = GusDeleteDeliveryQueue(pReqMsgH->usGCN, pReqMsgH->usGCType);
        break;

    case    CLI_FCGCFINDREADRAM:
        SerResp.pSavBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESGCF)];
        SerResp.ulSavBuffSize = sizeof(auchSerTmpBuf) - sizeof(CLIRESGCF);
        SerResp.pSavBuff->usDataLen = SERISP_MAX_LEN(sizeof(CLIRESGCF));
        ResMsgH.sReturn = GusIndRead(pReqMsgH->usGCN, SerResp.pSavBuff->auchData, SerResp.pSavBuff->usDataLen);
        if (0 <= ResMsgH.sReturn) {
            SerResp.pSavBuff->usDataLen = ResMsgH.sReturn;
        } else {
            SerResp.pSavBuff->usDataLen = 0;
        }
        ResMsgH.sResCode = STUB_MULTI_SEND;
        break;

    case    CLI_FCGCFSEARCHQUE:
        ResMsgH.sReturn = GusSearchQueue(pReqMsgH->usQueue, pReqMsgH->usGCN, pReqMsgH->usGCType);
        break;

    case    CLI_FCGCFCHECKREAD:
        SerResp.pSavBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESGCF)]; 
        SerResp.ulSavBuffSize = sizeof(auchSerTmpBuf) - sizeof(CLIRESGCF);
        SstReadFileFH(0, &usMaxTmpBuff, sizeof(usMaxTmpBuff), hsSerTmpFile);
        SerResp.pSavBuff->usDataLen = usMaxTmpBuff - (SERTMPFILE_DATASTART + sizeof(CLIRESGCF));
        
		//RPH SR# 201
		ResMsgH.sReturn = GusCheckAndReadFH(pReqMsgH->usGCN, pReqMsgH->usGCType, hsSerTmpFile, SERTMPFILE_DATASTART + sizeof(CLIRESGCF), SerResp.pSavBuff->usDataLen, &ulActualBytesRead);
		//RPH SR#201
        if (0 <= ResMsgH.sReturn) {
            SerResp.pSavBuff->usDataLen = ResMsgH.sReturn;
        } else {
            SerResp.pSavBuff->usDataLen = 0;
        }
        ResMsgH.sResCode = STUB_MULTI_SEND;
        break;

    case    CLI_FCGCFRESETDVQUE:
        ResMsgH.sReturn = GusResetDeliveryQueue();
        break;

    case    CLI_FCGCFDVINDREAD:
        SerResp.pSavBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESGCF)];
        SerResp.ulSavBuffSize = sizeof(auchSerTmpBuf) - sizeof(CLIRESGCF);
        SerResp.pSavBuff->usDataLen = SERISP_MAX_LEN(sizeof(CLIRESGCF));
        ResMsgH.sReturn = GusDeliveryIndRead(pReqMsgH->usGCType, SerResp.pSavBuff->auchData, SerResp.pSavBuff->usDataLen, &ResMsgH.usGCN);
        if (0 <= ResMsgH.sReturn) {
            SerResp.pSavBuff->usDataLen = ResMsgH.sReturn;
        } else {
            SerResp.pSavBuff->usDataLen = 0;
        }
        ResMsgH.sResCode = STUB_MULTI_SEND;
        break;

    case    CLI_FCGCFDVINDREADBAK:
        ResMsgH.sReturn = GusDeliveryIndRead(pReqMsgH->usGCType, auchSerTmpBuf, 0, &ResMsgH.usGCN);
        if (ResMsgH.sReturn == GCF_CHANGE_QUEUE) {
            ResMsgH.sReturn = GCF_SUCCESS;
        }
        break;

    /*********************** Add R3.0 *************************/
    case    CLI_FCGCFALLIDBW:           /* V3.3 */
        SerResp.pSavBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESGCF)];
        SerResp.ulSavBuffSize = sizeof(auchSerTmpBuf) - sizeof(CLIRESGCF);
        SstReadFileFH(0, &usMaxTmpBuff, sizeof(usMaxTmpBuff), hsSerTmpFile);/* ### ADD (2171 for Win32) V1.0 */
        SerResp.pSavBuff->usDataLen = usMaxTmpBuff - (SERTMPFILE_DATASTART + sizeof(CLIRESGCF));
        ResMsgH.sReturn = GusAllIdReadBW(pReqMsgH->usGCType, pReqMsgH->ulWaiterNo, (USHORT *)SerResp.pSavBuff->auchData, SerResp.pSavBuff->usDataLen);
        if (0 <= ResMsgH.sReturn) {
            SerResp.pSavBuff->usDataLen = ResMsgH.sReturn;
        } else {
            SerResp.pSavBuff->usDataLen = 0;
        }
        ResMsgH.sResCode = STUB_MULTI_SEND;
        break;

    case    CLI_FCGCREADFILTERED:           /* V3.3 */
		{
			CLIREQGCFFILTERED * const pReqMsgH = (CLIREQGCFFILTERED *)SerRcvBuf.auchData;

			SerResp.pSavBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESGCF)];
            SerResp.ulSavBuffSize = sizeof(auchSerTmpBuf) - sizeof(CLIRESGCF);
            SstReadFileFH(0, &usMaxTmpBuff, sizeof(usMaxTmpBuff), hsSerTmpFile);/* ### ADD (2171 for Win32) V1.0 */
			SerResp.pSavBuff->usDataLen = (usMaxTmpBuff - (SERTMPFILE_DATASTART + sizeof(CLIRESGCF)))/sizeof(GCF_STATUS_STATE);
			ResMsgH.sReturn = GusAllIdReadFiltered(pReqMsgH->usGCType, pReqMsgH->ulWaiterNo, pReqMsgH->usTableNo, (GCF_STATUS_STATE *)SerResp.pSavBuff->auchData, SerResp.pSavBuff->usDataLen);
			if (0 <= ResMsgH.sReturn) {
				SerResp.pSavBuff->usDataLen = ResMsgH.sReturn * sizeof(GCF_STATUS_STATE);
			} else {
				SerResp.pSavBuff->usDataLen = 0;
			}
			ResMsgH.sResCode = STUB_MULTI_SEND;
		}
        break;

    case    CLI_FCGCREADUNIQUEID:           /* V3.3 */
		{
			CLIREQGCFFILTERED * const pReqMsgH = (CLIREQGCFFILTERED *)SerRcvBuf.auchData;

			SerResp.pSavBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESGCF)];
            SerResp.ulSavBuffSize = sizeof(auchSerTmpBuf) - sizeof(CLIRESGCF);
            SstReadFileFH(0, &usMaxTmpBuff, sizeof(usMaxTmpBuff), hsSerTmpFile);/* ### ADD (2171 for Win32) V1.0 */
			SerResp.pSavBuff->usDataLen = (usMaxTmpBuff - (SERTMPFILE_DATASTART + sizeof(CLIRESGCF)))/sizeof(GCF_STATUS_STATE);
			ResMsgH.sReturn = GusSearchForBarCode(pReqMsgH->usGCType, pReqMsgH->uchUniqueIdentifier, (GCF_STATUS_STATE *)SerResp.pSavBuff->auchData, SerResp.pSavBuff->usDataLen);
			if (0 <= ResMsgH.sReturn) {
				SerResp.pSavBuff->usDataLen = ResMsgH.sReturn * sizeof(GCF_STATUS_STATE);
			} else {
				SerResp.pSavBuff->usDataLen = 0;
			}
			ResMsgH.sResCode = STUB_MULTI_SEND;
		}
        break;

    default:                                    /* not used */
		// Issue a PifLog() to record that we have an error condition then
		// return an error to the sender.  This is a bad function code.
		PifLog (MODULE_SER_LOG, LOG_EVENT_SER_GCF_BAD_FUNCODE);
		ResMsgH.sReturn = GCF_COMERROR;
        //return;
        break;
    }

    if (STUB_MULTI_SEND == ResMsgH.sResCode) {
        sSerSendStatus = SerSendMultiple((CLIRESCOM *)&ResMsgH, sizeof(CLIRESGCF));
		if (sSerSendStatus < 0) {
			char xBuff [128];
			sprintf (xBuff, "SerRecvGCF(): sSerSendStatus = %d, ip 0x%x, usFunCode = 0x%x, usSeqNo = 0x%x, sReturn %d", sSerSendStatus, *(ULONG *)SerRcvBuf.auchFaddr, pReqMsgH->usFunCode, pReqMsgH->usSeqNo, ResMsgH.sReturn);
			NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
		}
    } else {
        sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESGCF), NULL, 0);
		if (sSerSendStatus < 0) {
			char xBuff [128];
			sprintf (xBuff, "SerRecvGCF(): sSerSendStatus = %d, ip 0x%x, usFunCode = 0x%x, usSeqNo = 0x%x, sReturn %d", sSerSendStatus, *(ULONG *)SerRcvBuf.auchFaddr, pReqMsgH->usFunCode, pReqMsgH->usSeqNo, ResMsgH.sReturn);
			NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
		}
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerChekGCFUnLock(VOID);
*
** Return:      none.
*
** Description:
*   This function is called when the multi. sending becomes time out.
*   This function checks GCF should be unlock or not.
*===========================================================================
*/
VOID    SerChekGCFUnLock(VOID)
{
    CLIRESGCF   * const pResMsgH = (CLIRESGCF *)auchSerTmpBuf;

    if (CLI_FCGCFREADLOCK == (pResMsgH->usFunCode & CLI_RSTCONTCODE)) {
        GusResetReadFlag(pResMsgH->usGCN, GCF_NO_APPEND);
    }
}

/*===== END OF SOURCE =====*/
