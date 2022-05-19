/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : SERVER module, Operational Para. Functions Source File
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVOPR.C                                            
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*           SerRmtFileServer()
*           SerOpUnjoinCluster ()
*           SerOpJoinCluster ()               Handle Join Cluster operation 
*           SerOpDisplayOperatorMessage ()    Handle storage of Operator Message, broadcast
*           SerOpRetrieveOperatorMessage ()   Handle request for Operator Message received
*           SerOpTransferTotals ()            Handle transfer of totals with Join
*           SerOpUpdateTotalsMsg ()
*           SerRecvOpr();                     Operational Parameter function handling
*           SerRecvOprPlu()                   Handle request PLU data/information
*           SerRecvOprPara()                  Handle request operational parameter (PARA)
*           SerConvertDateTime();             Convert date/ time structure
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* Dec-20-99:hrkato     : Saratoga
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
#include	<math.h>

#include    <ecr.h>
#include    <appllog.h>
#include    <pif.h>
#include    <rfl.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <csop.h>
#include    "csstbopr.h"
#include    "servmin.h"
#include	"ispcom.h"
#include    "csstbttl.h"
#include    <csstbstb.h>
#include    <nb.h>
#include    <BlFwIf.h>
#define __BLNOTIFY_C__
#include    <bl.h>
#include    <cscas.h>
#include    <maint.h>

//-------------------  Helper functions
//

void SerAssignClireqmntpluToPluif (PLUIF *pPlu, CLIREQMNTPLU *pReqMsgM)
{
    memcpy(pPlu->auchPluNo, pReqMsgM->aucPluNumber, sizeof(pPlu->auchPluNo));
    pPlu->uchPluAdj               = pReqMsgM->uchAdjective;
    pPlu->ParaPlu.ContPlu.usDept  = pReqMsgM->usDeptNumber;
    pPlu->ParaPlu.ContPlu.uchItemType = pReqMsgM->uchItemType;
    RflConv4bto3b(pPlu->ParaPlu.auchPrice, pReqMsgM->ulUnitPrice);
    pPlu->ParaPlu.uchPriceMulWeight   = pReqMsgM->uchPriceMulWeight;
    memcpy(pPlu->ParaPlu.auchPluName, pReqMsgM->aucMnemonic, sizeof(pPlu->ParaPlu.auchPluName));
    memcpy(pPlu->ParaPlu.ContPlu.auchContOther, pReqMsgM->aucStatus, sizeof(pPlu->ParaPlu.ContPlu.auchContOther));
    pPlu->ParaPlu.usFamilyCode    = pReqMsgM->usFamilyCode;

	pPlu->ParaPlu.ContPlu.uchRept = pReqMsgM->uchReportCode;
    pPlu->ParaPlu.usLinkNo        = pReqMsgM->usLinkNumber;
    pPlu->ParaPlu.uchRestrict     = pReqMsgM->uchRestrict;
    pPlu->ParaPlu.uchTableNumber  = pReqMsgM->uchTableNumber;
    pPlu->ParaPlu.uchGroupNumber  = pReqMsgM->uchGroupNumber;
    pPlu->ParaPlu.uchPrintPriority= pReqMsgM->uchPrintPriority;
    pPlu->ParaPlu.uchLinkPPI      = pReqMsgM->uchLinkPPI; /* 10/10/00 */
    memcpy(pPlu->ParaPlu.auchAltPluName, pReqMsgM->auchAltPluName, sizeof(pPlu->ParaPlu.auchAltPluName));
	pPlu->ParaPlu.uchColorPaletteCode = pReqMsgM->uchColorPaletteCode;
	pPlu->ParaPlu.ContPlu.usBonusIndex  = pReqMsgM->usBonusIndex;
	pPlu->ParaPlu.usTareInformation     = pReqMsgM->usTareInformation;
	memcpy (pPlu->ParaPlu.uchExtendedGroupPriority, pReqMsgM->uchExtendedGroupPriority, sizeof(pPlu->ParaPlu.uchExtendedGroupPriority));
}

void SerAssignPluifToCliresscnplu (CLIRESSCNPLU *pResMsgH, PLUIF *pPlu)
{
    memcpy(pResMsgH->aucPluNumber, pPlu->auchPluNo, sizeof(pResMsgH->aucPluNumber));
    pResMsgH->uchAdjective        = pPlu->uchPluAdj;
    pResMsgH->usDeptNumber        = pPlu->ParaPlu.ContPlu.usDept;
    pResMsgH->uchItemType         = pPlu->ParaPlu.ContPlu.uchItemType;
    pResMsgH->ulUnitPrice         = RflFunc3bto4b(pPlu->ParaPlu.auchPrice);
    pResMsgH->uchPriceMulWeight   = pPlu->ParaPlu.uchPriceMulWeight;
    memcpy(pResMsgH->aucMnemonic, pPlu->ParaPlu.auchPluName, sizeof(pResMsgH->aucMnemonic));
    memcpy(pResMsgH->aucStatus, pPlu->ParaPlu.ContPlu.auchContOther, sizeof(pResMsgH->aucStatus));
    pResMsgH->uchReportCode       = pPlu->ParaPlu.ContPlu.uchRept;
    pResMsgH->usLinkNumber        = pPlu->ParaPlu.usLinkNo;
    pResMsgH->uchRestrict         = pPlu->ParaPlu.uchRestrict;
    pResMsgH->uchTableNumber      = pPlu->ParaPlu.uchTableNumber;
    pResMsgH->uchGroupNumber      = pPlu->ParaPlu.uchGroupNumber;
    pResMsgH->uchPrintPriority    = pPlu->ParaPlu.uchPrintPriority;
    pResMsgH->usFamilyCode        = pPlu->ParaPlu.usFamilyCode;
    pResMsgH->uchLinkPPI          = pPlu->ParaPlu.uchLinkPPI;   /* 10/10/00 */
    memcpy(pResMsgH->auchAltPluName, pPlu->ParaPlu.auchAltPluName, sizeof(pResMsgH->auchAltPluName));
	pResMsgH->uchColorPaletteCode = pPlu->ParaPlu.uchColorPaletteCode;
	pResMsgH->usBonusIndex        = pPlu->ParaPlu.ContPlu.usBonusIndex;
	pResMsgH->usTareInformation   = pPlu->ParaPlu.usTareInformation;
	memcpy (pResMsgH->uchExtendedGroupPriority, pPlu->ParaPlu.uchExtendedGroupPriority, sizeof(pResMsgH->uchExtendedGroupPriority));
}

//--------------------------------------------

/*
*===========================================================================
** Synopsis:    VOID    SerRmtFileServer(VOID);
*
** Return:      none.
*
** Description: This function executes for request layout files
*===========================================================================
*/ //ESMITH LAYOUT
VOID SerRmtFileServer(VOID)
{
    VOID    *pReqMsgH, *pResMsgH;
    USHORT  usLen;
    SHORT   sResult,sFileHandle;
    ULONG   ulActPos;
	ULONG	ulActualBytesRead;
	UCHAR	auchSerTmpBuf[PIF_LEN_UDATA];

	NHPOS_ASSERT_ARRAYSIZE(auchSerTmpBuf, PIF_LEN_UDATA);

	/* clear pesponse message   */
    usLen = 0;
    memset(auchSerTmpBuf, 0, sizeof(auchSerTmpBuf));

	pReqMsgH = SerRcvBuf.auchData;
	pResMsgH = auchSerTmpBuf;

    ((CLIRESCOM *)pResMsgH)->usFunCode = ((CLIREQCOM *)pReqMsgH)->usFunCode;
    ((CLIRESCOM *)pResMsgH)->usSeqNo   = ((CLIREQCOM *)pReqMsgH)->usSeqNo &
                                         CLI_SEQ_CONT;
    ((CLIRESCOM *)pResMsgH)->sResCode  = STUB_SUCCESS;

    switch(((CLIREQCOM *)pReqMsgH)->usFunCode & CLI_RSTCONTCODE) {

    /* read the file    */
    case CLI_FCRMTREADFILE:

        usLen = sizeof(CLIRESREAD);

		if((sFileHandle = PifOpenFile(((CLIREQREAD *)pReqMsgH)->aszFileName, ((CLIREQREAD *)pReqMsgH)->aszMode)) >= 0) {
			if((sResult = PifSeekFile(sFileHandle, ((CLIREQREAD *)pReqMsgH)->ulPosition, &ulActPos)) < 0) {
				char  xBuff[128];

				((CLIRESREAD *)pResMsgH)->lReturn = sResult;
				PifCloseFile(sFileHandle);
				sprintf (xBuff, "SerRmtFileServer(): PifSeekFile() status = %d", sResult);
				NHPOS_ASSERT_TEXT((sResult >= 0), xBuff);
				break;
			}

			PifReadFile(sFileHandle, ((CLIRESREAD *)pResMsgH)->auchData, ((CLIREQREAD *)pReqMsgH)->usBytes, &ulActualBytesRead);

			if ( ulActualBytesRead >= 0) {
				((CLIRESREAD *)pResMsgH)->ulPosition = ((CLIREQREAD *)pReqMsgH)->ulPosition;
			}

			((CLIRESREAD *)pResMsgH)->lReturn = ulActualBytesRead;

			PifCloseFile(sFileHandle);
		} else {
			char  xBuff[128];

			sprintf (xBuff, "SerRmtFileServer(): PifOpenFile() status = %d", sFileHandle);
			NHPOS_ASSERT_TEXT((sFileHandle >= 0), xBuff);
		}

        break;
    }

    /* send response message to PC  */
	SerSendResponse((CLIRESCOM *)pResMsgH, usLen, NULL, 0);
}



