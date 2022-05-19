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
* Title       : Client/Server STUB, Operational Parameter Source File
* Category    : Client/Server STUB, US Hospitality Model
* Program Name: CSSTBOPR.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*           CliReqOpPara()          Request read operational parameter
*           CliOpPluIndRead()       Read individual PLU record
*           CliOpDeptIndRead()      Read individual Dept record
*           CliOpCpnIndRead()       Read individual Coupon record   R3.0
*           CliOpCstrIndRead()      Read individual Contorl Storing record    R3.0 
*           CliOpOepPluRead()       Read Oep PLU record             R3.0 
*           CliOpPpiIndRead()       Read Promotional Pricing Item   R3.1
*
*           SerChangeOpPara()       Change parameter, inform to NB
*           SerOpDeptAssign()       Assign Dept record
*           CliOpPluAssign()        Assign PLU record
*           SerOpCpnAssign()        Assign Coupon record            R3.0
*           SerOpCstrAssign()       Assign control string record    R3.0
*           SerOpPpiAssign()        Assign Promotional Pricing Item R3.1
*           SerOpPpiDelete()        Delete Promotional Pricing Item R3.1
*           CliOpPluDelete()        Delete PLU record
*           SerOpCstrDelete()       Delete control string record    R3.0
*           SerOpPluFileUpdate()    Update PLU (index) file
*           SerOpPluAllRead()       Read All PLU record
*           SerOpRprtPluRead()      Read PLU record by report code
*           SerOpDeptPluRead()      Read PLU record by dept number
*           SerOpLock()             Lock Record
*           SerOpUnLock()           UnLock Record
*           SerOpMajorDeptRead()    Read specified major dept parameter
*
*           CstConvertDateTime()    convert date/ time structure
* --------------------------------------------------------------------------
* Update Histories
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial
* Feb-28-95:hkato      : R3.0
* Feb-01-96:T.Nakahata : R3.1 Initial (Add Promotional Pricing Item File)
* Aug-17-98:M.Ozawa    : Add Mld Mnemonics File
* 
*** NCR2171 **
* Aug-26-99:M.Teraki   : initial (for Win32)
* Dec-20-99:hrkato     : Saratoga
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2  R.Chambers   Source cleanup to remove old, before NCR 7448,
*                                   code that applied to NCR 2170 back in the 1990s.
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
#include	<string.h>
#include    <memory.h>
#include    <math.h>
#include    <stdio.h>

#include    "ecr.h"
#include    "pif.h"
#include    "plu.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csop.h"
#include    "nb.h"
#include    "csstbpar.h"
#include    "csstbopr.h"
#include    "csstbfcc.h"
#include    "csstbstb.h"
#include    "csstbbak.h"
#include    "csserm.h"
#include    "rfl.h"
#include    "csstubin.h"
#include	"blfwif.h"
#include	"appllog.h"

#include    "servmin.h"
#include    "ispcom.h"
#include    "csttl.h"
#include    "bl.h"
#include    "ttl.h"

extern XGRAM       CliSndBuf;          /* Send Buffer */    
extern XGRAM       CliRcvBuf;          /* Receive Buffer */
extern  CLICOMIF   CliMsg;

//----------------   Helper functions used for assignment between different variable types
//
static void CliAssignCliresscnpluToPluif (PLUIF *pPlu, CLIRESSCNPLU *pResMsgH)
{
    _tcsncpy(pPlu->auchPluNo, pResMsgH->aucPluNumber, STD_PLU_NUMBER_LEN);
    pPlu->uchPluAdj             = pResMsgH->uchAdjective;
    pPlu->ParaPlu.ContPlu.usDept= pResMsgH->usDeptNumber;
    pPlu->ParaPlu.ContPlu.uchItemType = pResMsgH->uchItemType;
    RflConv4bto3b(pPlu->ParaPlu.auchPrice, pResMsgH->ulUnitPrice);
    pPlu->ParaPlu.uchPriceMulWeight         = pResMsgH->uchPriceMulWeight;
    _tcsncpy(pPlu->ParaPlu.auchPluName, pResMsgH->aucMnemonic, STD_PLU_MNEMONIC_LEN);
    _tcsncpy(pPlu->ParaPlu.auchAltPluName, pResMsgH->auchAltPluName, STD_PLU_MNEMONIC_LEN);
    memcpy(pPlu->ParaPlu.ContPlu.auchContOther, pResMsgH->aucStatus, sizeof(pPlu->ParaPlu.ContPlu.auchContOther));
    pPlu->ParaPlu.ContPlu.uchRept   = pResMsgH->uchReportCode;
    pPlu->ParaPlu.usLinkNo          = pResMsgH->usLinkNumber;
    pPlu->ParaPlu.uchRestrict       = pResMsgH->uchRestrict;
    pPlu->ParaPlu.uchTableNumber    = pResMsgH->uchTableNumber;
    pPlu->ParaPlu.uchGroupNumber    = pResMsgH->uchGroupNumber;
    pPlu->ParaPlu.uchPrintPriority  = pResMsgH->uchPrintPriority;
    pPlu->ParaPlu.usFamilyCode      = pResMsgH->usFamilyCode;
    pPlu->ParaPlu.uchLinkPPI        = pResMsgH->uchLinkPPI;
    memcpy(pPlu->ParaPlu.auchAltPluName, pResMsgH->auchAltPluName, sizeof(pPlu->ParaPlu.auchAltPluName));
	pPlu->ParaPlu.uchColorPaletteCode = pResMsgH->uchColorPaletteCode;
	pPlu->ParaPlu.ContPlu.usBonusIndex  = pResMsgH->usBonusIndex;
	pPlu->ParaPlu.usTareInformation   = pResMsgH->usTareInformation;
	memcpy (pPlu->ParaPlu.uchExtendedGroupPriority, pResMsgH->uchExtendedGroupPriority, sizeof(pPlu->ParaPlu.uchExtendedGroupPriority));
	memcpy (pPlu->ParaPlu.uchPluOptionFlags, pResMsgH->uchPluOptionFlags, sizeof(pPlu->ParaPlu.uchPluOptionFlags));
}


static void CliAssignPluifToClireqmntplu(CLIREQMNTPLU *pReqMsgH, PLUIF *pPlu)
{
    _tcsncpy(pReqMsgH->aucPluNumber, pPlu->auchPluNo, STD_PLU_NUMBER_LEN);
    pReqMsgH->uchAdjective        = pPlu->uchPluAdj;
    pReqMsgH->usDeptNumber        = pPlu->ParaPlu.ContPlu.usDept;
    pReqMsgH->uchItemType         = pPlu->ParaPlu.ContPlu.uchItemType;
    pReqMsgH->ulUnitPrice         = RflFunc3bto4b(pPlu->ParaPlu.auchPrice);
    pReqMsgH->uchPriceMulWeight     = pPlu->ParaPlu.uchPriceMulWeight;
    _tcsncpy(pReqMsgH->aucMnemonic, pPlu->ParaPlu.auchPluName, STD_PLU_MNEMONIC_LEN);
    _tcsncpy(pReqMsgH->auchAltPluName, pPlu->ParaPlu.auchAltPluName, STD_PLU_MNEMONIC_LEN);
    memcpy(pReqMsgH->aucStatus, pPlu->ParaPlu.ContPlu.auchContOther, sizeof(pReqMsgH->aucStatus));
    pReqMsgH->uchReportCode       = pPlu->ParaPlu.ContPlu.uchRept;
    pReqMsgH->usLinkNumber        = pPlu->ParaPlu.usLinkNo;
    pReqMsgH->uchRestrict         = pPlu->ParaPlu.uchRestrict;
    pReqMsgH->uchTableNumber      = pPlu->ParaPlu.uchTableNumber;
    pReqMsgH->uchGroupNumber      = pPlu->ParaPlu.uchGroupNumber;
    pReqMsgH->uchPrintPriority    = pPlu->ParaPlu.uchPrintPriority;
    pReqMsgH->usFamilyCode        = pPlu->ParaPlu.usFamilyCode;
    pReqMsgH->uchLinkPPI          = pPlu->ParaPlu.uchLinkPPI;
    memcpy(pReqMsgH->auchAltPluName, pPlu->ParaPlu.auchAltPluName, sizeof(pReqMsgH->auchAltPluName));
	pReqMsgH->uchColorPaletteCode = pPlu->ParaPlu.uchColorPaletteCode;
	pReqMsgH->usBonusIndex        = pPlu->ParaPlu.ContPlu.usBonusIndex;
	pReqMsgH->usTareInformation   = pPlu->ParaPlu.usTareInformation;
	memcpy (pReqMsgH->uchExtendedGroupPriority, pPlu->ParaPlu.uchExtendedGroupPriority, sizeof(pReqMsgH->uchExtendedGroupPriority));
	memcpy (pReqMsgH->uchPluOptionFlags, pPlu->ParaPlu.uchPluOptionFlags, sizeof(pReqMsgH->uchPluOptionFlags));
}
//----------------------------------------------------------------------------------



/*
*===========================================================================
** Synopsis:    SHORT   CliReqSystemStatus(USHORT *pusCstComReadStatus);
*     InPut:    
*
** Return:      OP_PERFORM:   read OK
*               other:        error
*
** Description: This function queries the Master Terminal's system status to
*               determine if the Master Terminal is on line.  It's major use
*               is in the Dynamic PLU button functionality to check if the
*               Master Terminal is on-line despite the Master Off-line indicator
*               being set.  The Master Off-line indicator may be set if the
*               Satellite Terminal is on a different sub-net of a LAN and any
*               Notice Board messages sent out by the Master Terminal are not
*               seen by the Satellite Terminal because they are blocked by
*               network gear.
*===========================================================================
*/
SHORT  CliReqSystemStatus(USHORT *pusCstComReadStatus)
{
    CLIREQSYSTEMSTATUS   ReqMsgH = {0};
	CLIRESSYSTEMSTATUS   ResMsgH = {0};
    SHORT           sError;
    SHORT           sErrorM = STUB_SUCCESS;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOSYSTEMSTATUS;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQSYSTEMSTATUS);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESSYSTEMSTATUS);

    sErrorM = CstSendMaster();
	sError = CliMsg.sRetCode;

	if (pusCstComReadStatus) {
		*pusCstComReadStatus = CstComReadStatus();    // read the Notice Board system status for CstSendMaster()
	}

    PifReleaseSem(husCliExeNet);

    return sError;
}