SHORT SerOpUnjoinCluster (VOID)
{
    TCHAR  wszHostName[PIF_LEN_HOST_NAME + 1] = { 0 };
	UCHAR  auchHostIpAddress[4];
	UCHAR  auchLocalAddr[4];
	UCHAR  auchTerminalNo = 0;
    CLIREQJOIN    *pReqMsgH;
    CLIRESJOIN    ResMsgH;

#if 1
	if (STUB_SUCCESS != CstIamMaster ()) {
		return 0;
	}

	if (STUB_SUCCESS == CstIamDisconnectedSatellite()) {
		return 0;
	}
#else
	// This #else clause is executed if the developer wants to do testing of
	// Join functionality with a single terminal or with a non-Master Terminal.
	// This functionality should never be enabled for a Release build so it
	// has a check to trigger a compiler error if the compile is being done
	// in Release mode.
	{
#pragma message("  ****   ")
		UCHAR dummyvalue = 0x1fda;
#pragma message("  ****   CstIamMaster() in SerOpUnjoinCluster() stubbed out.  Responding to CLIREQJOIN messages.   ******")
	}
#if !defined(_DEBUG)
	I_Will_Create_An_Error++;
#endif
#pragma message("  ****   ")
#endif

    pReqMsgH = (CLIREQJOIN *)SerRcvBuf.auchData;

    memset(&ResMsgH, 0, sizeof(ResMsgH));
    ResMsgH.usFunCode = pReqMsgH->usFunCode;
    ResMsgH.usSeqNo   = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.sResCode  = STUB_SUCCESS;

	ResMsgH.sReturn = OpUnjoinCluster(pReqMsgH->wszHostName, pReqMsgH->auchHostIpAddress, &(pReqMsgH->auchTerminalNo));

    memset(wszHostName, 0, sizeof(wszHostName));
    memset(auchLocalAddr, 0, sizeof(auchLocalAddr));
    memset(auchHostIpAddress, 0, sizeof(auchHostIpAddress));
	MaintPifGetHostAddress(auchLocalAddr, auchHostIpAddress, wszHostName);

	memcpy (ResMsgH.wszMasName, wszHostName, sizeof(ResMsgH.wszMasName));
	memcpy (ResMsgH.auchMasIpAddress, auchHostIpAddress, sizeof(ResMsgH.auchMasIpAddress));
	ResMsgH.auchTerminalNo = pReqMsgH->auchTerminalNo;

    PifLog (MODULE_SER_LOG, LOG_EVENT_SER_UNJOINMSG_RECEIVED);
    PifLog (MODULE_DATA_VALUE(MODULE_SER_LOG), ResMsgH.auchTerminalNo);

    /* send response message to PC  */
	SerSendResponseToIpAddress(pReqMsgH->auchHostIpAddress, (CLIRESCOM *)&ResMsgH, sizeof(ResMsgH), NULL, 0);

	return 1;
}

SHORT SerOpJoinCluster(VOID)
{
	UCHAR  auchHostIpAddress[4];
	UCHAR  auchLocalAddr[4];
	UCHAR  auchTerminalNo = 0;
	CLIREQJOIN    *pReqMsgH;
	CLIRESJOIN    ResMsgH;
	PIF_CLUSTER_HOST  PifHostInfo;

	pReqMsgH = (CLIREQJOIN *)SerRcvBuf.auchData;

#if 1
	if (STUB_SUCCESS != CstIamMaster()) {
		return 0;
	}

	if (STUB_SUCCESS == CstIamDisconnectedSatellite()) {
		return 0;
	}
#else
	// This #else clause is executed if the developer wants to do testing of
	// Join functionality with a single terminal or with a non-Master Terminal.
	// This functionality should never be enabled for a Release build so it
	// has a check to trigger a compiler error if the compile is being done
	// in Release mode.
	{
#pragma message("  ****   ")
		UCHAR dummyvalue = 0x1fda;
#pragma message("  ****   CstIamMaster() in SerOpJoinCluster() stubbed out.  Responding to CLIREQJOIN messages. \z  ******")
	}
#if !defined(_DEBUG)
	I_Will_Create_An_Error++;
#endif
#pragma message("  ****   ")
#endif

	memset(&ResMsgH, 0, sizeof(ResMsgH));
	ResMsgH.usFunCode = pReqMsgH->usFunCode;
	ResMsgH.usSeqNo = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
	ResMsgH.sResCode = STUB_SUCCESS;

	_tcscpy(PifHostInfo.wszHostName, pReqMsgH->wszHostName);
	memcpy(PifHostInfo.auchHostIpAddress, pReqMsgH->auchHostIpAddress, 4);
	PifHostInfo.usStoreNumber = pReqMsgH->usStoreNumber;         // The store number for this terminal
	PifHostInfo.usRegisterNumber = pReqMsgH->usRegisterNumber;   // the number of this particular terminal or register

	if (OpSignInStatus() != 0) {
		ResMsgH.sReturn = OP_LOCK;
	}
	else {
		TCHAR  wszHostName[PIF_LEN_HOST_NAME + 1] = { 0 };

		ResMsgH.sReturn = OpJoinCluster(&PifHostInfo, &(pReqMsgH->auchTerminalNo));

    	memset(wszHostName, 0, sizeof(wszHostName));
		memset(auchLocalAddr, 0, sizeof(auchLocalAddr));
		memset(auchHostIpAddress, 0, sizeof(auchHostIpAddress));
		MaintPifGetHostAddress(auchLocalAddr, auchHostIpAddress, wszHostName);

		memcpy (ResMsgH.wszMasName, wszHostName, sizeof(ResMsgH.wszMasName));
		memcpy (ResMsgH.auchMasIpAddress, auchHostIpAddress, sizeof(ResMsgH.auchMasIpAddress));
		ResMsgH.auchTerminalNo = pReqMsgH->auchTerminalNo;

		{
			SERINQSTATUS    InqData;
			USHORT  usUA = ResMsgH.auchTerminalNo - 1;

			if (ResMsgH.auchTerminalNo == 0)
				usUA = 0;

			SstReadInqStat(&InqData);
			ResMsgH.ausTranNo = InqData.ausTranNo[usUA];          /* Transaction # for this terminal number */
			ResMsgH.ausPreTranNo = InqData.ausPreTranNo[usUA];    /* Prev. Transaction # for this terminal number */
		}

		{
			UCHAR  ucOperationalFlags[4] = {0, 0, 0, 0};

			SEROPDISPLAY_OPTIONS_SET(SEROPDISPLAY_FORCE_SIGNOUT,ucOperationalFlags);
			SEROPDISPLAY_OPTIONS_SET(SEROPDISPLAY_AUTO_SIGNOUT,ucOperationalFlags);
			SEROPDISPLAY_OPTIONS_SET(SEROPDISPLAY_SIGNIN_BLOCK,ucOperationalFlags);
			SEROPDISPLAY_OPTIONS_SET(SEROPDISPLAY_SIGNIN_UNBLOCK,ucOperationalFlags);

			// Set special flags for the Joining terminal to perform particular actions as
			// part of completing the Join.
			ResMsgH.ulSpecialFlags = 0; 
			if (SerOpCheckOperatorMessageFlags(ucOperationalFlags)) {
				ResMsgH.ulSpecialFlags |= CLIRESJOIN_FLAGS_CHECK_OP_MSG; 
			}
		}
	}

    PifLog (MODULE_SER_LOG, LOG_EVENT_SER_JOINMSG_RECEIVED);
	PifLog (MODULE_DATA_VALUE(MODULE_SER_LOG), ResMsgH.auchTerminalNo);
	PifLog (MODULE_DATA_VALUE(MODULE_SER_LOG), ResMsgH.ausTranNo);
	PifLog (MODULE_ERROR_NO(MODULE_SER_LOG),   ResMsgH.sReturn);

    /* send response message to PC  */
	SerSendResponseToIpAddress(pReqMsgH->auchHostIpAddress, (CLIRESCOM *)&ResMsgH, sizeof(ResMsgH), NULL, 0);

	{
		CHAR  xBuffer[256];

		sprintf (xBuffer, "SerOpJoinCluster (): ip address: %d.%d.%d.%d  ResMsgH.sReturn = %d", pReqMsgH->auchHostIpAddress[0], pReqMsgH->auchHostIpAddress[1], pReqMsgH->auchHostIpAddress[2], pReqMsgH->auchHostIpAddress[3], ResMsgH.sReturn);
		NHPOS_ASSERT_TEXT(0, xBuffer);
	}
	return 1;
}

/*
*	Following functions used for the Operator Message functionality.
*
*	SerOpDisplayOperatorMessage() handles a CLI_FCOPREQDISPMSG operation.
*	SerOpRetrieveOperatorMessage() handles a CLI_FCOPREQMSGBUFFER operation
*
*	Currently the last Operator Message received is stored in LastOperatorMessageReceived
*	from which it is retrieved when requested by another terminal.
*/
static CLIREQMSGDISP LastOperatorMessageReceived = {0};

SHORT SerOpDisplayOperatorMessage (CLIREQMSGDISP *pReqMsgH, CLIRESMSGDISP *pResMsgH)
{
	TCHAR  tcsMessage[512];
	ULONG  ulMessageLength = 0;
	USHORT usOpFlags = 0;

    pResMsgH->sResCode  = STUB_SUCCESS;

	LastOperatorMessageReceived = *pReqMsgH;

	// ensure that the message is will fit in our buffer and be zero terminated
	// for the end of string.  Zero out to ensure we have an end of string.
	memset (tcsMessage, 0, sizeof(tcsMessage));
	ulMessageLength = pReqMsgH->usMsgLen * sizeof(pReqMsgH->usTextMessage[0]);
	if (ulMessageLength > sizeof(tcsMessage)) {
		ulMessageLength = sizeof(tcsMessage) - sizeof(tcsMessage[0]);
	}

	memcpy (tcsMessage, pReqMsgH->usTextMessage, ulMessageLength);

	if (ulMessageLength > 0)
		BlFwIfPopupMessageWindow (tcsMessage);

	// Notify other terminals in the cluster that there is an operator
	// message that is available.
	if (SEROPDISPLAY_OPTIONS_ISSET (SEROPDISPLAY_NO_BROADCAST, pReqMsgH->ucOperationalFlags)) {
		NbResetMessage(NB_MESOFFSET0, NB_REQ_OPERATOR_MSG);
		NHPOS_NONASSERT_TEXT("Received message with Do not Broadcast Set");
		usOpFlags |= 0x0001;
	}
	else {
		NbWriteMessage(NB_MESOFFSET0, NB_REQ_OPERATOR_MSG);
	}

	if (SEROPDISPLAY_OPTIONS_ISSET (SEROPDISPLAY_DELETE_MSGS, pReqMsgH->ucOperationalFlags)) {
		NHPOS_NONASSERT_TEXT("Received message with Delete Messages Set");
		usOpFlags |= 0x0002;
	}

	SerOpClearOperatorMessageStatusTable (0x0000);  // init the operator status message table.
	if (SEROPDISPLAY_OPTIONS_ISSET (SEROPDISPLAY_FORCE_SIGNOUT, pReqMsgH->ucOperationalFlags)) {
		NHPOS_NONASSERT_TEXT("Received message with Force Sign-out Set");
		BlFwIfSetAutoSignOutTime (0, BLFWIF_AUTO_MODE_NO_CHANGE);  // Clear timer if set
		PifSleep (50);
		CasForceAutoSignOut(1);
		usOpFlags |= 0x0008;
		SerOpClearOperatorMessageStatusTable (0x0001);
	}
	else if (SEROPDISPLAY_OPTIONS_ISSET (SEROPDISPLAY_AUTO_SIGNOUT, pReqMsgH->ucOperationalFlags)) {
		NHPOS_NONASSERT_TEXT("Received message with Auto Sign-out Set");
		BlFwIfSetAutoSignOutTime (50, BLFWIF_AUTO_MODE_NO_CHANGE);
		usOpFlags |= 0x0004;
	}

	if (SEROPDISPLAY_OPTIONS_ISSET (SEROPDISPLAY_SIGNIN_UNBLOCK, pReqMsgH->ucOperationalFlags)) {
		NHPOS_NONASSERT_TEXT("Received message with Unblock Sign-in Set");
		OpSignInBlockOff ();
		usOpFlags |= 0x0020;
	}
	else if (SEROPDISPLAY_OPTIONS_ISSET (SEROPDISPLAY_SIGNIN_BLOCK, pReqMsgH->ucOperationalFlags)) {
		NHPOS_NONASSERT_TEXT("Received message with Block Sign-in Set");
		OpSignInBlockOn ();
		usOpFlags |= 0x0010;
	}

	if ((usOpFlags & (0x0008 | 0x0010)) == (0x0008 | 0x0010)) {
		if (SEROPDISPLAY_OPTIONS_ISSET (SEROPDISPLAY_AUTO_SIGNOUT, pReqMsgH->ucOperationalFlags)) {
			SerOpClearOperatorMessageStatusTable (0x0003);  // indicate force signout and clear any lingering, possible zombie cashieres
		}
	}

	if (SEROPDISPLAY_OPTIONS_ISSET (SEROPDISPLAY_FORCE_SHUTDOWN, pReqMsgH->ucOperationalFlags)) {
		NHPOS_NONASSERT_TEXT("Received message with Force Shutdown Set");
		usOpFlags |= 0x0040;
		BlFwIfSetAutoSignOutTime (0, BLFWIF_AUTO_MODE_NO_CHANGE);  // Clear timer if set
		PifSleep (50);
		CasForceAutoSignOut(1);
		BlFwIfForceShutdown();
	}

	PifLog (MODULE_SER_LOG, LOG_EVENT_STB_OP_MSG_PROCESSED);
	PifLog (MODULE_DATA_VALUE(MODULE_SER_LOG), usOpFlags);  // indicate which flags were set in this message
	return 1;
}

SHORT SerOpRetrieveOperatorMessage (CLIREQMSGBUFFER *pReqMsgH, CLIRESMSGBUFFER *pResMsgH)
{
	ULONG  ulMessageLength = 0;

    pResMsgH->sResCode  = STUB_SUCCESS;

	pResMsgH->usMsgLen = LastOperatorMessageReceived.usMsgLen;

	ulMessageLength = pResMsgH->usMsgLen * sizeof(pResMsgH->usTextMessage[0]);
	if (ulMessageLength >= sizeof(LastOperatorMessageReceived.usTextMessage)) {
		ulMessageLength = sizeof(LastOperatorMessageReceived.usTextMessage) - sizeof(LastOperatorMessageReceived.usTextMessage[0]);
	}
	memcpy (pResMsgH->usTextMessage, LastOperatorMessageReceived.usTextMessage, ulMessageLength);
	memcpy (pResMsgH->ucOperationalFlags, LastOperatorMessageReceived.ucOperationalFlags, sizeof(pResMsgH->ucOperationalFlags));

	return 1;
}