/*
*===========================================================================
** Synopsis:    SHORT   CliReqOpPara(USHORT usType);
*     InPut:    usType  - 1:All 2:Super 3:PLU
*
** Return:      OP_PERFORM:   read OK
*               other:        error
*
** Description: This function supports to receive Parameter from Master.
*===========================================================================
*/
SHORT   CliReqOpPara(USHORT usType)
{
	USHORT          usLockHnd = 0;
    SHORT           sRetCode;
    USHORT          usLen=0;
    USHORT          usFunCode;
    PARASTOREGNO    ParaRegNo;
    CLIOPFINBLK     FinBlock;
    DATE_TIME       D_T;

	if ( (usLockHnd = OpLock()) == OP_LOCK) {
		NHPOS_ASSERT_TEXT(!OP_LOCK, "==WARNING: CliReqOpPara(): OpLock() is OP_LOCK");
		return OP_LOCK;
	}

    PifRequestSem(husCliExeNet);

    switch(usType) {
    case    CLI_LOAD_ALL:                  // request all data files and parameters
        sRetCode = OpReqAll(usLockHnd);
        usFunCode = CLI_FCOPREQALLFIN;     // indicate we have completed downloading parameters
        break;

    case    CLI_LOAD_SUP:                  // request the Satellite type data files and parameters
        sRetCode = OpReqSup(usLockHnd);
        usFunCode = CLI_FCOPREQSUPFIN;     // indicate we have completed downloading parameters
        break;
        
    case    CLI_LOAD_PLU:                       /* PLU should be read */
        sRetCode = OpReqPluOep(CLI_FCOPREQSUP, usLockHnd);
        usFunCode = CLI_FCOPREQSUPFIN;
        break;

	case	CLI_LOAD_LAY:						/* Layout should be read */
		sRetCode  = OpReqLay(usLockHnd);
		BlFwIfReloadLayout2();					/* Reload Layout */
		usFunCode = CLI_FCOPREQLAYFIN;
		break;

	case	CLI_LOAD_PRINTER_LOGOS:						/* transfer printer logo files by requesting them */
		sRetCode  = OpReqPrinterLogos(usLockHnd);
		usFunCode = CLI_FCOPREQLAYFIN;
		break;

	case	CLI_LOAD_ADDED_PARAM:						/* additional PARAM files (PARAMINI, etc.) should be read */
		sRetCode  = OpReqAdditionalParams(usLockHnd);
		usFunCode = CLI_FCOPREQLAYFIN;
		break;

    default:                                    /* not use */
//		OpUnLock();
		PifReleaseSem(husCliExeNet);
        return OP_PERFORM;
        break;                                  
    }

    if (OP_PERFORM == sRetCode ){
		// Send the final message indicating that we have completed our download.
		// This message will update the datas tructure used for the AC75 report.
		// If this is not sent then the AC75 broadcast report will show this terminal as still
		// incomplete with a uchBcasStatus of 2 rather than complete as a uchBcasStatus of 1.
		// The AC75 will still finish when the broadcast timer counts down even if this is not sent.
        ParaRegNo.uchMajorClass = CLASS_PARASTOREGNO;
        ParaRegNo.uchAddress = 1;
        CliParaRead(&ParaRegNo);

        FinBlock.usRegNo = ParaRegNo.usRegisterNo;
        usLen = sizeof(CLIOPFINBLK);

        CstReqOpePara(usFunCode, (UCHAR *)&FinBlock, sizeof(CLIOPFINBLK), (UCHAR *)&FinBlock, &usLen);

        if ((CLI_LOAD_ALL == usType) || (CLI_LOAD_SUP == usType)) {
            CstConvertDateTime(&FinBlock.DateTime, &D_T);
            PifSetDateTime(&D_T);
        }
	} else {
		char xBuff[128];

		sprintf (xBuff, "CliReqOpPara(): usType = %d, sRetCode %d", usType, sRetCode);
		NHPOS_ASSERT_TEXT((OP_PERFORM == sRetCode), xBuff);
	}

	OpUnLock();
    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    VOID    CstConvertDateTime(CLIDATETIME *pSource, 
*                                          DATE_TIME *pDestine);
*
*     Input:    pSource     - source date/ time
*               pDestine    - destination date/ time
*    
** Return:      none.
*
** Description:
*   This function converts CLIDATETIME structure to DATE_TIME structure
*   which is defined in PIF.
*
*   The Server side has a reverse function SerConvertDateTime() which makes
*   the conversion from DATE_TIME structure to CLIDATETIME structure.
*===========================================================================
*/
VOID    CstConvertDateTime(CLIDATETIME *pSource, DATE_TIME *pDestine)
{
    pDestine->usSec     = pSource->usSec;
    pDestine->usMin     = pSource->usMin;
    pDestine->usHour    = pSource->usHour;
    pDestine->usMDay    = pSource->usMDay;
    pDestine->usMonth   = pSource->usMonth;    
    pDestine->usYear    = pSource->usYear;
    pDestine->usWDay    = pSource->usWDay;
}


SHORT   OpUnjoinCluster (TCHAR *pwszHostName, UCHAR *pauchHostIpAddress, UCHAR *pauchTerminalNo)
{
	SHORT sRetStatus;

	sRetStatus = PifNetRemoveJoin (pwszHostName, pauchHostIpAddress, pauchTerminalNo);

	if (sRetStatus == PIF_OK)
		sRetStatus = OP_PERFORM;
	else
		sRetStatus = OP_FILE_FULL;

	return sRetStatus;
}

/*
*===========================================================================
** Synopsis:    SHORT   CliOpUnjoinCluster(TCHAR *pwszHostName,
*                                          UCHAR *pauchHostIpAddress,
*                                          UCHAR *pauchTerminalNo);
*
*     Input:    pwszHostName        - Computer Name, name of Disconnected Satellite host
*               pauchHostIpAddress  - IP address of this Disconnected Satellite host
*               pauchTerminalNo     - terminal number or zero if lookup against IP address
*    
** Return:      none.
*
** Description:
*===========================================================================
*/
SHORT   CliOpConnEngine (const TCHAR *pwszConnEngineRequest, LONG tcBufferSize)
{
	AllObjects            *pAllObjects;
	UCHAR                 ReqMsgBuffer[sizeof(CLIREQOPCONNENGINE)];
	UCHAR                 ResMsgBuffer[sizeof(CLIRESOPCONNENGINE)];
	TCHAR                 auchSndBuffer[400];
    CLIREQOPCONNENGINE    *pReqMsgH;
    CLIRESOPCONNENGINE    *pResMsgH;
    SHORT                 sMoved=0, sError, sRetCode, sErrorBM = 0;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg,  0, sizeof(CLICOMIF));
    memset(&ReqMsgBuffer, 0, sizeof(CLIREQOPCONNENGINE));
    memset(&ResMsgBuffer, 0, sizeof(CLIRESOPCONNENGINE));
    memset(&auchSndBuffer, 0, sizeof(auchSndBuffer));

    pReqMsgH = (CLIREQOPCONNENGINE *)ReqMsgBuffer;
    pResMsgH = (CLIRESOPCONNENGINE *)ResMsgBuffer;

	pReqMsgH->usTerminalNo = (UCHAR) PIF_CLUSTER_MASK_TERMINAL_NO(PifSysConfig()->usTerminalPositionFromName);
	pResMsgH->usTerminalNo = pReqMsgH->usTerminalNo;

    CliMsg.usFunCode    = CLI_FCCOPONNENGINEMSG;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgBuffer;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPCONNENGINE);
    CliMsg.pauchReqData = (UCHAR *)auchSndBuffer;

    pAllObjects = (AllObjects *)auchSndBuffer;
	pAllObjects->ConnEngine.iObjectType = BL_CONNENGINE_MSG_TCHARSTRING;
	if (tcBufferSize > TCHARSTRINGARRAYSIZE) {
		NHPOS_ASSERT_TEXT((tcBufferSize <= TCHARSTRINGARRAYSIZE), "**ERROR - CliOpConnEngine() String too long. Truncated.");
		tcBufferSize = TCHARSTRINGARRAYSIZE;
	}
	pAllObjects->u.TcharString.usLength = tcBufferSize;
	_tcsncpy (pAllObjects->u.TcharString.tcsArray, pwszConnEngineRequest, tcBufferSize);

	// Following size calculation assumes AllObjects.u.TcharString.tcsArray has a size of 1 element
	// this means that the request length we are specifying should be including the zero terminator.
	CliMsg.usReqLen     = ((UCHAR *)&(pAllObjects->u) - (UCHAR *)pAllObjects) + sizeof(pAllObjects->u.TcharString) + (tcBufferSize * sizeof(TCHAR));

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgBuffer;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPCONNENGINE);
    CliMsg.sRetCode     = OP_COMM_ERROR;

	sRetCode = CliMsg.sRetCode = OP_PERFORM;
    sError = CstSendMaster();                   /* --- Send Master Terminal --- */

#if 0
	// code needed for a Backup Master system to keep Backup Master updated.
	// this code appears to create a problem in a Backup Master system however.
	// further work to be done.
	if ((sError == STUB_M_DOWN) || (OP_PERFORM == CliMsg.sRetCode)) {
        if (STUB_SELF == (sErrorBM = CstSendBMaster())) {
			sError = STUB_SELF;
		}
	}
#endif

    PifReleaseSem(husCliExeNet);

    return sError;
}

SHORT   CliOpConnEngineObject (AllObjects  *pAllObjects)
{
	CLIREQOPCONNENGINE    ReqMsgBuffer = {0};
	CLIRESOPCONNENGINE    ResMsgBuffer = {0};
    SHORT                 sMoved=0, sError, sRetCode;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg,  0, sizeof(CLICOMIF));

	ResMsgBuffer.usTerminalNo = 0;
	ReqMsgBuffer.iObjectType = pAllObjects->ConnEngine.iObjectType;
	ReqMsgBuffer.usTerminalNo = pAllObjects->ConnEngine.sTerminalNumber;
	if (pAllObjects->ConnEngine.ulFlags & BL_CONNENGINE_FLAGS_UNIQUE_ID) {
		memcpy (ReqMsgBuffer.uchUniqueIdentifier, pAllObjects->u.Transaction.uchUniqueIdentifier, sizeof(ReqMsgBuffer.uchUniqueIdentifier));
		ReqMsgBuffer.lErrorCode = pAllObjects->u.Transaction.lErrorCode;
	}

	CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgBuffer;
	CliMsg.usReqMsgHLen = sizeof(CLIREQOPCONNENGINE);

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgBuffer;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPCONNENGINE);
    CliMsg.sRetCode     = OP_COMM_ERROR;

	sRetCode = CliMsg.sRetCode = OP_PERFORM;
	if (pAllObjects->ConnEngine.tcsTempFileName[0]) {
		SHORT  sFileHandle = -1;
		// need to do a file transfer for this
		sFileHandle = PifOpenFile (pAllObjects->ConnEngine.tcsTempFileName, "tr");
		if (sFileHandle >= 0) {
			ULONG  ulFileSizeLow, ulFileSizeHigh;
			ulFileSizeLow = PifGetFileSize (sFileHandle, &ulFileSizeHigh);
			CliMsg.usFunCode    = CLI_FCCOPONNENGINEMSGFH;
			CliMsg.pauchReqData = ( UCHAR *)&sFileHandle;
			NHPOS_ASSERT(ulFileSizeLow < 0xFFFF);
			CliMsg.usReqLen = ReqMsgBuffer.usBufferLengthBytes = ulFileSizeLow;
			sError = CstSendTerminalFH(0, pAllObjects->ConnEngine.sTerminalNumber);
			PifCloseFile (sFileHandle);
		} else {
			char xBuff[128];
			sprintf (xBuff, "CliOpConnEngineObject(): File Open error %d", sFileHandle);
			NHPOS_ASSERT_TEXT((sFileHandle >= 0), xBuff);
		}
	} else {
		CliMsg.usFunCode    = CLI_FCCOPONNENGINEMSG;
		CliMsg.pauchReqData = (UCHAR *)pAllObjects;
		CliMsg.usReqLen     = sizeof(AllObjects);

		sError = CstSendTerminal(pAllObjects->ConnEngine.sTerminalNumber);
	}

    PifReleaseSem(husCliExeNet);

    return sError;
}

SHORT   CliOpConnEngineFH (SHORT  hsFileHandle, USHORT usSize)
{
	CLIREQOPCONNENGINE    ReqMsgBuffer = {0};
	CLIRESOPCONNENGINE    ResMsgBuffer = {0};
    SHORT                 sMoved=0, sError, sRetCode, sErrorBM = 0;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg,  0, sizeof(CLICOMIF));

	ReqMsgBuffer.usTerminalNo = (UCHAR) PIF_CLUSTER_MASK_TERMINAL_NO(PifSysConfig()->usTerminalPositionFromName);
	ResMsgBuffer.usTerminalNo = ReqMsgBuffer.usTerminalNo;
	ReqMsgBuffer.iObjectType = BL_CONNENGINE_MSG_SENDFILE;

    CliMsg.usFunCode    = CLI_FCCOPONNENGINEMSGFH;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgBuffer;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPCONNENGINE);
    CliMsg.pauchReqData = ( UCHAR *)&hsFileHandle;
    CliMsg.usReqLen = ReqMsgBuffer.usBufferLengthBytes = usSize;

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgBuffer;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPCONNENGINE);
    CliMsg.sRetCode     = OP_COMM_ERROR;

	sRetCode = CliMsg.sRetCode = OP_PERFORM;
	// We need to make sure that we specify (!CLI_FCBAKGCF) as the type
	// because using zero (0) results in Satellite Terminal issues with Service Total.
	// When this was zero (0) a Service Total from the Satellite Terminal as well as
	// guest check delivery in Drive Thru functionality was not working correctly.
    sError = CstSendMasterFH(!CLI_FCBAKGCF);       /* --- Send Master Terminal --- */

#if 0
	// code needed for a Backup Master system to keep Backup Master updated.
	// this code appears to create a problem in a Backup Master system however.
	// further work to be done.
	if ((sError == STUB_M_DOWN) || (OP_PERFORM == CliMsg.sRetCode)) {
        if (STUB_SELF == (sErrorBM = CstSendBMasterFH(0))) {
			sError = STUB_SELF;
		}
	}
#endif

    PifReleaseSem(husCliExeNet);

    return sError;
}