SHORT SerOpCheckOperatorMessageFlags (UCHAR *ucOperationalFlags)
{
	SHORT  sRetStatus = 0;
	int    iLoop = 0;
	int    iMaxCount = sizeof(LastOperatorMessageReceived.ucOperationalFlags)/sizeof(LastOperatorMessageReceived.ucOperationalFlags[0]);

	for (iLoop = 0; iLoop < iMaxCount; iLoop++) {
		if ((ucOperationalFlags[iLoop] & LastOperatorMessageReceived.ucOperationalFlags[iLoop]) != 0) {
			sRetStatus = 1;
			break;
		}
	}

	return sRetStatus;
}


#if !defined(CAS_TERMINAL_NO_EXTENDED)
#define  CAS_TERMINAL_NO_EXTENDED  255
#endif

static ULONG    ulSerOperatorMessageStatusTableIndicator = 0;
static USHORT   ausCasTerminalOperatorMessageTbl[CAS_TERMINAL_NO_EXTENDED];

SHORT SerOpClearOperatorMessageStatusTable (ULONG ulIndicator)
{
	ulSerOperatorMessageStatusTableIndicator = ulIndicator;
	memset (ausCasTerminalOperatorMessageTbl, 0, sizeof(ausCasTerminalOperatorMessageTbl));
	return 0;
}

LONG SerOpSetOperatorMessageStatusTable (USHORT usTerminalNo, USHORT usFlags)
{
	LONG    lRetStatus = -1;
	USHORT  usIndex = usTerminalNo - 1;

	if (usIndex >= 0 && usIndex < CAS_TERMINAL_NO_EXTENDED) {
		ULONG  ulTemp = ausCasTerminalOperatorMessageTbl[usIndex];
		lRetStatus = (LONG)(ulTemp & 0xffff);
		ausCasTerminalOperatorMessageTbl[usIndex] = usFlags;
	}

	return lRetStatus;
}

LONG SerOpGetOperatorMessageStatusTable (USHORT usTerminalNo, ULONG *pulStatus)
{
	LONG    lRetStatus = -1;
	USHORT  usIndex = usTerminalNo - 1;

	if (pulStatus) {
		pulStatus[0] = 0;
		pulStatus[1] = 0;
	}
	if (usIndex >= 0 && usIndex < CAS_TERMINAL_NO_EXTENDED) {
		ULONG  ulTemp = ausCasTerminalOperatorMessageTbl[usIndex];
		lRetStatus = (LONG)(ulTemp & 0xffff);
		if (pulStatus) {
			pulStatus[0] = ulSerOperatorMessageStatusTableIndicator;
			pulStatus[1] = lRetStatus;
		}
	}

	return lRetStatus;
}

/*
*	SerOpTransferTotals() handles a CLI_FCOPTRANSFERTOTALS operation.
*
*	This function uses a list of the totals files to transfer and then
*	issues a request to the terminal from which it has received the
*	Totals Transfer request to transfer the totals file.
*
*	The function CliOpTransferTotals() is used on the Client side to initiate
*	a Totals Transfer.
*/
SHORT SerOpTransferTotals (VOID)
{
	SHORT  sSerSendStatus = 0;
	UCHAR  auchTerminalNo = 0;
	TCHAR  szTotalLocalFileName[16];
	TCHAR *(TotalRemoteFileNameList []) = {
		_T("TOTALBAS"),   /* Base Total File Name, szTtlBaseFileName */
		_T("TOTALCAS"),   /* Cashier Total File Name, szTtlCasFileName */
		_T("TOTALCPN"),   /* Coupon Total File Name, R3.0, szTtlCpnFileName */
		_T("TOTALDEP"),   /* Dept Total File Name, szTtlDeptFileName */
		_T("TOTALETK"),   /* Employee Time Keeping Total File Name */
		_T("TOTALIND"),   /* Individual Financial, R3.1, szTtlIndFileName */
		_T("TOTALTIM"),   /* Serive Time, R3.1, szTtlSerTimeName */
		_T("TOTALPLD"),   /* PLU Totals interim file, CLASS_TTLCURDAY */
		_T("TOTALPLP"),   /* PLU Totals interim file, CLASS_TTLCURPTD */
		NULL };
	TCHAR *(TotalLocalFileNameList []) = {
		_T("ZATALBAS"),   /* Base Total File Name, szTtlBaseFileName */
		_T("ZATALCAS"),   /* Cashier Total File Name, szTtlCasFileName */
		_T("ZATALCPN"),   /* Coupon Total File Name, R3.0, szTtlCpnFileName */
		_T("ZATALDEP"),   /* Dept Total File Name, szTtlDeptFileName */
		_T("ZATALETK"),   /* Employee Time Keeping Total File Name */
		_T("ZATALIND"),   /* Individual Financial, R3.1, szTtlIndFileName */
		_T("ZATALTIM"),   /* Serive Time, R3.1, szTtlSerTimeName */
		_T("ZATALPLD"),   /* PLU Totals interim file, CLASS_TTLCURDAY */
		_T("ZATALPLP"),   /* PLU Totals interim file, CLASS_TTLCURPTD */
		NULL };
    CLIREQTRANSFER    *pReqMsgH;
    CLIRESTRANSFER    ResMsgH;
	int               iLoop;
	USHORT            usLockHnd = 0;
	USHORT            usTerminalNo = 0;

    pReqMsgH = (CLIREQTRANSFER *)SerRcvBuf.auchData;

    memset(&ResMsgH, 0, sizeof(ResMsgH));
    ResMsgH.usFunCode = pReqMsgH->usFunCode;
    ResMsgH.usSeqNo   = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.sResCode  = STUB_SUCCESS;

	ResMsgH.auchTerminalNo = pReqMsgH->auchTerminalNo;
	usTerminalNo = pReqMsgH->auchTerminalNo;
	if (usTerminalNo > PIF_NET_MAX_IP)
		usTerminalNo = 0;

#if 1
	if (STUB_SUCCESS != CstIamMaster ()) {
		ResMsgH.sReturn =  OP_EOF;
		sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(ResMsgH), NULL, 0);
		if (sSerSendStatus < 0) {
			char xBuff [128];
			sprintf (xBuff, "SerOpTransferTotals(): sSerSendStatus = %d,  usTerminalNo = %d, sReturn = %d", sSerSendStatus, usTerminalNo, ResMsgH.sReturn);
			NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
		}
		return 0;
	}

	if (STUB_SUCCESS == CstIamDisconnectedSatellite()) {
		ResMsgH.sReturn =  OP_EOF;
		sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(ResMsgH), NULL, 0);
		if (sSerSendStatus < 0) {
			char xBuff [128];
			sprintf (xBuff, "SerOpTransferTotals(): sSerSendStatus = %d,  usTerminalNo = %d, sReturn = %d", sSerSendStatus, usTerminalNo, ResMsgH.sReturn);
			NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
		}
		return 0;
	}
#else
	// This #else clause is executed if the developer wants to do testing of
	// Join functionality with a single terminal or with a non-Master Terminal.
	// This functionality should never be enabled for a Release build so it
	// has a check to trigger a compiler error if the compile is being done
	// in Release mode.
	{
#pragma message("  ****   ")
		UCHAR dummyvalue = 0x1fda;
#pragma message("  ****   CstIamMaster() in SerOpTransferTotals() stubbed out.  Responding to CLIREQTRANSFER messages. \z  ******")
	}
#if !defined(_DEBUG)
	I_Will_Create_An_Error++;
#endif
#pragma message("  ****   ")
#endif

	for (iLoop = 0; TotalLocalFileNameList[iLoop] && TotalRemoteFileNameList[iLoop]; iLoop++) {
		_tcscpy (szTotalLocalFileName, TotalLocalFileNameList[iLoop]);
		szTotalLocalFileName[1] += usTerminalNo;
		ResMsgH.sReturn = OpTransferFile(pReqMsgH->auchTerminalNo, szTotalLocalFileName, TotalRemoteFileNameList[iLoop], usLockHnd);
		_tcsncpy (ResMsgH.wszRemoteFileName, TotalRemoteFileNameList[iLoop], 14);
		/* send response message to PC  */
		ResMsgH.sResCode = STUB_SUCCESS;
		if (ResMsgH.sReturn != OP_PERFORM) {
			ResMsgH.sResCode  = STUB_ILLEGAL;
			ResMsgH.sReturn = OP_PERFORM;
		}
		sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(ResMsgH), NULL, 0);
		if (ResMsgH.sResCode != STUB_SUCCESS) {
			char  xBuff[128];
			sprintf (xBuff, "SerOpTransferTotals(): Transfer error file # %d usTerminalNo = %d.", iLoop+1, usTerminalNo);
			NHPOS_ASSERT_TEXT((ResMsgH.sResCode == STUB_SUCCESS), xBuff); 
		}
		if (sSerSendStatus < 0) {
			char xBuff [128];
			sprintf (xBuff, "SerOpTransferTotals(): sSerSendStatus = %d,  usTerminalNo = %d, sReturn = %d", sSerSendStatus, usTerminalNo, ResMsgH.sReturn);
			NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
		}
	}

	ResMsgH.sReturn =  OP_EOF;
	ResMsgH.sResCode  = STUB_SUCCESS;
	sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(ResMsgH), NULL, 0);
	if (sSerSendStatus < 0) {
		char xBuff [128];
		sprintf (xBuff, "SerOpTransferTotals(): sSerSendStatus = %d, usTerminalNo = %d, sReturn = %d", sSerSendStatus, usTerminalNo, ResMsgH.sReturn);
		NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
	}

	SerTtlStartTransferredTotalsUpdate (pReqMsgH->auchTerminalNo);

    PifLog (MODULE_SER_LOG, LOG_EVENT_SER_TRANSFERRQST_RECEIVED);
    PifLog (MODULE_DATA_VALUE(MODULE_SER_LOG), pReqMsgH->auchTerminalNo);
	return 1;
}

SHORT SerOpUpdateTotalsMsg (TTLMESSAGEUNION *pTtlMessage)
{
	SHORT            sRetStatus = OP_PERFORM;

	sRetStatus = TtlWriteNextStoreForwardTtlMessage (pTtlMessage);

	if (sRetStatus == OP_PERFORM) {
		SerTtlStartUpdateTotalsMsg (0);
	}
	return sRetStatus;
}