/*
*===========================================================================
** Synopsis:    SHORT   CliOpUnjoinCluster(TCHAR *pwszHostName,
*                                          UCHAR *pauchHostIpAddress,
*                                          UCHAR *pauchTerminalNo);
*
*     Input:    pwszHostName        - Computer Name, name of Disconnected Satellite host
*               pauchHostIpAddress  - IP address of this Disconnected Satellite host
*               pauchTerminalNo     - terminal number or zero if lookup against IP address
*    
** Return:      none.
*
** Description:
*===========================================================================
*/
SHORT   CliOpUnjoinCluster(TCHAR *pwszHostName, UCHAR *pauchHostIpAddress, UCHAR *pauchTerminalNo)
{
    SYSCONFIG    *pSysConfig = PifSysConfig();
    CLIREQJOIN    ReqMsgH = {0};
    CLIRESJOIN    ResMsgH = {0};
    SHORT         sMoved=0, sError, sRetCode;

    PifRequestSem(husCliExeNet);

	*pauchTerminalNo = (UCHAR) PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);

    memset(&CliMsg,  0, sizeof(CLICOMIF));

    CliMsg.usFunCode    = CLI_FCOPUNJOINCLUSTER;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQJOIN);

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESJOIN);
    CliMsg.sRetCode     = OP_COMM_ERROR;

	_tcscpy (ReqMsgH.wszHostName, pwszHostName);

	ReqMsgH.auchHostIpAddress[0] = pauchHostIpAddress[0];
	ReqMsgH.auchHostIpAddress[1] = pauchHostIpAddress[1];
	ReqMsgH.auchHostIpAddress[2] = pauchHostIpAddress[2];
	ReqMsgH.auchHostIpAddress[3] = pauchHostIpAddress[3];

	ReqMsgH.auchTerminalNo = *pauchTerminalNo;

	sRetCode = CliMsg.sRetCode = OP_PERFORM;
    if (STUB_SELF != (sError = CstSendMaster())) {     /* --- Send Master Terminal --- */
		sRetCode = CliMsg.sRetCode = OP_PERFORM;
		CliNetConfig.auchFaddr[CLI_POS_UA] = 1; 
		CliNetConfig.fchSatStatus &= ~CLI_SAT_JOINED;
		IspNetConfig.auchFaddr[CLI_POS_UA] = 1; 
		SerNetConfig.auchFaddr[CLI_POS_UA] = 1;
		SerNetConfig.fchSatStatus &= ~SER_SAT_JOINED;
		pSysConfig->auchLaddr[CLI_POS_UA] = 1;
		usBMOption = 0;    // doing CliOpUnjoinCluster() so force Backup Master indicator to be off.

		PifNetUpdateMasterBackup (pwszHostName, pauchHostIpAddress, 0, 0);
    }

    PifReleaseSem(husCliExeNet);

    return sRetCode;
}

SHORT   OpJoinCluster (PIF_CLUSTER_HOST  *pPifHostInfo, UCHAR *pauchTerminalNo)
{
	SHORT sRetStatus;

	sRetStatus = PifNetAddJoin (pPifHostInfo, pauchTerminalNo);

	if (sRetStatus == PIF_OK)
		sRetStatus = OP_PERFORM;
	else
		sRetStatus = OP_FILE_FULL;

	return sRetStatus;
}

/*
*===========================================================================
** Synopsis:    SHORT   CliOpjoinCluster(TCHAR *pwszHostName,
*                                        UCHAR *pauchHostIpAddress,
*                                        UCHAR *pauchTerminalNo);
*
*     Input:    pwszHostName        - Computer Name, name of Disconnected Satellite host
*               pauchHostIpAddress  - IP address of this Disconnected Satellite host
*               pauchTerminalNo     - terminal number requested or zero if
*    
** Return:      none.
*
** Description:
*   This function issues a broadcasted Join message requesting that a Master Terminal
*   allow this Disconnected Satellite to Join an existing cluster.  The message
*   broadcasted includes the Disconnected Satellite's Computer Name as well as its
*   IP address.  A terminal number may be specified so that the Master Terminal will
*   add the Disconnected Satellite as a particular terminal number in the cluster or
*   if the terminal number is zero, the Master Terminal will pick the next available
*   terminal number and use that.
*===========================================================================
*/
SHORT   CliOpJoinCluster(TCHAR *pwszHostName, UCHAR *pauchHostIpAddress, UCHAR *pauchTerminalNo)
{
    SYSCONFIG    *pSysConfig = PifSysConfig();
	CLIREQJOIN    ReqMsgH = {0};
    CLIRESJOIN    ResMsgH = {0};
    SHORT         sMoved=0, sError, sRetCode;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg,  0, sizeof(CLICOMIF));

    CliMsg.usFunCode    = CLI_FCOPJOINCLUSTER;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQJOIN);

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESJOIN);
    CliMsg.sRetCode     = OP_COMM_ERROR;

	_tcscpy (ReqMsgH.wszHostName, pwszHostName);

	ReqMsgH.auchHostIpAddress[0] = pauchHostIpAddress[0];
	ReqMsgH.auchHostIpAddress[1] = pauchHostIpAddress[1];
	ReqMsgH.auchHostIpAddress[2] = pauchHostIpAddress[2];
	ReqMsgH.auchHostIpAddress[3] = pauchHostIpAddress[3];

	ReqMsgH.auchTerminalNo = *pauchTerminalNo = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);
	ReqMsgH.usStoreNumber = 0; //SysConfig->usStoreNumber;         // The store number for this terminal
	ReqMsgH.usRegisterNumber = 0; //SysConfig->usRegisterNumber;   // the number of this particular terminal or register

    if (STUB_SELF == (sError = CstSendBroadcast(&CliMsg))) {     /* --- Send Master Terminal --- */
		sRetCode = sError;
		NHPOS_ASSERT_TEXT(0, "CliOpJoinCluster():  STUB_SELF == (sError = CstSendBroadcast())");
    } else {
		// Get a copy of the return code in a local variable before exiting
		// the critical region protecting access to the global CliMsg in
		// case some other thread is waiting for it.
		// If the status is OP_PERFORM then we have a valid response
		// and we should update our PifNet tables so that we know where the
		// Master and the Backup Master terminals are located.
		// Finally, we will update our own table so that the Client Stub code
		// will use the correct IP address if we are addressing ourself.
		sRetCode = CliMsg.sRetCode;
		NHPOS_ASSERT_TEXT((sRetCode == OP_PERFORM), "CliOpJoinCluster():  Join request failed.");
		if (sRetCode == OP_PERFORM) {
			PIF_CLUSTER_HOST  PifHostInfo = {0};

			_tcscpy (PifHostInfo.wszHostName, pwszHostName);
			memcpy (PifHostInfo.auchHostIpAddress, pauchHostIpAddress, 4);
			PifHostInfo.usStoreNumber = 0; //SysConfig->usStoreNumber;         // The store number for this terminal
			PifHostInfo.usRegisterNumber = 0; //SysConfig->usRegisterNumber;   // the number of this particular terminal or register

			*pauchTerminalNo = ResMsgH.auchTerminalNo;
			CliNetConfig.auchFaddr[CLI_POS_UA] = *pauchTerminalNo;
			CliNetConfig.fchSatStatus |= CLI_SAT_JOINED;
			IspNetConfig.auchFaddr[CLI_POS_UA] = *pauchTerminalNo; 
			SerNetConfig.auchFaddr[CLI_POS_UA] = *pauchTerminalNo;
			SerNetConfig.fchSatStatus |= SER_SAT_JOINED;
			pSysConfig->auchLaddr[CLI_POS_UA] = *pauchTerminalNo;
			PifNetUpdateMasterBackup (ResMsgH.wszMasName, ResMsgH.auchMasIpAddress, ResMsgH.wszBMasName, ResMsgH.auchBMasIpAddress);

			if (ResMsgH.wszBMasName[0]) {
				usBMOption = 1;    // doing CliOpJoinCluster(), force Backup Master indicator ON since specified.
			}
			else {
				usBMOption = 0;    // doing CliOpJoinCluster(), force Backup Master indicator OFF since not specified.
			}
			PifNetAddJoin (&PifHostInfo, pauchTerminalNo);
			_tcscpy (pwszHostName, ResMsgH.wszMasName);
			memcpy (pauchHostIpAddress, ResMsgH.auchMasIpAddress, PIF_LEN_IP);

			SstChangeInqCurPreTraNo (*pauchTerminalNo, ResMsgH.ausTranNo, ResMsgH.ausPreTranNo);
			CstTtlSetCliTranNo(*pauchTerminalNo);
			PifLog(MODULE_STB_LOG, LOG_EVENT_STB_JOIN_PROCESSED);
			PifLog(MODULE_DATA_VALUE(MODULE_STB_LOG), (USHORT) *pauchTerminalNo);
			PifLog(MODULE_DATA_VALUE(MODULE_STB_LOG), (USHORT) ResMsgH.ausTranNo);
			PifLog(MODULE_DATA_VALUE(MODULE_STB_LOG), (USHORT) ResMsgH.ausPreTranNo);
		}
		else {
			PifLog(MODULE_STB_LOG, LOG_ERROR_STB_JOIN_FAILED);
			PifLog(MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT) abs(CliMsg.sRetCode));
		}
	}

    PifReleaseSem(husCliExeNet);

	// Now lets see if we have special actions that we need to do as part of Joining this cluster
	if ((sRetCode == OP_PERFORM) && (ResMsgH.ulSpecialFlags & CLIRESJOIN_FLAGS_CHECK_OP_MSG) != 0) {
		// We need to check for the last operator message.
		USHORT  usMessageBufferEnum = 0xffff;
		OpDisplayOperatorMessage (&usMessageBufferEnum);
	}

    return sRetCode;
}


SHORT   CliOpTransferTotals(USHORT usTerminalPosition)
{
    CLIREQTRANSFER    ReqMsgH = {0};
    CLIRESTRANSFER    ResMsgH = {0};
    SHORT             sMoved=0, sError, sRetCode;
	USHORT            usLockHnd = 0;

	{
		SHORT  sRetStatus = 0;

		PifDeleteFile(_T("TOTALPLD"));
		sRetStatus = TtlExportPLU (CLASS_TTLCURDAY, _T("TOTALPLD"));
		if (sRetStatus < 0) {
			PifLog(MODULE_STB_LOG, LOG_ERROR_STB_PLU_TTLCURDAY_FAILED);
			PifLog(MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)abs(sRetStatus));
		}

		PifDeleteFile(_T("TOTALPLP"));
		sRetStatus = TtlExportPLU (CLASS_TTLCURPTD, _T("TOTALPLP"));
		if (sRetStatus < 0) {
			PifLog(MODULE_STB_LOG, LOG_ERROR_STB_PLU_TTLCURPTD_FAILED);
			PifLog(MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)abs(sRetStatus));
		}
	}

    PifRequestSem(husCliExeNet);

    memset(&CliMsg,  0, sizeof(CLICOMIF));

    CliMsg.usFunCode    = CLI_FCOPTRANSFERTOTALS;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(ReqMsgH);

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(ResMsgH);

	ReqMsgH.auchTerminalNo = usTerminalPosition;

	sRetCode = CliMsg.sRetCode = OP_PERFORM;
    if (STUB_SELF == (sError = CstSendMaster())) {     /* --- Send Master Terminal --- */
		NHPOS_ASSERT_TEXT(0, "CliOpTransferTotals(): STUB_SELF == (sError = CstSendMaster()");
		CliMsg.sRetCode = OP_COMM_ERROR;
	}
	else {
		int  iFileCount = 1;
		int  iRetryAttempts = 0;

		while (CliMsg.sRetCode == OP_PERFORM) {
			CstNetReceive(CLI_MDC_CPMWAIT, &CliMsg, &CliSndBuf, &CliRcvBuf);
			// Process the message received from the Server Thread that
			// is doing the transfer of files.  As long as things are ok
			// we should see a return code of OP_PERFORM and when done
			// we should see a return code of OP_EOF.
			CstComCheckError(&CliMsg, &CliRcvBuf);                  /* error check */
			CstComRespHandle(&CliMsg, &CliSndBuf, &CliRcvBuf);      /* response handling */

			if (CliMsg.sError != STUB_SUCCESS) {
				char  xBuff[128];

				if (iRetryAttempts == 0 || CliMsg.sError != STUB_TIME_OUT) {
					sprintf (xBuff, "CliOpTransferTotals(): Transfer error file # %d, sError = %d.", iFileCount, CliMsg.sError);
					NHPOS_ASSERT_TEXT(CliMsg.sError == STUB_SUCCESS, xBuff);
				}

				if (CliMsg.sError == STUB_TIME_OUT) {
					iRetryAttempts++;
					if (iRetryAttempts > 10)
						break;
					PifSleep (150);       //  CliOpTransferTotals(): transfer totals from Master.
					CliMsg.sRetCode = OP_PERFORM;
					continue;
				}
			}
			iFileCount++;
			iRetryAttempts = 0;
		}
	}

	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);

    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT   CliOpUpdateTotalsMsg(PLUIF *pPlu, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pPif        - PLU information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_NOT_IN_FILE:     PLU not in file
*               OP_LOCK:            during Locked
*
** Description: This function will update the specified totals.
*               It is intended to be used with Store and Forward during the
*               Transfer functionality to update the appropriate totals as
*               stored transactions are processed and either accepted, denied,
*               or an error of some kind happens.
*===========================================================================
*/
SHORT   CliOpUpdateTotalsMsg(TTLMESSAGEUNION  *pTtlMessage)
{
    CLIREQSFTOTALS  ReqMsgH = {0};
    CLIREQSFTOTALS  ResMsgH = {0};
    SHORT           sError, sRetCode;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg,  0, sizeof(CliMsg));

    CliMsg.usFunCode    = CLI_FCOPUPDATETOTALSMSG | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(ReqMsgH);

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(ResMsgH);
    CliMsg.sRetCode     = OP_PERFORM;

	memcpy (&(ReqMsgH.TtlMessage), pTtlMessage, sizeof(TTLMESSAGEUNION));

    if (STUB_SELF == (sError = CstSendMaster())) {     /* --- Send Master Terminal --- */
        CliMsg.sRetCode = SerOpUpdateTotalsMsg(pTtlMessage);
    }

    if ((sError == STUB_M_DOWN) || (CliMsg.sRetCode == OP_PERFORM)) {
        if (STUB_SELF == (sError = CstSendBMaster())) {     /* --- Send BMaster Terminal --- */
            CliMsg.sRetCode = SerOpUpdateTotalsMsg(pTtlMessage);
        }
    }

	// Get a copy of the return code in a local variable before exiting
	// the critical region protecting access to the global CliMsg in
	// case some other thread is waiting for it.
	sRetCode = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);

    return sRetCode;
}