/*
*===========================================================================
** Synopsis:    VOID    SerRecvOpr(VOID);
*
** Return:      none.
*
** Description: This function executes for request operational parameter.
*===========================================================================
*/
VOID    SerRecvOpr(VOID)
{
    SHORT           sFlag = 0;
	SHORT           sSerSendStatus = 0;
    CLIREQOPPARA    *pReqMsgH;
    CLIRESOPPARA    ResMsgH;
    CLIREQDATA      *pReqBuff;
    PLUIF           *pPlu;
    PLUIF_REP       *pPluIf_Rep;
    PLUIF_DEP       *pPluIf_Dep;
    static PLUIF           PluIf;       /* saratoga */
    static PLUIF_REP       PluIf_Rep;   /* saratoga */
    static PLUIF_DEP       PluIf_Dep;   /* saratoga */

    pReqMsgH = (CLIREQOPPARA *)SerRcvBuf.auchData;
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ResMsgH.usFunCode     = pReqMsgH->usFunCode;
    ResMsgH.usSeqNo       = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.sResCode      = STUB_MULTI_SEND;
    ResMsgH.sReturn       = STUB_SUCCESS;
    ResMsgH.uchMajorClass = pReqMsgH->uchMajorClass;
    ResMsgH.usHndorOffset = pReqMsgH->usHndorOffset;
    ResMsgH.usRWLen       = pReqMsgH->usRWLen;

    pReqMsgH = (CLIREQOPPARA *)SerRcvBuf.auchData;

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {
    case CLI_FCOPREQALL:
    case CLI_FCOPREQALLFIN:                 /* reset Broadcast Bit */
    case CLI_FCOPREQSUP:
    case CLI_FCOPREQSUPFIN:
    case CLI_FCOPREQPLU:
    case CLI_FCOPREQPLUFIN:
	case CLI_FCOPREQLAYFIN:					/* ESMITH LAYOUT */
        SerRecvOprPara();
        break;

    case CLI_FCOPPLUINDREAD:                /* Read,        Saratoga */
    case CLI_FCOPPLUASSIGN:                 /* Add,         Saratoga */
    case CLI_FCOPPLUUPDATE:                 /* Update an existing PLU */
    case CLI_FCOPPLUDELETE:                 /* Delete,      Saratoga */
    case CLI_FCPLUENTERCRITMD:              /* Enter Crit,  Saratoga */
    case CLI_FCPLUEXITCRITMD:               /* Exit Crit,   Saratoga */
    case CLI_FCOPPLUOEPREAD:
        SerRecvOprPlu();
        break;

	case CLI_FCOPREQLAY:					/* ESMITH LAYOUT */
		SerRmtFileServer();
		break;

	case CLI_FCOPJOINCLUSTER:          // process request from terminal to join cluster
		ResMsgH.sReturn = SerOpJoinCluster();
		break;

	case CLI_FCOPUNJOINCLUSTER:          // process request from terminal to remove from cluster
		ResMsgH.sReturn = SerOpUnjoinCluster();
		break;

	case CLI_FCOPTRANSFERTOTALS:         // process request from terminal to remove from cluster
		ResMsgH.sReturn = SerOpTransferTotals();
		break;

	case CLI_FCOPREQMSGBUFFER:             // process request from terminal display a message to the operator
		{
			CLIREQMSGBUFFER   *pReqMsgHBuffer = ((CLIREQMSGBUFFER *)SerRcvBuf.auchData);
			CLIRESMSGBUFFER   ResMsgHBuffer;
			SHORT             sTargetTerminal = SerResponseTargetTerminal();

#if 0
			// commented out.  to be used for debugging.
			{
				char  xBuff[128];
				sprintf (xBuff, " CLI_FCOPREQMSGBUFFER for terminal %d", sTargetTerminal);
				NHPOS_ASSERT_TEXT(0, xBuff);
			}
#endif
			if (sTargetTerminal > 0) {
				SerOpSetOperatorMessageStatusTable (sTargetTerminal, 0x0001);
			}

			memset(&ResMsgHBuffer, 0, sizeof(ResMsgHBuffer));
			ResMsgHBuffer.usFunCode = pReqMsgHBuffer->usFunCode;
			ResMsgHBuffer.usSeqNo   = pReqMsgHBuffer->usSeqNo & CLI_SEQ_CONT;
			ResMsgHBuffer.sResCode  = STUB_SUCCESS;
			ResMsgHBuffer.sReturn = SerOpRetrieveOperatorMessage(pReqMsgHBuffer, &ResMsgHBuffer);
			sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgHBuffer, sizeof(ResMsgHBuffer), NULL, 0);
		}
        break;  

	case CLI_FCOPUPDATETOTALSMSG:       // process request from terminal to update totals
		{
			CLIREQSFTOTALS   *pReqMsgH = ((CLIREQSFTOTALS *)SerRcvBuf.auchData);
			CLIRESSFTOTALS   ResMsgH;

			memset(&ResMsgH, 0, sizeof(ResMsgH));
			ResMsgH.usFunCode = pReqMsgH->usFunCode;
			ResMsgH.usSeqNo   = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
			ResMsgH.sResCode  = STUB_SUCCESS;
			ResMsgH.sReturn   = SerOpUpdateTotalsMsg(&(pReqMsgH->TtlMessage));
			sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(ResMsgH), NULL, 0);
		}
		break;

	case CLI_FCOPSIGNMASTER:
		break;

    case CLI_FCOPPLUREADBDP :   /* OP PLU  read by dept number, R3.0    */
        SerResp.pSavBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESOPPARA)];
        pReqBuff = (CLIREQDATA *)((UCHAR *)pReqMsgH + sizeof(CLIREQOPPARA));
        SerResp.pSavBuff->usDataLen = pReqBuff->usDataLen;
        pPlu = (PLUIF *)SerResp.pSavBuff->auchData; 
        memcpy(SerResp.pSavBuff->auchData, pReqBuff->auchData, pReqBuff->usDataLen);
        pPluIf_Dep = (PLUIF_DEP *)pPlu;
        if ((pPluIf_Dep->culCounter == 0) || ((fsIspLockedFC & ISP_LOCK_MASSMEMORY) == 0))  {  /* if locked previously */
            memset(&PluIf_Dep, 0, sizeof(PLUIF_DEP));
        }
        PluIf_Dep.culCounter = pPluIf_Dep->culCounter;
        PluIf_Dep.usDept = pPluIf_Dep->usDept;          /* Saratoga */

        if (pPluIf_Dep->culCounter == 0) {
            if (fsIspLockedFC & ISP_LOCK_MASSMEMORY) {  /* if locked previously */
                IspCleanUpLockMas();                    /* clean up MMM, saratoga */
            }
        }

        SerResp.pSavBuff->usDataLen = 0; /* */

        if ((ResMsgH.sReturn = SerOpDeptPluRead(&PluIf_Dep, husIspOpHand)) != OP_PERFORM) {
            if (ResMsgH.sReturn == OP_EOF) {
                //ResMsgH.sReturn = OP_PERFORM;
                fsIspLockedFC &= ~ISP_LOCK_MASSMEMORY;  /* saratoga */
            }
        } else {
            husIspMasHand  = PluIf_Dep.husHandle;       /* saratoga */
            fsIspLockedFC |= ISP_LOCK_MASSMEMORY;   /* Set MASSMEMORY */
        }
        memcpy(pPluIf_Dep++, &PluIf_Dep, sizeof(PLUIF_DEP));
        SerResp.pSavBuff->usDataLen += sizeof(PLUIF_DEP); /* */
        
		sSerSendStatus = SerSendMultiple((CLIRESCOM *)&ResMsgH, sizeof(CLIRESOPPARA));
		break;

    case CLI_FCOPPLUREADBRC :   /* OP PLU  read by report code, R3.0    */
        SerResp.pSavBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESOPPARA)];
        pReqBuff = (CLIREQDATA *)((UCHAR *)pReqMsgH + sizeof(CLIREQOPPARA));
        SerResp.pSavBuff->usDataLen = pReqBuff->usDataLen;
        pPlu = (PLUIF *)SerResp.pSavBuff->auchData; 
        memcpy(SerResp.pSavBuff->auchData, pReqBuff->auchData, pReqBuff->usDataLen);
        pPluIf_Rep = (PLUIF_REP *)pPlu;
        if ((pPluIf_Rep->culCounter == 0) || ((fsIspLockedFC & ISP_LOCK_MASSMEMORY) == 0))  {  /* if locked previously */
            memset(&PluIf_Rep, 0, sizeof(PLUIF_REP));
        }
        PluIf_Rep.culCounter = pPluIf_Rep->culCounter;
        PluIf_Rep.uchReport = pPluIf_Rep->uchReport;

        if (pPluIf_Rep->culCounter == 0) {
            if (fsIspLockedFC & ISP_LOCK_MASSMEMORY) {  /* if locked previously */
                IspCleanUpLockMas();                    /* clean up MMM, saratoga */
            }
        }

        SerResp.pSavBuff->usDataLen = 0; /* */

        if ((ResMsgH.sReturn = SerOpRprtPluRead(&PluIf_Rep, husIspOpHand)) != OP_PERFORM) {
            if (ResMsgH.sReturn == OP_EOF) {
                //ResMsgH.sReturn = OP_PERFORM;
                fsIspLockedFC &= ~ISP_LOCK_MASSMEMORY;  /* saratoga */
            }
        } else {
            husIspMasHand  = PluIf_Rep.husHandle;       /* saratoga */
            fsIspLockedFC |= ISP_LOCK_MASSMEMORY;   /* Set MASSMEMORY */
        }
        memcpy(pPluIf_Rep++, &PluIf_Rep, sizeof(PLUIF_REP));
        SerResp.pSavBuff->usDataLen += sizeof(PLUIF_REP); /* */
        
        sSerSendStatus = SerSendMultiple((CLIRESCOM *)&ResMsgH, sizeof(CLIRESOPPARA));
        break;

	case    CLI_FCCOPONNENGINEMSG:
		// This section processes a Connection Engine request message received from a
		// Satellite Terminal requesting that the Master Terminal use its Connection Engine
		// interface to send a message out and optionally to return a response.
		// This happens in one step that may be followed by a second step:
		//   - Start the request, this step may or may not result in a Connection Engine response
		//   - Wait for and return Connection Engine response, an optional step depending on the message
		{
			CLIREQOPCONNENGINE   *pReqMsgH = ((CLIREQOPCONNENGINE *)SerRcvBuf.auchData);
			CLIRESOPCONNENGINE   ResMsgH;
			CLIREQDATA           *pReqData;
			AllObjects           *pAllObjects;

			memset(&ResMsgH, 0, sizeof(ResMsgH));
			ResMsgH.usFunCode = pReqMsgH->usFunCode;
			ResMsgH.usSeqNo   = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
			ResMsgH.sResCode  = STUB_SUCCESS;
			pReqData = (CLIREQDATA *)(SerRcvBuf.auchData + sizeof(CLIREQOPCONNENGINE));
			pAllObjects = (AllObjects *)(pReqData->auchData);

			ResMsgH.sReturn = 0;
			sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(ResMsgH), NULL, 0);

			if (pAllObjects->ConnEngine.iObjectType == BL_CONNENGINE_MSG_PRINTFILE) {
				BlSubmitFileToFilePrint (pAllObjects->u.PrintFile.tcsFileName);
			}
			else if (pAllObjects->ConnEngine.iObjectType == BL_CONNENGINE_MSG_EMPLOYEECHANGE) {
				BlProcessEmployeeChange (&(pAllObjects->u.EmployeeChange));
			}
			else if (pAllObjects->ConnEngine.iObjectType == BL_CONNENGINE_MSG_GUESTCHECK_RTRV) {
				BlProcessGuestCheck_Rtrv (&(pAllObjects->u.GuestCheck_Rtrv));
			}
			else if (pAllObjects->ConnEngine.iObjectType == BL_CONNENGINE_MSG_CASHIERACTION) {
				BlProcessCashierAction (&(pAllObjects->u.CashierAction));
			}
			else if (pAllObjects->ConnEngine.iObjectType == BL_CONNENGINE_MSG_EJENTRYACTION) {
				BlProcessEjEntryAction (&(pAllObjects->u.EjEntryAction));
			}
			else if (pAllObjects->ConnEngine.iObjectType == BL_CONNENGINE_MSG_TRANSACTION) {
				BlProcessTransaction (&(pAllObjects->u.Transaction));
			}
			else if (pAllObjects->ConnEngine.iObjectType == BL_CONNENGINE_MSG_CONNENGINE) {
				BlProcessConnEngine (pAllObjects);
			}
			else {
				ConnEngineStartRequest(pReqMsgH->usTerminalNo, pAllObjects->u.TcharString.tcsArray);
			}
		}
        break;

    case    CLI_FCCOPONNENGINEMSGFH:
		{
			CLIREQOPCONNENGINE   *pReqMsgH = ((CLIREQOPCONNENGINE *)SerRcvBuf.auchData);
			CLIRESOPCONNENGINE   ResMsgH;

			memset(&ResMsgH, 0, sizeof(ResMsgH));
			ResMsgH.usFunCode = pReqMsgH->usFunCode;
			ResMsgH.usSeqNo   = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
			ResMsgH.sResCode  = STUB_SUCCESS;
			sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(ResMsgH), NULL, 0);

			if (pReqMsgH->iObjectType == BL_CONNENGINE_MSG_SENDFILE) {
				ResMsgH.sReturn = ConnEngineObjectSendFile(pReqMsgH->usTerminalNo, pReqMsgH->usBufferLengthBytes);
			} else {
				SHORT  sTempFileHandle;
				ULONG  ulActualBytesRead;
				USHORT usNoBytes = pReqMsgH->usBufferLengthBytes;
				TRANSACTION  Transaction;
				TCHAR  tcsBuff[2048];

				// Transfer the data in the temporary file created by the Connection Engine Interface
				// to the standard Server Temp file so that it can be processed properly.
				sTempFileHandle = PifOpenFile (_T("OBJTEMP"), "tnw");
				if (sTempFileHandle < 0) {
					PifDeleteFile (_T("OBJTEMP"));
					sTempFileHandle = PifOpenFile (_T("OBJTEMP"), "tnw");
					if (sTempFileHandle < 0) {
						char  xBuff[128];
						sprintf (xBuff, "CLI_FCCOPONNENGINEMSGFH OBJTEMP file open error %d", sTempFileHandle);
						NHPOS_ASSERT_TEXT((sTempFileHandle >= 0), xBuff);
						break;
					}
				}
				PifSeekFile (hsSerTmpFile, SERTMPFILE_DATASTART, &ulActualBytesRead);
				do {
					PifReadFile (hsSerTmpFile, tcsBuff, sizeof(tcsBuff), &ulActualBytesRead);
					if (usNoBytes < ulActualBytesRead)
						ulActualBytesRead = usNoBytes;
					PifWriteFile (sTempFileHandle, tcsBuff, ulActualBytesRead);
					usNoBytes -= ulActualBytesRead;
				} while (usNoBytes > 0);
				PifCloseFile (sTempFileHandle);
				memset (&Transaction, 0, sizeof(Transaction));
				memcpy (Transaction.uchUniqueIdentifier, pReqMsgH->uchUniqueIdentifier, sizeof(Transaction.uchUniqueIdentifier));
				Transaction.lErrorCode = pReqMsgH->lErrorCode;
				BlProcessTransaction (&Transaction);
			}
		}
        break;

    default:
		// Issue a PifLog() to record that we have an error condition then
		// return an error to the sender.  This is a bad function code.
		PifLog (MODULE_SER_LOG, LOG_EVENT_SER_OPR_BAD_FUNCODE);
		return;
        break;
    }

	if (sSerSendStatus < 0) {
		char xBuff [128];
		sprintf (xBuff, "SerRecvOpr(): sSerSendStatus = %d, usFunCode = 0x%x, usSeqNo = 0x%x, sReturn = %d", sSerSendStatus, ResMsgH.usFunCode, ResMsgH.usSeqNo, ResMsgH.sReturn);
		NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
	}
}

/*
*===========================================================================
** Synopsis:    VOID    SerRecvOprPlu(VOID)
*
** Return:      none.
*
** Description: This function executes for request PLU.             Saratoga
*===========================================================================
*/
VOID    SerRecvOprPlu(VOID)
{
    CLIREQSCNPLU    *pReqMsgH;
    CLIRESSCNPLU    ResMsgH;
    CLIREQMNTPLU    *pReqMsgM;
    CLIRESMNTPLU    ResMsgM;
    PLUIF           Plu;
    CLIREQOEPPLU    *pReqMsg;
    CLIRESOEPPLU    ResMsg;
	SHORT           sSerSendStatus = 0;

    memset(&Plu, 0, sizeof(Plu));
    pReqMsgH = (CLIREQSCNPLU *)SerRcvBuf.auchData;
    pReqMsgM = (CLIREQMNTPLU *)SerRcvBuf.auchData;
    pReqMsg  = (CLIREQOEPPLU *)SerRcvBuf.auchData;

    memset(&ResMsgH, 0, sizeof(CLIRESSCNPLU));
    memset(&ResMsgM, 0, sizeof(CLIRESMNTPLU));
    ResMsgH.usFunCode = pReqMsgH->usFunCode;
    ResMsgM.usFunCode = pReqMsgM->usFunCode;
    ResMsgH.usSeqNo   = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgM.usSeqNo   = pReqMsgM->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.sResCode  = STUB_SUCCESS;
    ResMsgM.sResCode  = STUB_SUCCESS;

    memset(&ResMsg, 0, sizeof(CLIRESOEPPLU));
    ResMsg.usFunCode = pReqMsg->usFunCode;
    ResMsg.usSeqNo   = pReqMsg->usSeqNo & CLI_SEQ_CONT;
    ResMsg.sResCode  = STUB_SUCCESS;

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {
    case CLI_FCOPPLUINDREAD:                /* Read,        Saratoga */
        memcpy(Plu.auchPluNo, pReqMsgM->aucPluNumber, sizeof(Plu.auchPluNo));
        Plu.uchPluAdj = pReqMsgM->uchAdjective;
        ResMsgH.sReturn = OpPluIndRead(&Plu, 0);

		SerAssignPluifToCliresscnplu (&ResMsgH, &Plu);

        sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESSCNPLU), NULL, 0);
        break;

    case CLI_FCOPPLUUPDATE:                 /* Update a PLU,         Saratoga */
    case CLI_FCOPPLUASSIGN:                 /* Add,         Saratoga */
		SerAssignClireqmntpluToPluif (&Plu, pReqMsgM);
        ResMsgM.sReturn = OpPluAssign(&Plu, 0);

        sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgM, sizeof(CLIRESMNTPLU), NULL, 0);
        break;

    case CLI_FCOPPLUDELETE:                 /* Delete,      Saratoga */
        memcpy(Plu.auchPluNo, pReqMsgM->aucPluNumber, sizeof(Plu.auchPluNo));
        Plu.uchPluAdj               = pReqMsgM->uchAdjective;
        ResMsgM.sReturn = OpPluDelete(&Plu, 0);

        sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgM, sizeof(CLIRESMNTPLU), NULL, 0);
        break;

    case CLI_FCOPPLUOEPREAD:                /* Read,        Saratoga */
        ResMsgM.sReturn = OpPluOepRead((OPPLUOEPPLUNO *)pReqMsg->auchPluNo, (OPPLUOEPPLUNAME *)ResMsg.auchPluName, 0);

        sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsg, sizeof(CLIRESOEPPLU), NULL, 0);
        break;

    default:
       // Issue a PifLog() to record that we have an error condition then
       // return an error to the sender.  This is a bad function code.
       PifLog (MODULE_SER_LOG, LOG_EVENT_SER_OPR_BAD_FUNCODE);
       return;
       break;
    }

	if (sSerSendStatus < 0) {
		char xBuff[128];
		sprintf(xBuff, "SerRecvOprPlu(): sSerSendStatus = %d, 0x%x, usFunCode 0x%x, usSeqNo 0x%x, sReturn %d", sSerSendStatus, *((ULONG *)&SerSndBuf.auchFaddr[0]), pReqMsgH->usFunCode, pReqMsgH->usSeqNo, ResMsgM.sReturn);
		NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
	}
}