/*
*===========================================================================
** Synopsis:    SHORT   CliOpPluIndRead(PLUIF *pPif, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pPif        - PLU information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_NOT_IN_FILE:     PLU not in file
*               OP_LOCK:            during Locked
*
** Description: This function supports to read a PLU record.        Saratoga
*===========================================================================
*/
SHORT   CliOpPluIndRead(PLUIF *pPlu, USHORT usLockHnd)
{
	XGRAM       CliSndBuf2;          /* Send Buffer */    
	XGRAM       CliRcvBuf2;          /* Receive Buffer */
	CLICOMIF    CliMsg2;
    CLIREQSCNPLU    ReqMsgH = {0};
    CLIRESSCNPLU    ResMsgH = {0};
    SHORT           sMoved = 0, sRetCode;
    SHORT           sErrorBM = 0, sErrorM = 0;
	BOOL			blTest;
	USHORT          i;

//    PifRequestSem(husCliExeNet);

    memset(&CliMsg2,  0, sizeof(CLICOMIF));

	// Test Mode.  The following code tests for a special
	// PLU number of "00000000000000" (an illegal PLU number
	// of all digits being zero) and if the PLU number is
	// this illegal value then the PLU data buffer is filled with
	// a sequence of byte aligned values that number the bytes.
	// This numbering of bytes is to allow Third Party products
	// such as MWS to be tested to ensure that they are decoding
	// the data packets correctly.
	blTest = TRUE;
	for (i = 0; i < sizeof(pPlu->auchPluNo)/sizeof(pPlu->auchPluNo[0]) && blTest; i++)
	{
		blTest = blTest && (pPlu->auchPluNo[i] == _T('-'));
	}

	if (blTest)
	{
		// Fill the PLUIF buffer with a sequence of numbers
		// beginning with 1.  At 256, the sequence will restart
		// at 0.
		UCHAR    *pTestArray;
		for (i = 0, pTestArray = (UCHAR *)pPlu; i < sizeof(PLUIF); i++, pTestArray++)
			*pTestArray = (UCHAR)(i+1);

		sRetCode = CliMsg2.sRetCode = OP_TEST;
//		PifReleaseSem(husCliExeNet);

		return sRetCode;
	}

    _tcsncpy(ReqMsgH.aucPluNumber, pPlu->auchPluNo, STD_PLU_NUMBER_LEN);
    ReqMsgH.uchAdjective = pPlu->uchPluAdj;

    CliMsg2.usFunCode    = CLI_FCOPPLUINDREAD | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg2.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg2.usReqMsgHLen = sizeof(CLIREQSCNPLU);

    CliMsg2.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg2.usResMsgHLen = sizeof(CLIRESSCNPLU);
    CliMsg2.sRetCode     = OP_PERFORM;

	if (STUB_SELF == (sErrorM = CstSendMasterWithArgs (&CliMsg2, &CliSndBuf2, &CliRcvBuf2))) {
        CliMsg2.sRetCode = OpPluIndRead(pPlu, usLockHnd);
        sMoved = 1;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
	}

    if (sErrorM == STUB_M_DOWN) {
        if (STUB_SELF == (sErrorBM = CstSendBMasterWithArgs (&CliMsg2, &CliSndBuf2, &CliRcvBuf2))) {     /* --- Send BMaster Terminal --- */
            CliMsg2.sRetCode = OpPluIndRead(pPlu, usLockHnd);
            sMoved = 1;
        }
    }

    if (sMoved == 0) {
		CliAssignCliresscnpluToPluif (pPlu, &ResMsgH);
	}

	// Get a copy of the return code in a local variable before exiting
	// the critical region protecting access to the global CliMsg in
	// case some other thread is waiting for it.
	sRetCode = CliMsg2.sRetCode;
//    PifReleaseSem(husCliExeNet);

	if (sErrorM == STUB_M_DOWN) {
//		CstMTOutOfDate();
	}
	if (sErrorBM == STUB_BM_DOWN) {
//		CstBMOutOfDate();
	}
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT   CliOpPluAssign(PLUIF *pPlu, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pPif        - PLU information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_NOT_IN_FILE:     PLU not in file
*               OP_LOCK:            during Locked
*
** Description: This function adds PLU record for Building.                  Saratoga
*===========================================================================
*/
SHORT   CliOpPluAssign(PLUIF *pPlu, USHORT usLockHnd)
{
    CLIREQMNTPLU    ReqMsgH = {0};
    CLIRESMNTPLU    ResMsgH = {0};
    SHORT           sErrorBM = 0, sErrorM = 0, sRetCode;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg,  0, sizeof(CLICOMIF));

	CliAssignPluifToClireqmntplu (&ReqMsgH, pPlu);

	CliMsg.usFunCode    = CLI_FCOPPLUASSIGN | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQMNTPLU);

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESMNTPLU);
    CliMsg.sRetCode     = OP_PERFORM;

    if (STUB_SELF == (sErrorM = CstSendMaster())) {     /* --- Send Master Terminal --- */
        CliMsg.sRetCode = OpPluAssign(pPlu, usLockHnd);
		sErrorM = STUB_SUCCESS;
    }

	// Get a copy of the return code in a local variable before exiting
	// the critical region protecting access to the global CliMsg in
	// case some other thread is waiting for it.
	sRetCode = CliMsg.sRetCode;

    if ((sErrorM == STUB_M_DOWN) || (CliMsg.sRetCode == OP_NEW_PLU || CliMsg.sRetCode == OP_CHANGE_PLU)) {
        if (STUB_SELF == (sErrorBM = CstSendBMaster())) {     /* --- Send BMaster Terminal --- */
            CliMsg.sRetCode = OpPluAssign(pPlu, usLockHnd);
        }

		if (sErrorM == STUB_M_DOWN) {
			// if there was a problem accessing Master Terminal then return Backup's error.
			sRetCode = CliMsg.sRetCode;
		}
    }
    PifReleaseSem(husCliExeNet);

	if (sErrorM == STUB_M_DOWN) {
		CstMTOutOfDate();
	}
	if (sErrorBM == STUB_BM_DOWN) {
		CstBMOutOfDate();
	}
	if (sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		char xBuff[128];
		sprintf (xBuff, "==NOTE: CliOpPluOepRead() sRetCode %d, sErrorM %d, sErrorBM %d", sRetCode, sErrorM, sErrorBM);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT   CliOpPluDelete(PLUIF *pPlu, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pPif        - PLU information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_NOT_IN_FILE:     PLU not in file
*               OP_LOCK:            during Locked
*
** Description: This function deletes PLU record for building.                   Saratoga
*===========================================================================
*/
SHORT   CliOpPluDelete(PLUIF *pPlu, USHORT usLockHnd)
{
    CLIREQMNTPLU    ReqMsgH = {0};
    CLIRESMNTPLU    ResMsgH = {0};
    SHORT           sError, sRetCode;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg,  0, sizeof(CLICOMIF));

    _tcsncpy(ReqMsgH.aucPluNumber, pPlu->auchPluNo, STD_PLU_NUMBER_LEN);
    ReqMsgH.uchAdjective    = pPlu->uchPluAdj;

    CliMsg.usFunCode    = CLI_FCOPPLUDELETE | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQMNTPLU);

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESMNTPLU);
    CliMsg.sRetCode     = OP_PERFORM;

    if (STUB_SELF == (sError = CstSendMaster())) {     /* --- Send Master Terminal --- */
        CliMsg.sRetCode = OpPluDelete(pPlu, usLockHnd);
    }

    if ((sError == STUB_M_DOWN) || (CliMsg.sRetCode == OP_PERFORM)) {
        if (STUB_SELF == (sError = CstSendBMaster())) {     /* --- Send BMaster Terminal --- */
            CliMsg.sRetCode = OpPluDelete(pPlu, usLockHnd);
        }
    }

	// Get a copy of the return code in a local variable before exiting
	// the critical region protecting access to the global CliMsg in
	// case some other thread is waiting for it.
	sRetCode = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);

    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpPluAssign(PLUIF *pPlu, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pPif        - PLU information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_NOT_IN_FILE:     PLU not in file
*               OP_LOCK:            during Locked
*
** Description: This function adds PLU record.                  Saratoga
*===========================================================================
*/
SHORT   SerOpPluAssign(PLUIF *pPlu, USHORT usLockHnd)
{
	CLIREQMNTPLU    ReqMsgH = {0};
    CLIRESMNTPLU    ResMsgH = {0};
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg,  0, sizeof(CLICOMIF));

	CliAssignPluifToClireqmntplu(&ReqMsgH, pPlu);

    CliMsg.usFunCode    = CLI_FCOPPLUASSIGN | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQMNTPLU);

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESMNTPLU);
    CliMsg.sRetCode     = OP_PERFORM;

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = OpPluAssign(pPlu, usLockHnd);
    } else {
        sRetCode = STUB_NOT_MASTER;
    }
    if (sRetCode == OP_NEW_PLU || sRetCode == OP_CHANGE_PLU) {
        SstUpdateBackUp();
    }

    if (PifSysConfig()->ausOption[UIE_TEST_OPTION8] == UIE_TEST_PASS) {  /* SerOpPluAssign() Debug Option, Generate PLU records */
        
        /*=================== TEST START, Saratoga ================================*/
        if (sRetCode == OP_NEW_PLU || sRetCode == OP_CHANGE_PLU) {
            if ((pPlu->ParaPlu.auchPluName[0] == _T('A')) &&
                (pPlu->ParaPlu.auchPluName[1] == _T('0')) &&
                (pPlu->ParaPlu.auchPluName[2] == _T('A')) &&
                (pPlu->ParaPlu.auchPluName[3] == _T('1')))
			{ 
				ULONG           ulI = 1L;
				LABELANALYSIS   PLUNO = {0};
				PLUIF           Plu = *pPlu;   // initialize PLUIF before beginning iteration through max test range

				NHPOS_NONASSERT_NOTE("==TEST ENABLE", "==TEST ENABLE: Generate FLEX_PLU_MAX PLU records.");
                for (ulI = 1L; ((OP_NEW_PLU == sRetCode) || (OP_CHANGE_PLU == sRetCode)) && (ulI <= FLEX_PLU_MAX); ulI++) {
                    Plu.ParaPlu.auchPluName[0] = _T('P');
                    Plu.ParaPlu.auchPluName[1] = _T('L');
                    Plu.ParaPlu.auchPluName[2] = _T('U');
                    _ultot(ulI, &Plu.ParaPlu.auchPluName[3], 10);
                    _ultot(ulI, PLUNO.aszScanPlu, 10);
                    PLUNO.fchModifier |= LA_UPC_VEL;
                    RflLabelAnalysis(&PLUNO);
                    _tcsncpy(Plu.auchPluNo, PLUNO.aszLookPlu, sizeof(Plu.auchPluNo)/sizeof(Plu.auchPluNo[0]));

                    if (Plu.ParaPlu.ContPlu.uchItemType != 0 ) {
                        Plu.ParaPlu.ContPlu.uchItemType = PLU_NON_ADJ_TYP;  
                    }
                    sRetCode = OpPluAssign(&Plu, usLockHnd);
                }
            }
        }
        /*=================== TEST END =============================*/
    }

    PifReleaseSem(husCliExeNet);

    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpPluDelete(PLUIF *pPlu, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pPif        - PLU information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_NOT_IN_FILE:     PLU not in file
*               OP_LOCK:            during Locked
*
** Description: This function deletes PLU record.                   Saratoga
*===========================================================================
*/
SHORT   SerOpPluDelete(PLUIF *pPlu, USHORT usLockHnd)
{
	CLIREQMNTPLU    ReqMsgH = {0};
    CLIRESMNTPLU    ResMsgH = {0};
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg,  0, sizeof(CLICOMIF));

    memcpy(ReqMsgH.aucPluNumber, pPlu->auchPluNo, sizeof(ReqMsgH.aucPluNumber));
    ReqMsgH.uchAdjective    = pPlu->uchPluAdj;

    CliMsg.usFunCode    = CLI_FCOPPLUDELETE | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQMNTPLU);

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESMNTPLU);
    CliMsg.sRetCode     = OP_PERFORM;

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = OpPluDelete(pPlu, usLockHnd);
    } else {
        sRetCode = STUB_NOT_MASTER;
    }
    if (OP_PERFORM == sRetCode) {
        SstUpdateBackUp();
    }

    PifReleaseSem(husCliExeNet);

    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT   CliOpDeptIndRead(DEPTIF *pDpi, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pDpi        - Dept information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_ABNORMAL_DEPT:   PLU not found in PLU file
*               OP_LOCK:            during Locked
*
** Description: This function supports to read a Dept record.
*===========================================================================
*/
SHORT   CliOpDeptIndRead(DEPTIF *pDpi, USHORT usLockHnd)
{
    return OpDeptIndRead(pDpi, usLockHnd);
}

/*
*===========================================================================
** Synopsis:    SHORT   CliOpCpnIndRead(CPNIF *pCpn, USHORT usLockHnd);
*     InPut:    usLockHnd    - handle
*     InOut:    *pCpn        - Coupon information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  coupon file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to read a Coupon record.
*===========================================================================
*/
SHORT   CliOpCpnIndRead(CPNIF *pCpn, USHORT usLockHnd)
{
    return OpCpnIndRead(pCpn, usLockHnd);
}

/*
*===========================================================================
** Synopsis:    SHORT   CliOpOepPluRead(PLUIF_OEP *pPlu, USHORT usLockHnd)
*     InPut:    usLockHnd    - handle
*     InOut:    *pPlu        - OEP taget PLU
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to read OEP target PLU.
*===========================================================================
*/
SHORT   CliOpOepPluRead(PLUIF_OEP *pCpn, USHORT usLockHnd, UCHAR ucNumOfPLUrequested)
{
	// Lets check to see if notice board says master is NOT updating.
	// If not then we will just return with an error.
//	if (CLI_IAM_MASTER || CLI_IAM_BMASTER) {
//		if (CstComReadStatus() & (CLI_STS_STOP | CLI_STS_INQUIRY | CLI_STS_INQTIMEOUT)) {
//			return OP_NOT_IN_FILE;
//		}
//	}
    return OpOepPluRead(pCpn, usLockHnd, ucNumOfPLUrequested); /* added ucNumOfPLUrequested to hold
																	number of item requested - CSMALL */
}

SHORT   CliOpOepPluReadMultiTable(USHORT usTableNo, USHORT usFilePart, PLUIF_OEP *pCpn, USHORT usLockHnd, UCHAR ucNumOfPLUrequested)
{
	// Lets check to see if notice board says master is NOT updating.
	// If not then we will just return with an error.
//	if (CLI_IAM_MASTER || CLI_IAM_BMASTER) {
//		if (CstComReadStatus() & (CLI_STS_STOP | CLI_STS_INQUIRY | CLI_STS_INQTIMEOUT)) {
//			return OP_NOT_IN_FILE;
//		}
//	}
    return OpOepPluReadMultiTable(usTableNo, usFilePart, pCpn, usLockHnd, ucNumOfPLUrequested); /* added ucNumOfPLUrequested to hold
																	number of item requested - CSMALL */
}

/*
*===========================================================================
** Synopsis:    SHORT   CliOpCstrIndRead(CSTRIF *pCstr, USHORT usLockHnd)
*     InPut:    usLockHnd    - handle
*     InOut:    *pPlu        - OEP taget PLU
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to read control string table.
*===========================================================================
*/
SHORT   CliOpCstrIndRead(CSTRIF *pCstr, USHORT usLockHnd)
{
    return OpCstrIndRead(pCstr, usLockHnd);
}

/*
*===========================================================================
** Synopsis:    SHORT    SerChangeOpPara(USHORT usType, 
*                                       CLIOPBCAS *pBcasRegNo);
*     InPut:    usType  - 1:All 2:Super 3:PLU
*               pBcasRegNo   - Broadcast register number storage area
*
** Return:      STUB_SUCCESS:   successful writing 
*               STUB_BUSY:      illegal condition detected    
*
** Description:
*   This function writes to change specified parameter onto Notice Board.
*===========================================================================
*/
SHORT    SerChangeOpPara(USHORT usType, CLIOPBCAS *pBcasRegNo)
{
    UCHAR       uchWriteFlag;
    USHORT      usFunCode;

    switch(usType) {
    case    CLI_LOAD_ALL:                    /* All Para changed */
        if (CLI_IAM_MASTER) {
            SstChangeInqBcasInf(0xfffe);    /* MT Mask */
        }
        if (CLI_IAM_BMASTER) {
            SstChangeInqBcasInf(0xfffd);    /* BM Mask */
        }
        usFunCode    = CLI_FCOPREQALL;
        uchWriteFlag = NB_REQALLPARA;
        break;
                                                 
    case    CLI_LOAD_SUP:                   /* Super changed */
        usFunCode    = CLI_FCOPREQSUP;
        uchWriteFlag = NB_REQSUPER;
        break;

#if 0
    case    CLI_LOAD_PLU:                   /* PLU changed */
        usFunCode    = CLI_FCOPREQPLU;
        uchWriteFlag = NB_REQPLU;
        break;
#endif

    default:                                /* not use */
        return STUB_BUSY;
        break;
    }

    NbWriteMessage(NB_MESOFFSET0, uchWriteFlag);
    SstSetFlag(SER_SPESTS_BEGIN_BCAS);
    SstSetBcasPointer(pBcasRegNo, usFunCode);
    SstSetFlagWaitDone(SER_SPESTS_BCAS, husCliWaitDone);
    SstSetBcasPointer(NULL, 0);
    NbResetMessage(NB_MESOFFSET0, uchWriteFlag);

    return STUB_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpDeptAssign(DEPTIF *pDpi, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pDpi        - Dept information to assign
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  Dept file not exist
*               OP_ABNORMAL_DEPT:   abnormal dept number (HALT)
*               OP_LOCK:            during Locked
*
** Description: This function supports to assign a Dept record.
*===========================================================================
*/
SHORT   SerOpDeptAssign(DEPTIF *pDpi, USHORT usLockHnd)
{
    SHORT       sRetCode;

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = OpDeptAssign(pDpi, usLockHnd);
        if ((OP_NEW_DEPT == sRetCode) || (OP_CHANGE_DEPT == sRetCode)) {
            sRetCode = OP_PERFORM;
        }

        if (PifSysConfig()->ausOption[UIE_TEST_OPTION8] == UIE_TEST_PASS) {  /* SerOpDeptAssign() Debug Option, Generate Department records */

            /*================ TEST START =========================*/
            if ((OP_PERFORM == sRetCode ) &&
                (pDpi->ParaDept.auchMnemonic[0] == _T('A')) &&
                (pDpi->ParaDept.auchMnemonic[1] == _T('0')) &&
                (pDpi->ParaDept.auchMnemonic[2] == _T('A')) &&
                (pDpi->ParaDept.auchMnemonic[3] == _T('1')))
			{
				USHORT      usI;
				DEPTIF      Dpi = *pDpi;
				UCHAR       uchGrp = 1;

				NHPOS_NONASSERT_NOTE("==TEST ENABLE", "==TEST ENABLE: Generate FLEX_DEPT_MAX Department records.");
                for (usI = 1; (OP_PERFORM  == sRetCode) && (usI <= FLEX_DEPT_MAX); usI++) {
                    Dpi.usDeptNo = usI;
                    Dpi.ParaDept.auchMnemonic[0] = _T('D');
                    Dpi.ParaDept.auchMnemonic[1] = _T('E');
                    Dpi.ParaDept.auchMnemonic[2] = _T('P');
                    _itot(usI, &Dpi.ParaDept.auchMnemonic[3], 10);
                    Dpi.ParaDept.uchMdept = uchGrp;
                    sRetCode = OpDeptAssign(&Dpi, usLockHnd);
                    uchGrp ++;
                    if ( 11 == uchGrp ) {
                        uchGrp = 1 ;
                    }
                    if ((OP_NEW_DEPT == sRetCode) || (OP_CHANGE_DEPT == sRetCode)) {
                        sRetCode = OP_PERFORM;
                    }
                }
                sRetCode = OP_PERFORM;
            }
        }
        /*=================== TEST END =============================*/
        return (sRetCode);
    } 
    return OP_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpCpnAssign(CPNIF *pCpni, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pCpni      - Coupon information to assign
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  Coupon file not exist
*               OP_ABNORMAL_CPN:    abnormal coupon number (HALT)
*               OP_LOCK:            during Locked
*
** Description: This function supports to assign a Coupon record.
*===========================================================================
*/
SHORT   SerOpCpnAssign(CPNIF *pCpni, USHORT usLockHnd)
{
    SHORT            sRetCode;

    if (STUB_SELF == SstUpdateAsMaster()) {
        
        sRetCode = OpCpnAssign(pCpni, usLockHnd);

        if (PifSysConfig()->ausOption[UIE_TEST_OPTION8] == UIE_TEST_PASS) {  /* SerOpCpnAssign() Debug Option, generate Coupon records */

            /*================ TEST START =========================*/
            if ((OP_PERFORM == sRetCode ) &&
                (pCpni->ParaCpn.aszCouponMnem[0] == _T('A')) &&
                (pCpni->ParaCpn.aszCouponMnem[1] == _T('0')) &&
                (pCpni->ParaCpn.aszCouponMnem[2] == _T('A')) &&
                (pCpni->ParaCpn.aszCouponMnem[3] == _T('1')))
			{
				USHORT    usI = 1;
				CPNIF     Cpni = *pCpni;
    
				NHPOS_NONASSERT_NOTE("==TEST ENABLE", "==TEST ENABLE: Generate FLEX_CPN_MAX Coupon records.");
                for (usI = 1; (OP_PERFORM  == sRetCode) && (usI <= FLEX_CPN_MAX); usI++) {
                    Cpni.uchCpnNo = usI;
                    Cpni.ParaCpn.aszCouponMnem[0] = _T('C');
                    Cpni.ParaCpn.aszCouponMnem[1] = _T('P');
                    Cpni.ParaCpn.aszCouponMnem[2] = _T('N');
                    Cpni.ParaCpn.aszCouponMnem[3] = (TCHAR)((usI / 100) % 10) + (TCHAR)0x30;
                    Cpni.ParaCpn.aszCouponMnem[4] = (TCHAR)((usI % 100) / 10) + (TCHAR)0x30;
                    Cpni.ParaCpn.aszCouponMnem[5] = (TCHAR)(usI % 10) + (TCHAR)0x30;
                    sRetCode = OpCpnAssign(&Cpni, usLockHnd);
                }
                sRetCode = OP_PERFORM;
            }
        }
        /*=================== TEST END =============================*/
        return (sRetCode);
    } 
    return OP_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpCstrAssign(CSTRIF *pCstr, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pCstr      - Control String information to assign
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  Control String file not exist
*               OP_ABNORMAL_CSTR:   abnormal coupon number (HALT)
*               OP_LOCK:            during Locked
*
** Description: This function supports to assign a Control String record.
*===========================================================================
*/
SHORT   SerOpCstrAssign(CSTRIF *pCstr, USHORT usLockHnd)
{
    SHORT   sRetCode;

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = OpCstrAssign(pCstr, usLockHnd);
        return (sRetCode);
    } 
    return OP_NOT_MASTER;
}

#if 0
/*                      
*===========================================================================
** Synopsis:    SHORT   SerOpPluDelete(PLUIF *pPif, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pPif       - PLU information to delete
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_NOT_IN_FILE:     PLU not in file
*               OP_LOCK:            during Locked
*
** Description: This function supports to delete a PLU record.
*===========================================================================
*/
SHORT   SerOpPluDelete(PLUIF *pPif, USHORT usLockHnd)
{
    if (STUB_SELF == SstUpdateAsMaster()) {
        return OpPluDelete(pPif, usLockHnd);
    } 
    return OP_NOT_MASTER;
}
#endif

/*
*===========================================================================
** Synopsis:    SHORT   SerOpCstrDelete(CSTRIF *pCstr, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pCstr      - Control String information to delete
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  Control Storing file not exist
*               OP_ABNORMAL_CSTR:   abnormal coupon number (HALT)
*               OP_LOCK:            during Locked
*
** Description: This function supports to delete a Control String record.
*===========================================================================
*/
SHORT   SerOpCstrDelete(CSTRIF *pCstr, USHORT usLockHnd)
{
    if (STUB_SELF == SstUpdateAsMaster()) {
        return OpCstrDelete(pCstr, usLockHnd);
    } 
    return OP_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpPluFileUpdate(USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to delete a PLU record.
*===========================================================================
*/
SHORT   SerOpPluFileUpdate(USHORT usLockHnd)
{
    if (STUB_SELF == SstUpdateAsMaster()) {
        return OpPluFileUpdate(usLockHnd);
    } 
    return OP_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpPluAllRead(PLUIF *pPif, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pPif        - PLU information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_EOF:             all PLU read
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to read all PLU record.
*===========================================================================
*/
#if defined(SerOpPluAllRead)
SHORT	SerOpPluAllRead_Special(PLUIF *pPif, USHORT usLockHnd);

SHORT	SerOpPluAllRead_Debug(PLUIF *pPif, USHORT usLockHnd, char *aszFilePath, int nLineNo)
{
	char  xBuffer[256];

	sprintf (xBuffer, "SerOpPluAllRead_Debug(): husHandle = %d  File %s  lineno = %d", pPif->husHandle, RflPathChop(aszFilePath), nLineNo);
	NHPOS_ASSERT_TEXT(0, xBuffer);
	return SerOpPluAllRead_Special(pPif, usLockHnd);
}

SHORT	SerOpPluAllRead_Special(PLUIF *pPif, USHORT usLockHnd)
#else
SHORT   SerOpPluAllRead(PLUIF *pPif, USHORT usLockHnd)
#endif
{
    if (STUB_SELF == SstReadAsMaster()) {
        return OpPluAllRead(pPif, usLockHnd);
    } 
    return OP_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpRprtPluRead(PLUIF_REP *pPrp, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pPrp        - PLU information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_EOF:             all PLU read
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_PLU_FULL:        exceed PLU number
*               OP_NOT_IN_FILE:     PLU not in file
*               OP_LOCK:            during Locked
*
** Description: This function supports to read PLU record by report code.
*
*                This function is used to iterate over the PLU database returning
*                a series of PLUs that match the report number specified in the
*                PLUIF_REP member uchReport.
*
*                The member variable culCounter is a counter that is used as
*                part of the iteration to control the progress of the iteration.
*
*                A value of zero for culCounter indicates the start of the iteration.
*===========================================================================
*/
SHORT   SerOpRprtPluRead(PLUIF_REP *pPrp, USHORT usLockHnd)
{
	SHORT			sRetCode;

    if (STUB_SELF == SstReadAsMaster()) {
        return OpRprtPluRead(pPrp, usLockHnd);
    } else
	{
		CLIREQOPPARA    ReqMsgH = {0};
		CLIRESOPPARA    ResMsgH = {0};

		PifRequestSem(husCliExeNet);

		ReqMsgH.usHndorOffset = usLockHnd;
           
		memset(&CliMsg, 0, sizeof(CLICOMIF));
		CliMsg.usFunCode    = CLI_FCOPPLUREADBRC;
		CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
		CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
		CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
		CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
		CliMsg.pauchReqData = (UCHAR *)pPrp;
		CliMsg.usReqLen     = sizeof(PLUIF_REP);// * CLI_PLU_MAX_NO;
		CliMsg.pauchResData = (UCHAR *)pPrp;
		CliMsg.usResLen     = sizeof(PLUIF_REP);// * CLI_PLU_MAX_NO;

		CstSendMaster();
		sRetCode = CliMsg.sRetCode;

		PifReleaseSem(husCliExeNet);
		return sRetCode;    
	}
    return OP_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpDeptPluRead(PLUIF_DEP *Pdp, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *Pdp        - PLU information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_EOF:             all PLU read
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_PLU_FULL:        exceed PLU number
*               OP_NOT_IN_FILE:     PLU not in file
*               OP_LOCK:            during Locked
*
** Description: This function supports to read PLU record by dept number.
*
*                This function is used to iterate over the PLU database returning
*                a series of PLUs that match the department number specified in the
*                PLUIF_DEP member usDept.
*
*                The member variable culCounter is a counter that is used as
*                part of the iteration to control the progress of the iteration.
*
*                A value of zero for culCounter indicates the start of the iteration.
*===========================================================================
*/
SHORT   SerOpDeptPluRead(PLUIF_DEP *pPdp, USHORT usLockHnd)
{
	SHORT			sRetCode;

    if (STUB_SELF == SstReadAsMaster()) {
        return OpDeptPluRead(pPdp, usLockHnd);
    }else
	{
		CLIREQOPPARA    ReqMsgH = {0};
		CLIRESOPPARA    ResMsgH = {0};

		PifRequestSem(husCliExeNet);

		ReqMsgH.usHndorOffset = usLockHnd;
           
		memset(&CliMsg, 0, sizeof(CLICOMIF));
		CliMsg.usFunCode    = CLI_FCOPPLUREADBDP;
		CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
		CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
		CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
		CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
		CliMsg.pauchReqData = (UCHAR *)pPdp;
		CliMsg.usReqLen     = sizeof(PLUIF_DEP);// * CLI_PLU_MAX_NO;
		CliMsg.pauchResData = (UCHAR *)pPdp;
		CliMsg.usResLen     = sizeof(PLUIF_DEP);// * CLI_PLU_MAX_NO;

		CstSendMaster();
		sRetCode = CliMsg.sRetCode;

		PifReleaseSem(husCliExeNet);
		return sRetCode;
	}
	return SUCCESS;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpPluAbort(PLUIF *pPif, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pPif        - PLU information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_EOF:             all PLU read
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to read all PLU record.
*===========================================================================
*/
SHORT   SerOpPluAbort(USHORT husHandle, USHORT usLockHnd)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return OpPluAbort(husHandle, usLockHnd);
    } 
    return OP_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpLock(VOID);
*
** Return:      Lock Handle
*               OP_LOCK:        already locked      
*
** Description: This function supports to lock PLU/ Dept record.
*===========================================================================
*/
SHORT   SerOpLock(VOID)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return OpLock();
    } 
    return OP_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    VOID   SerOpUnLock(VOID);
*
** Return:      none
*
** Description: This function supports to unlock PLU/ Dept record.
*===========================================================================
*/
VOID   SerOpUnLock(VOID)
{
    OpUnLock();
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpMajorDeptRead(MDEPTIF *pDpi, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pDpi       - Dept information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_EOF:             all dept read
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to read a Dept record.
*===========================================================================
*/
SHORT   SerOpMajorDeptRead(MDEPTIF *pDpi, USHORT usLockHnd)
{
    return OpMajorDeptRead(pDpi, usLockHnd);
}

/******* New Functions - Promotional Pricing Item (Release 3.1) BEGIN ******/

/*
*===========================================================================
** Synopsis:    SHORT   CliOpPpiIndRead(PPIIF *pPpi, USHORT usLockHnd)
*     InPut:    usLockHnd   - handle
*     InOut:    pPpi        - PPI information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to read promotional pricing item.
*===========================================================================
*/
SHORT   CliOpPpiIndRead(PPIIF *pPpi, USHORT usLockHnd)
{
    return OpPpiIndRead(pPpi, usLockHnd);
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpPpiAssign(PPIIF *pPpi, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pPpi       - Promotional Pricing Item information to assign
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to assign a PPI record.
*===========================================================================
*/
SHORT   SerOpPpiAssign(PPIIF *pPpi, USHORT usLockHnd)
{
    SHORT   sRetCode;

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = OpPpiAssign(pPpi, usLockHnd);
        return (sRetCode);
    }
    return OP_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpPpiDelete(PPIIF *pPpi, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pPpi       - Promotional Pricing Item information to assign
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to delete a PPI record.
*===========================================================================
*/
SHORT   SerOpPpiDelete(PPIIF *pPpi, USHORT usLockHnd)
{
    SHORT   sRetCode;

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = OpPpiDelete(pPpi, usLockHnd);
        return (sRetCode);
    }
    return OP_NOT_MASTER;
}

/******* New Functions - Promotional Pricing Item (Release 3.1) END ******/

/*
*===========================================================================
** Synopsis:    SHORT   CliOpMldIndRead(MLDIF *pMld, USHORT usLockHnd)
*     InPut:    usLockHnd   - handle
*     InOut:    pMld        - MLD information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to read MLD Mnemonic File, V3.3
*===========================================================================
*/
SHORT   CliOpMldIndRead(MLDIF *pMld, USHORT usLockHnd)
{
    return OpMldIndRead(pMld, usLockHnd);
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpMldAssign(MLDIF *pMld, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pMld       - MLD information to assign
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to assign a MLD record. V3.3
*===========================================================================
*/
SHORT   SerOpMldAssign(MLDIF *pMld, USHORT usLockHnd)
{
    return OpMldAssign(pMld, usLockHnd);
}


/*                      
*===========================================================================
** Synopsis:    SHORT   SerOpDeptDelete(DEPTIF *pDpi, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pDpi       - Dept information to delete
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_NOT_IN_FILE:     PLU not in file
*               OP_LOCK:            during Locked
*
** Description: This function supports to delete a Dept record. 2172
*===========================================================================
*/
SHORT   SerOpDeptDelete(DEPTIF *pDpi, USHORT usLockHnd)
{
    if (STUB_SELF == SstUpdateAsMaster()) {
        return OpDeptDelete(pDpi, usLockHnd);
    } 
    return OP_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpDeptAllRead(DEPTIF *pDpi, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pDpi        - Dept information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_EOF:             all PLU read
*               OP_FILE_NOT_FOUND:  Dept file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to read all Dept record.
*===========================================================================
*/
SHORT   SerOpDeptAllRead(DEPTIF *pDpi, USHORT usLockHnd)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return OpDeptAllRead(pDpi, usLockHnd);
    } 
    return OP_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT  SerPluEnterCritMode(USHORT usFile,
*                                           USHORT usFunc,
*                                           USHORT *pusHandle,
*                                           VOID   *pvParam);
*     InPut:    usFile      File
*               usFunc      Function
*               pvParam     Option parameter
*     InOut:    pusHandle   Handle
*
** Return:      SPLU_COMPLETED         * successfully completed
*               SPLU_INVALID_DATA      * invalid data given
*               SPLU_TOO_MANY_USERS    * too many users accessed
*
** Description:
*   This function supports to enter a critical mode.
*===========================================================================
*/
SHORT  SerPluEnterCritMode(USHORT usFile, USHORT usFunc, USHORT *pusHandle, VOID *pvParam)
{
    SHORT          sRetCode;

    PifRequestSem(husCliExeNet);
    sRetCode = CstPluEnterCritMode(usFile, usFunc, pusHandle, pvParam, 0);
    PifReleaseSem(husCliExeNet);
    return (sRetCode);
}

/*
*===========================================================================
** Synopsis:    SHORT  CstPluEnterCritMode(USHORT usFile,
*                                           USHORT usFunc,
*                                           USHORT *pusHandle,
*                                           VOID   *pvParam,
*                                           USHORT usUser)
*     InPut:    usFile      File
*               usFunc      Function
*               pvParam     Option parameter
*               usUser      Define user (Super or Isp)
*     InOut:    pusHandle   Handle
*
** Return:      SPLU_COMPLETED         * successfully completed
*               SPLU_INVALID_DATA      * invalid data given
*               SPLU_TOO_MANY_USERS    * too many users accessed
*
** Description: This function supports to enter a critical mode.
*
*               WARNING:  This function must be called AFTER a call to
*                         function PifRequestSem(husCliExeNet) in order
*                         to gain access to the CliMsg global buffer used
*                         by the Client subsystem for network communication
*                         with the Master Terminal.
*===========================================================================
*/
SHORT  CstPluEnterCritMode(USHORT usFile, USHORT usFunc, 
                                USHORT *pusHandle, VOID *pvParam, USHORT usUser)
{
    CLIRESPLUECM   *pResp = (CLIRESPLUECM *)CliRcvBuf.auchData;
	CLIREQPLUECM   ReqMsgH = {0};
    CLIRESPLUECM   ResMsgH = {0};
    SHORT          sRetCode, sBRetCode;
    USHORT         fsOption;

    ReqMsgH.usFile      = usFile;
    ReqMsgH.usFunc      = usFunc;
    fsOption = 0;                    /* Clear option */

    switch (usFunc) {
    case FUNC_INDIV_LOCK:    /* indiv. access with lock */
        fsOption = REPORT_AS_RESET;
        break;

    case FUNC_REPORT_RANGE:  /* report by range         */
        memcpy(ReqMsgH.auchPara, pvParam, sizeof(ECMRANGE));
        fsOption = ((ECMRANGE *)pvParam)->fsOption;
        break;

    case FUNC_REPORT_ADJECT: /* report by adjective item */
        memcpy(ReqMsgH.auchPara, pvParam, sizeof(ECMADJECT));
        fsOption = ((ECMADJECT *)pvParam)->fsOption;
        break;

    case FUNC_REPORT_COND:   /* report by conditional   */
        memcpy(ReqMsgH.auchPara, pvParam, sizeof(ECMCOND));
        fsOption = ((ECMCOND *)pvParam)->fsOption;
        break;

    case FUNC_REPORT_MAINTE: /* report mainte. records  */
        memcpy(ReqMsgH.auchPara, pvParam, sizeof(ECMMAINTE));
        break;

    case FUNC_APPLY_MAINTE:  /* apply mainte. reocrds   */
        memcpy(ReqMsgH.auchPara, pvParam, sizeof(ECMAPPLY));
        fsOption = REPORT_AS_RESET;
        break;

    case FUNC_PURGE_MAINTE:  /* purge mainte. records   */
        memcpy(ReqMsgH.auchPara, pvParam, sizeof(ECMPURGE));
        fsOption = REPORT_AS_RESET;
        break;

    default:
		PifLog(MODULE_STB_LOG, LOG_ERROR_CLIENT_INVALID_FUNCODE);
        break;
    }

    if (CLI_IAM_BMASTER) {
        fsOption = 0;             /* clear option */
    }
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCPLUENTERCRITMD | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQPLUECM);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESPLUECM);

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = PluEnterCritMode(usFile, usFunc, pusHandle, pvParam);
    } else {
        sRetCode = STUB_NOT_MASTER;
    }
    if (SPLU_COMPLETED == sRetCode) {

        if ( fsOption & REPORT_WITH_LOCK ) {
            fsOption |= REPORT_AS_RESET;     /* Set Reset Flag */
        }

        if ( 0 == usUser ) {
            CliMMMHand.hFst.usMTHand = *pusHandle;     /* Super */
            CliMMMHand.hFst.fsOption = fsOption;
            CliMMMHand.hFst.usFile   = usFile;
        } else {
            CliMMMHand.hSnd.usMTHand = *pusHandle;     /* Isp   */
            CliMMMHand.hSnd.fsOption = fsOption;
            CliMMMHand.hSnd.usFile   = usFile;
        }

        if ( fsOption & REPORT_AS_RESET ) {     /* if Reset Flag on , then backup */

            if (CLI_IAM_MASTER) {                   /* I am Master ? */
                sBRetCode = CstSendBMaster();       /* Send to B Master */
                if (SPLU_COMPLETED == sBRetCode) {
                    if ( 0 == usUser ) {
                        CliMMMHand.hFst.usBMHand = pResp->usHandle; /* Sup */
                    } else {
                        CliMMMHand.hSnd.usBMHand = pResp->usHandle; /* Isp */
                    }
                }
            }
        }
    }
    return (sRetCode);
}

/*
*===========================================================================
** Synopsis:    SHORT  SerPluExitCritMode(USHORT usHandle,
*                                          USHORT usReserve)
*
*     InPut:    usHandle    Handle
*               usReseve
*
*     InOut:    noting
*
** Return:      SPLU_COMPLETED         * successfully completed
*               SPLU_INVALID_HANDLE    * invalid handle
*
** Description:
*   This function supports to exit crit mode.
*===========================================================================
*/
SHORT  SerPluExitCritMode(USHORT usHandle, USHORT usReserve)
{
    SHORT          sRetCode;

    PifRequestSem(husCliExeNet);
    sRetCode = CstPluExitCritMode(usHandle, usReserve, 0);
    PifReleaseSem(husCliExeNet);
    return (sRetCode);
}

/*
*===========================================================================
** Synopsis:    SHORT  SerIspPluEnterCritMode(USHORT usFile,
*                                              USHORT usFunc,
*                                              USHORT *pusHandle,
*                                              VOID   *pvParam);
*     InPut:    usFile      File
*               usFunc      Function
*               pvParam     Option parameter
*     InOut:    pusHandle   Handle
*
** Return:      SPLU_COMPLETED         * successfully completed
*               SPLU_INVALID_DATA      * invalid data given
*               SPLU_TOO_MANY_USERS    * too many users accessed
*
** Description:
*   This function supports to enter a critical mode.
*===========================================================================
*/
SHORT  SerIspPluEnterCritMode(USHORT usFile, USHORT usFunc, USHORT *pusHandle, VOID *pvParam)
{
    SHORT          sRetCode;

    PifRequestSem(husCliExeNet);
    sRetCode = CstPluEnterCritMode(usFile, usFunc, pusHandle, pvParam, CLI_FCPLUENTERCRITMD);
    PifReleaseSem(husCliExeNet);
    return (sRetCode);
}

/*
*===========================================================================
** Synopsis:    SHORT  SerIspPluExitCritMode(USHORT usHandle,
*                                             USHORT usReserve)
*
*     InPut:    usHandle    Handle
*               usReseve
*
*     InOut:    noting
*
** Return:      SPLU_COMPLETED         * successfully completed
*               SPLU_INVALID_HANDLE    * invalid handle
*
** Description:
*   This function supports to exit crit mode.
*===========================================================================
*/
SHORT  SerIspPluExitCritMode(USHORT usHandle, USHORT usReserve)
{
    SHORT          sRetCode;

    PifRequestSem(husCliExeNet);
    sRetCode = CstPluExitCritMode(usHandle, usReserve, CLI_FCPLUEXITCRITMD);
    PifReleaseSem(husCliExeNet);

	if (sRetCode != 0) {
		char xBuff[128];
		sprintf (xBuff, "SerIspPluExitCritMode() CstPluExitCritMode() returned %d, usHandle %d", sRetCode, usHandle);
		NHPOS_NONASSERT_TEXT(xBuff)
	}
    return (sRetCode);
}

/*
*===========================================================================
** Synopsis:    SHORT  CstPluExitCritMode(USHORT usHandle,
*                                          USHORT usReserve,
*                                          USHORT usUser)
*
*     InPut:    usHandle    Handle
*               usReseve    Alway 0
*               usUser      Sup or Isp
*
*     InOut:    noting
*
** Return:      SPLU_COMPLETED         * successfully completed
*               SPLU_INVALID_HANDLE    * invalid handle
*
** Description:
*   This function supports to exit crit mode.
*===========================================================================
*/
SHORT  CstPluExitCritMode(USHORT usHandle, USHORT usReserve, USHORT usUser)
{
	CLIREQPLUCOM   ReqMsgH = {0};
    CLIRESPLUCOM   ResMsgH = {0};
    SHORT          sRetCode;
    USHORT         fsOption;

    ReqMsgH.usFileorHnd = CstPluGetBMHandle(usHandle);
    ReqMsgH.usReserve   = 0;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCPLUEXITCRITMD | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQPLUCOM);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESPLUCOM);

    if (STUB_SELF == SstUpdateAsMaster()) {
        PluCancelReserved(usHandle);
        sRetCode = PluExitCritMode(usHandle, usReserve);
    } else {
        sRetCode = STUB_NOT_MASTER;
    }

    if ( 0 == usUser ) {
        fsOption = CliMMMHand.hFst.fsOption;     /* Super */
        CliMMMHand.hFst.usMTHand = 0;
        CliMMMHand.hFst.usBMHand = 0;
    } else {
        fsOption = CliMMMHand.hSnd.fsOption;     /* Isp   */
        CliMMMHand.hSnd.usMTHand = 0;
        CliMMMHand.hSnd.usBMHand = 0;
    }

    if ( fsOption & REPORT_AS_RESET ) {
        if (SPLU_COMPLETED == sRetCode) {
            SstUpdateBackUp();
        }
    }
    return (sRetCode);
}

/*
*===========================================================================
** Synopsis:    USHORT  CstPluGetBMHandle(USHORT usHandle)
*
*     InPut:    usHandle  - Master terminal handle
*     InOut:    nothing
*
** Return:      B-Master handle
*
** Description:
*   This function supports to get b-master handle.
*===========================================================================
*/
USHORT  CstPluGetBMHandle(USHORT usHandle)
{
    if (CliMMMHand.hFst.usMTHand == usHandle ) {
        return (CliMMMHand.hFst.usBMHand);
    } else {
        return (CliMMMHand.hSnd.usBMHand);
    }
}

/*
*===========================================================================
** Synopsis:    SHORT  SerPluReportRecord(USHORT  usHandle,
*                                          VOID    *pvBuffer,
*                                          VOID    *pusRet);
*     InPut:    usHandle    Handle
*
*     InOut:    pvBuffer    Read   data
*               pusRet      Return length
*
** Return:      SPLU_COMPLETED         * successfully completed
*               SPLU_INVALID_HANDLE    * invalid handle
*               SPLU_FILE_NOT_FOUND    * file   not found
*               SPLU_REC_LOCKED        * record locked
*               SPLU_FUNC_CONTINUED    * function continued
*               SPLU_END_OF_FILE       * end of a file
*
** Description:
*   This function supports to report a record.
*===========================================================================
*/
SHORT  SerPluReportRecord(USHORT usHandle, VOID *pvBuffer, USHORT *pusRet)
{
    SHORT           sRetry, sRetryCo, sError;
    USHORT          fsOption, usFile;
	CLIREQPLURPT    ReqMsgH = {0};
    CLIRESPLURPT    ResMsgH = {0};

    PifRequestSem(husCliExeNet);

    if (CliMMMHand.hFst.usMTHand == usHandle ) {
        ReqMsgH.usFileorHnd = CliMMMHand.hFst.usBMHand;
        fsOption = CliMMMHand.hFst.fsOption;
        usFile   = CliMMMHand.hFst.usFile;
    } else {
        ReqMsgH.usFileorHnd = CliMMMHand.hSnd.usBMHand;
        fsOption = CliMMMHand.hSnd.fsOption;
        usFile   = CliMMMHand.hSnd.usFile;
    }

    if ( 0 == ( fsOption & REPORT_AS_RESET ) ) {
        PifReleaseSem(husCliExeNet);
    }

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetryCo = sRetry = CstReadMDCPara(CLI_MDC_RETRY);
        do {
            sError = PluReportRecord(usHandle, pvBuffer, pusRet);

            if (SPLU_REC_LOCKED == sError) {
                if (0 > --sRetryCo) {
                    break;
                 /*   CstDisplayError(STUB_IAM_BUSY);    */
                 /*   sRetryCo = sRetry;                 */
                } else {
                    PifSleep (750);  // CstSleep ();
                }
            }
        } while (SPLU_REC_LOCKED == sError);

        if ( 0 == ( fsOption & REPORT_AS_RESET ) ) {
            return(sError);
        }
    } else {
        if ( fsOption & REPORT_AS_RESET ) {
            PifReleaseSem(husCliExeNet);
        }
        return (STUB_NOT_MASTER);
    }

    if ( (SPLU_COMPLETED == sError) || (SPLU_END_OF_FILE == sError) || (SPLU_FUNC_CONTINUED  == sError ) ) {
        memset(&CliMsg, 0, sizeof(CLICOMIF));
        CliMsg.usFunCode    = CLI_FCPLUREPORTREC | CLI_FCWBACKUP | CLI_FCSERVERWB;
        CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
        CliMsg.usReqMsgHLen = sizeof(CLIREQPLURPT);
        CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
        CliMsg.usResMsgHLen = sizeof(CLIRESPLURPT);

        SstUpdateBackUp();
    }
    PifReleaseSem(husCliExeNet);
    return sError;
}

/*
*==========================================================================
**    Synopsis:   SHORT   CliReqBackupMMM(USHORT usFile, USHORT usFun)
*
*       Input:    USHORT    usFile
*                 USHORT    usFun
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   STUB_SUCESS
*                 CLI_ERROR_BAK_COMM
*                 CLI_ERROR_BAK_FULL
*
**  Description:    Request MMM File Data for Backup.
*==========================================================================
*/
SHORT   CliReqBackupMMM(USHORT usFile, USHORT usFun)
{
    CLI_MMM_BACKUP  MMM_BakSnd, *pMMM_BakRcv;
    UCHAR           auchRcvBuf[(OP_BACKUP_WORK_SIZE + sizeof(CLI_MMM_BACKUP))];
    ECMBLOCK        ECM = {0};
    USHORT			usHand;
	SHORT           sStatus;

    /*  Get handle  */
    ECM.fsOption = BLOCK_WRITE;
    ECM.usReserve = 0;

    sStatus = PluEnterCritMode(usFile, FUNC_BLOCK_ACCESS, &usHand, &ECM);

    if (SPLU_COMPLETED != sStatus) {
        if (SPLU_FILE_NOT_FOUND == sStatus) {
            return(STUB_SUCCESS);             /* if file not found, then ok */
        }
        return(sStatus);
    }

    memset(&MMM_BakSnd, 0, sizeof(CLI_MMM_BACKUP));
    pMMM_BakRcv = (CLI_MMM_BACKUP *)auchRcvBuf;
    MMM_BakSnd.usFile = usFile;
    MMM_BakSnd.lFilePosition = 0L;
    MMM_BakSnd.usSeqNo = 1;

    for (;;) {
		USHORT  usRcvLen,  usWriteLen;

        usRcvLen = (OP_BACKUP_WORK_SIZE + sizeof(CLI_MMM_BACKUP));

        if  ((sStatus = SstReqBackUp(usFun, (UCHAR *)&MMM_BakSnd, sizeof(CLI_MMM_BACKUP), auchRcvBuf, &usRcvLen)) != 0 ) {
            break;
        }
        if ( sizeof(CLI_MMM_BACKUP) >  usRcvLen ) {
            sStatus = CLI_ERROR_BAK_COMM;
            break;
        }
        if (pMMM_BakRcv->usSeqNo != MMM_BakSnd.usSeqNo ) {
            sStatus = CLI_ERROR_BAK_COMM;
            break;
        }
        if (0 == pMMM_BakRcv->usDataLen) {      /* End of File */
            break;
        }
        /* Wrte Data */
        sStatus = PluWriteBlock(usHand, &auchRcvBuf[sizeof(CLI_MMM_BACKUP)], pMMM_BakRcv->usDataLen, &usWriteLen);
        if (SPLU_COMPLETED != sStatus)  {
            break;
        }
        if (usWriteLen != pMMM_BakRcv->usDataLen) {
            sStatus = CLI_ERROR_BAK_FULL;
            break;
        }
        MMM_BakSnd.lFilePosition += (LONG)pMMM_BakRcv->usDataLen;
        MMM_BakSnd.usSeqNo++;
    }

    PluExitCritMode(usHand, 0);
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   CliResBackupMMM(UCHAR   *puchRcvData,
*                                         USHORT  usRcvLen,
*                                         UCHAR   *puchSndData,
*                                         UCHAR   *pusSndLen)
*
*       Input:    UCHAR         *puchRcvData        * Pointer to Request Data *
*                 USHORT        usRcvLen            * Width of puchRcvData *
*      Output:    UCHAR         *puchSndData        * Storage location for backup data. *
*       InOut:    USHORT        *pusSndLen          * Width of puchSndData *
*
**  Return    :   STUB_SUCCESS
*                 CLI_ERROR_BAK_COMM
*
**  Description:    Response MMM File Data for Backup for function code CLI_FCBAKPLU.
*==========================================================================
*/
SHORT   CliResBackupMMM(UCHAR *puchRcvData, USHORT usRcvLen, UCHAR *puchSndData, USHORT *pusSndLen)
{
    CLI_MMM_BACKUP  *pMMM_BakRcv = (CLI_MMM_BACKUP *)puchRcvData;
    CLI_MMM_BACKUP  *pMMM_BakSnd = (CLI_MMM_BACKUP *)puchSndData;
    LONG            lRet;
	ULONG           ulDataLen;
    ECMBLOCK        ECM = {0};
    SHORT           sStatus;
    USHORT          usHand;

    pMMM_BakSnd->lFilePosition = pMMM_BakRcv->lFilePosition;
    pMMM_BakSnd->usSeqNo       = pMMM_BakRcv->usSeqNo;

    if (usRcvLen < sizeof(CLI_MMM_BACKUP)) {
        return (CLI_ERROR_BAK_COMM);
    }

    /* Check current record number */
    if ( 1 == pMMM_BakRcv->usSeqNo ) {
		MFINFO    Mas;

        sStatus = PluSenseFile(pMMM_BakRcv->usFile, &Mas);  /* get info. of a file  */

        if (SPLU_FILE_NOT_FOUND == sStatus ) {
            return(sStatus);
        } else if (SPLU_COMPLETED == sStatus ) {
            if ( 0 == Mas.ulCurRec ) {
                pMMM_BakSnd->usDataLen = 0;
                sStatus = SPLU_COMPLETED;
                return(sStatus);
            }
        } else {
            return(sStatus);
        }
    }
    /*  Get handle  */
    ECM.fsOption = BLOCK_READ;
    ECM.usReserve = 0;

    sStatus = PluEnterCritMode(pMMM_BakRcv->usFile, FUNC_BLOCK_ACCESS, &usHand, &ECM);
    if (SPLU_COMPLETED != sStatus) {
        return(sStatus);
    }
    *pusSndLen = sizeof(CLI_MMM_BACKUP);

    /*  File Seek */
    sStatus = PluSeekBlock(usHand, pMMM_BakRcv->lFilePosition, PLU_SB_TOP, &lRet);
    if (SPLU_COMPLETED != sStatus) {
        if ((1 != pMMM_BakRcv->usSeqNo) && (SPLU_FILE_OVERFLOW == sStatus)) {
            pMMM_BakSnd->usDataLen = 0;
            sStatus = SPLU_COMPLETED;
        }
        PluExitCritMode(usHand, 0);
        return(sStatus);
    }
    /* Get Data */
    sStatus = PluReadBlock(usHand, puchSndData + sizeof(CLI_MMM_BACKUP), OP_BACKUP_WORK_SIZE, &ulDataLen);

	NHPOS_ASSERT(ulDataLen <= 0xFFFF/*MAXWORD*/);
	pMMM_BakSnd->usDataLen = (USHORT)ulDataLen;

    if (SPLU_COMPLETED != sStatus) {
        if ((1 != pMMM_BakRcv->usSeqNo) && (SPLU_END_OF_FILE == sStatus)) {
            pMMM_BakSnd->usDataLen = 0;
            sStatus = SPLU_COMPLETED;
        }
        PluExitCritMode(usHand, 0);
        return(sStatus);
    }

    *pusSndLen += pMMM_BakSnd->usDataLen;
    PluExitCritMode(usHand, 0);
    return (STUB_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:   SHORT   CliConvertErrorMMM(SHORT sStatus)
*
*       Input:    USHORT   usStatus
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Converted error code
*
**  Description:
*               Convert MMM error to opepara error.
*==========================================================================
*/
#if defined(CliConvertErrorMMM)
USHORT   CliConvertErrorMMM_Special(SHORT sStatus);
USHORT   CliConvertErrorMMM_Debug(SHORT sStatus, char *aszFilePath, int nLineNo)
{
	if (sStatus != SPLU_COMPLETED) {
		char  xBuffer[256];

		sprintf (xBuffer, "CliConvertErrorMMM_Debug(): sStatus = %d  File %s  lineno = %d", sStatus, RflPathChop(aszFilePath), nLineNo);
		NHPOS_ASSERT_TEXT(0, xBuffer);
	}
	return CliConvertErrorMMM_Special(sStatus);
}

USHORT   CliConvertErrorMMM_Special(SHORT sStatus)
#else
USHORT   CliConvertErrorMMM(SHORT sStatus)
#endif
{
    SHORT   sError;

    switch ( sStatus ) {
		case SPLU_COMPLETED :     /* successfully completed */
			sError = OP_PERFORM ;         /* Success    */
			break;

		case SPLU_FILE_NOT_FOUND : /* file not found            */
			sError = OP_FILE_NOT_FOUND ;  /* Error   -1  */
			break;

		case SPLU_DEVICE_FULL :   /* not enough memory */
		case SPLU_FILE_OVERFLOW :  /* file overflowed   */
			sError = OP_DEPT_FULL ;       /* Error   -2  */
			break;

		case SPLU_REC_NOT_FOUND :  /* record not found  */
			sError = OP_NOT_IN_FILE ;     /* Error   -3  */
			break;

		case SPLU_FILE_LOCKED  :  /* file locked     */
		case SPLU_REC_LOCKED   :  /* record locked */
			sError = OP_LOCK  ;           /* Error   -5  */
			break;

		case CLI_ERROR_BAK_FULL:
			sError = OP_COMM_ERROR;       /* Error   -37  */
			break;

		case CLI_ERROR_BAK_COMM:
			sError = OP_COMM_ERROR;       /* Error   -37  */
			break;

		default:
			sError = OP_NOT_MASTER - 1 ;   /* Error  -19  */
			break;
    }

	if (sError != OP_PERFORM) {
		PifLog (MODULE_STB_LOG, LOG_EVENT_STB_CONVERTERROR_MMM);
		PifLog (MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)(abs(sError)));
		PifLog (MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)(abs(sStatus)));
	}
    return (sError);
}

/*
*===========================================================================
** Synopsis:    SHORT   CliOpPluOepRead(OPPLUOEPIF *pPlu, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pPif        - PLU information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_NOT_IN_FILE:     PLU not in file
*               OP_LOCK:            during Locked
*
** Description: This function supports to read a PLU record.        Saratoga
*===========================================================================
*/
SHORT   CliOpPluOepRead(OPPLUOEPIF *pPlu, USHORT usLockHnd)
{
    SHORT           sRetCode;
    SHORT           sCliRetCode, sErrorBM = 0, sErrorM = 0;
	USHORT          usComReadStatus = 0;
	CLIREQOEPPLU    ReqMsgH = {0};
    CLIRESOEPPLU    ResMsgH = {0};

    PifRequestSem(husCliExeNet);

    memset(&CliMsg,  0, sizeof(CLICOMIF));
    
    memcpy(ReqMsgH.auchPluNo, pPlu->PluNo.auchPluNo, sizeof(OPPLUOEPPLUNO/*OPPLUOEPIF XP_PORT*/));

    CliMsg.usFunCode    = CLI_FCOPPLUOEPREAD | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOEPPLU);

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOEPPLU);
    CliMsg.sRetCode     = OP_PERFORM;

	if (CLI_STS_M_OFFLINE != ((usComReadStatus = CstComReadStatus()) & (CLI_STS_M_OFFLINE | CLI_STS_M_REACHABLE))) {
		if (STUB_SELF == (sErrorM = CstSendMaster())) {     /* --- Send Master Terminal --- */
			CliMsg.sRetCode = OpPluOepRead(&pPlu->PluNo, (OPPLUOEPPLUNAME *)&ResMsgH.auchPluName, usLockHnd);
			sErrorM = STUB_SUCCESS;
		}
		if (STUB_TIME_OUT == sErrorM) {
			// if we have a time out when talking to the Master Terminal then
			// lets assume that it is down.
			sErrorM = STUB_M_DOWN;
		}
	} else {
		sErrorM = STUB_M_DOWN;
	}

	sCliRetCode = CliMsg.sRetCode;

    if (sErrorM == STUB_M_DOWN) {
        if (STUB_SELF == (sErrorBM = CstSendBMaster())) {     /* --- Send BMaster Terminal --- */
			CliMsg.sRetCode = OpPluOepRead(&pPlu->PluNo, (OPPLUOEPPLUNAME *)&ResMsgH.auchPluName, usLockHnd);
        }
		else {
			// If there was an error talking to the Backup Master Terminal
			// then we will return the status of the Master Terminal request.
			// We depend on the CstSendBMasterFH () function to handle the
			// error condition indication to the operator and the caller
			// to display an error dialog if the Master Terminal is down.
			if (0 == (CliNetConfig.fchStatus & CLI_NET_BACKUP)) {
				// If there is not a Backup Master then return the
				// status of the Master Terminal regardless.
				CliMsg.sRetCode = sCliRetCode;
			}
		}
    }

	// NOTE:  OPPLUOEPIF is a union so the PLU numbers are being overwritten.
	memcpy(&(pPlu->PluName), &ResMsgH.auchPluName, sizeof(OPPLUOEPPLUNAME));

	// Get a copy of the return code in a local variable before exiting
	// the critical region protecting access to the global CliMsg in
	// case some other thread is waiting for it.
	sRetCode = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);

	if (sErrorM == STUB_M_DOWN) {
//		CstMTOffLine();
	}
	if (sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		char xBuff[128];
		sprintf (xBuff, "==NOTE: CliOpPluOepRead() ComReadStatus 0x%x, sRetCode %d, sErrorM %d, sErrorBM %d", usComReadStatus, sRetCode, sErrorM, sErrorBM);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return sRetCode;
}
/*===== END OF SOURCE =====*/