/*
*===========================================================================
** Synopsis:    VOID    SerRecvOpr(VOID);
*
** Return:      none.
*
** Description: This function executes for request operational parameter.
*===========================================================================
*/
VOID    SerRecvOprPara(VOID)
{
    CLIREQOPPARA    *pReqMsgH;
    CLIRESOPPARA    ResMsgH;
    CLIREQDATA      *pSavBuff, *pRcvBuff;
    USHORT          usDataLen, fsBcasInfUA;
    SERINQSTATUS    InqData;
    CLIOPFINBLK     *pFinRecv, *pFinSend;
    USHORT          usFunCode;
    UCHAR           uchStatus;
    DATE_TIME       D_T;
	SHORT           sSerSendStatus;


    pReqMsgH = (CLIREQOPPARA *)SerRcvBuf.auchData;
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));
    ResMsgH.usFunCode = pReqMsgH->usFunCode;
    ResMsgH.usSeqNo   = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.sResCode  = STUB_MULTI_SEND;

    usDataLen = SER_MAX_TMPBUF - sizeof(CLIRESOPPARA) - 2;
    pSavBuff = (CLIREQDATA *)&SerRcvBuf.auchData[sizeof(CLIREQOPPARA)];
    pRcvBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESOPPARA)];
    pFinSend = (CLIOPFINBLK *)pRcvBuff->auchData;
    pFinRecv = (CLIOPFINBLK *)pSavBuff->auchData;
    
    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {
    case    CLI_FCOPREQALL:
        ResMsgH.sReturn = OpResAll(pSavBuff->auchData, pSavBuff->usDataLen, pRcvBuff->auchData, &usDataLen, 0);
        if (OP_PERFORM != ResMsgH.sReturn) {
            usDataLen = 0;
        }
        usFunCode = CLI_FCOPREQALL;
        uchStatus = 2;                // indicates still in the process of downloading parameters
        break;

    case    CLI_FCOPREQALLFIN:              /* reset Broadcast Bit */
		// A terminal reports that it has completed downloading parameters by sending this message.
		// We will now update our broadcast reporting structure used by AC75 for its report.
        fsBcasInfUA = (1 << (SerRcvBuf.auchFaddr[CLI_POS_UA]-1));
        SstReadInqStat(&InqData);
        InqData.fsBcasInf &= ~fsBcasInfUA;
        SstChangeInqBcasInf(InqData.fsBcasInf);

        pFinSend->usRegNo = pFinRecv->usRegNo;
        PifGetDateTime(&D_T);
        SerConvertDateTime(&D_T, &pFinSend->DateTime);
        usDataLen = sizeof(CLIOPFINBLK);
        ResMsgH.sReturn = 0;

        usFunCode = CLI_FCOPREQALL;
        uchStatus = 1;                // indicates completed the process of downloading parameters
        break;
        
    case    CLI_FCOPREQSUP:
        ResMsgH.sReturn = OpResSup(pSavBuff->auchData, pSavBuff->usDataLen, pRcvBuff->auchData, &usDataLen, 0);
        if (OP_PERFORM != ResMsgH.sReturn) {
            usDataLen = 0;
        } 
        usFunCode = CLI_FCOPREQSUP;
        uchStatus = 2;                // indicates still in the process of downloading parameters
        break;

    case    CLI_FCOPREQSUPFIN:
		// A terminal reports that it has completed downloading parameters by sending this message.
		// We will now update our broadcast reporting structure used by AC75 for its report.
        pFinSend->usRegNo = pFinRecv->usRegNo;
        PifGetDateTime(&D_T);
        SerConvertDateTime(&D_T, &pFinSend->DateTime);
        usDataLen = sizeof(CLIOPFINBLK);
        ResMsgH.sReturn = 0;

        usFunCode = CLI_FCOPREQSUP;
        uchStatus = 1;                // indicates completed the process of downloading parameters
        break;

#if 0
    case    CLI_FCOPREQPLU:
        ResMsgH.sReturn = OpResPlu(pSavBuff->auchData, pSavBuff->usDataLen, pRcvBuff->auchData, &usDataLen, 0);
        if (OP_PERFORM != ResMsgH.sReturn) {
            usDataLen = 0;
        } 
        usFunCode = CLI_FCOPREQPLU;
        uchStatus = 2;
        break;

    case    CLI_FCOPREQPLUFIN:
        pFinSend->usRegNo = pFinRecv->usRegNo;
        usDataLen = sizeof(CLIOPFINBLK);
        ResMsgH.sReturn = 0;

        usFunCode = CLI_FCOPREQPLU;
        uchStatus = 1;
        break;
#endif
	case	CLI_FCOPREQLAYFIN: //ESMITH LAYOUT
        fsBcasInfUA = (1 << (SerRcvBuf.auchFaddr[CLI_POS_UA]-1));
        SstReadInqStat(&InqData);
        InqData.fsBcasInf &= ~fsBcasInfUA;
        SstChangeInqBcasInf(InqData.fsBcasInf);

        pFinSend->usRegNo = pFinRecv->usRegNo;
        usDataLen = sizeof(CLIOPFINBLK);
        ResMsgH.sReturn = 0;

        usFunCode = CLI_FCOPREQLAY;
        uchStatus = 1;
        break;

	case CLI_FCOPJOINCLUSTER:          // process request from terminal to join cluster
		ResMsgH.sReturn = SerOpJoinCluster();
        usDataLen = sizeof(CLIRESJOIN);
        usFunCode = CLI_FCOPJOINCLUSTER;
        uchStatus = 1;
        break;  

	case CLI_FCOPUNJOINCLUSTER:          // process request from terminal to join cluster
		ResMsgH.sReturn = SerOpUnjoinCluster();
        usDataLen = sizeof(CLIRESJOIN);
        usFunCode = CLI_FCOPUNJOINCLUSTER;
        uchStatus = 1;
        break;  

	case CLI_FCOPREQDISPMSG:             // process request from terminal display a message to the operator
		ResMsgH.sReturn = SerOpDisplayOperatorMessage((CLIREQMSGDISP *)SerRcvBuf.auchData, (CLIRESMSGDISP *)&ResMsgH);
        usDataLen = sizeof(CLIRESMSGDISP);
        usFunCode = CLI_FCOPREQDISPMSG;
        uchStatus = 1;
        break;  

	case CLI_FCOPTRANSFERTOTALS:         // process request from terminal to join cluster
		ResMsgH.sReturn = SerOpTransferTotals();
        usDataLen = sizeof(CLIRESTRANSFER);
        usFunCode = CLI_FCOPTRANSFERTOTALS;
        uchStatus = 1;
        break;  

	case CLI_FCOPUPDATETOTALSMSG:       // process request from terminal to update totals
		{
			CLIREQSFTOTALS   *pReqMsgH = ((CLIREQSFTOTALS *)SerRcvBuf.auchData);
			CLIRESSFTOTALS   ResMsgH;

			memset(&ResMsgH, 0, sizeof(ResMsgH));
			ResMsgH.usFunCode = pReqMsgH->usFunCode;
			ResMsgH.usSeqNo   = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
			ResMsgH.sResCode  = STUB_SUCCESS;
			ResMsgH.sReturn   = SerOpUpdateTotalsMsg(&(pReqMsgH->TtlMessage));
			usDataLen = sizeof(CLIRESSFTOTALS);
			usFunCode = CLI_FCOPUPDATETOTALSMSG;
			uchStatus = 1;
		}
		break;

    default:                                    /* not used */
        // Issue a PifLog() to record that we have an error condition then
        // return an error to the sender.  This is a bad function code.
        PifLog (MODULE_SER_LOG, LOG_EVENT_SER_OPR_BAD_FUNCODE);
        return;
        break;
    }

    SerResp.pSavBuff = pRcvBuff;
    SerResp.pSavBuff->usDataLen = usDataLen;
    sSerSendStatus = SerSendMultiple((CLIRESCOM *)&ResMsgH, sizeof(CLIRESOPPARA));
	if (sSerSendStatus < 0) {
		char xBuff [128];
		sprintf (xBuff, "SerRecvOprPara(): sSerSendStatus = %d, usFunCode = 0x%x, usSeqNo = 0x%x", sSerSendStatus, ResMsgH.usFunCode, ResMsgH.usSeqNo);
		NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
	}

	// Indicate that we have responded to a parameter request from a Satellite Terminal
	// that was most likely triggered by an AC75.  The AC75 on a Master Terminal causes the
	// Master Terminal to broadcast through Notice Board message that a new parameter set
	// is available and it then waits for requests from the Satellite Terminals for the new
	// parameters.  As each terminal requests its parameters, we indicate which terminals have
	// requested a parameter download for the AC75 report printed.
	//
	// A uchStatus of 1 indicates completed download, uchStatus of 2 indicates still downloading.
    SstSetBcasRegNo(SerRcvBuf.auchFaddr[CLI_POS_UA], pFinRecv->usRegNo, usFunCode, uchStatus);
    SerBcasTimerStart();
}

/*
*===========================================================================
** Synopsis:    VOID    SerConvertDateTime(DATE_TIME *pSource, 
*                                          CLIDATETIME *pDestine);
*
*     Input:    pSource     - source date/ time
*               pDestine    - destination date/ time
*    
** Return:      none.
*
** Description:
*   This function converts DATE_TIME structure to CLIDATETIME structure
*   which is defined in PIF.
*
*   The Client side has a reverse function CstConvertDateTime() which makes
*   the conversion from CLIDATETIME structure to DATE_TIME structure.
*===========================================================================
*/
VOID    SerConvertDateTime(DATE_TIME *pSource, CLIDATETIME *pDestine)
{
    pDestine->usSec     = pSource->usSec;    
    pDestine->usMin     = pSource->usMin;    
    pDestine->usHour    = pSource->usHour;    
    pDestine->usMDay    = pSource->usMDay;    
    pDestine->usMonth   = pSource->usMonth;    
    pDestine->usYear    = pSource->usYear;
    pDestine->usWDay    = pSource->usWDay;
}

/*===== END OF SOURCE =====*/