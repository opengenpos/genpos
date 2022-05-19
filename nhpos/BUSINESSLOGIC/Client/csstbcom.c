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
* Title       : Client/Server STUB module, Source File                        
* Category    : Client/Server STUB, internal functions
* Program Name: CSSTBCOM.C                                            
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
*       
*       CstSleep();                     Sleep function
*       CstNetOpen();                   Net work open
*       CstNetClose();                  Net work close
*       CstNetClear();                  Net work receive buffer clear 
*       CstNetSend();                   Net work send
*   C   CstNetReceive();                Net work receive
*       CstSendTarget();                Send Target terminal
*       CstCpmRecMessage();             Receive response message
*       
*       CstSendMasterFH();              Send request to Master(file handle I/F version)
*       CstSendBMasterFH();             Send request to Back Up Master(file handle I/F version)
*       CstComSendReceiveFH();          Send request and receive response(file handle I/F version)
*       SstReadFAsMasterFH();           Read File from as master by comm(file handle I/F version)
*       CstComRespHandleFH();           response message handling(file handle I/F version)
*       SstReadFileFH();                Read data from target file
*       SstWriteFileFH();               Write data to target file
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* Jun-08-93:H.Yamaguchi: Adds Enhance function.
* Mar-15-94:K.You      : Adds flex GC file feature(add CstSendMasterFH,
*          :           : CstSendBMasterFH, CstComSendReceiveFH, CstComMakeMultiDataFH,
*          :           : SstComRespHandleFH, SstReadFileFH, SstReadFAsMasterFH)
* Jun-27-95:M.Suzuki   : Not Disp. Flashing for SPS Polling (Chg CstNetReceive, CstComErrHandle)
* Apr-04-96:T.Nakahata : Add CliEJRead at ComRespHandle()
*
** NCR2171 **                                         
* Aug-26-99:M.Teraki   :initial (for 2171)
* Dec-16-99:hrkato     : Saratoga
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
#include	<Windows.h>
#include	<tchar.h>
#include    <memory.h>
#include    <math.h>
#include    <stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <para.h>
#include    <nb.h>
#include    <uie.h>
#include    <uireg.h>
#include    <fsc.h>
#include    <log.h>
#include    <csstbpar.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csstbstb.h>
#include    <csserm.h>
#include    <appllog.h>
#include    <cpm.h>
#include    <ej.h>
#include    <rfl.h>
#include    "csstubin.h"
#include	"csetk.h" //cwunn 6/30/03 Burger King ETK issue
#include	"csttl.h"
#include	"pararam.h"


// This #if construct provides a way for a developer doing testing to
// make a Master terminal perform actions similar to a Satellite terminal.
// To enable the functionality define  FORCE_SATELLITE_TERMINAL which will then
// enable the #if clause.  If it is not wanted, comment it out
//   WARNING:  See function CstCheckMasterRoleAssummed() which may be called from
//             both Satellite and Master.  You may need to help that function with
//             a break point.
//#define FORCE_SATELLITE_TERMINAL 1
//#define FORCE_BACKUP_TERMINAL    1

/*
------------------------------------------
    Internal Work Area (initialized)
------------------------------------------
*/
static SHORT   sMasterErrorSave = 0;      /* save the last error from Master Terminal to reduce repeated logs */
static SHORT   sBackMasterErrorSave = 0;  /* save the last error from Backup Master Terminal to reduce repeated logs */
static USHORT  usCstfsSystemInf = 0;      /* Client side special Notice Board status addition */

/* Function prototypes for static functions located in this file. */
static SHORT   CstComChekInquiry(VOID);
static VOID    CstComMakeMessage(USHORT usServer, USHORT usPort, CLICOMIF *pCliMsg, XGRAM *pCliSndBuf);
static VOID    CstComSendReceive(CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf);
static USHORT  CstComMakeMultiData(CLICOMIF *pCliMsg, XGRAM *pCliSndBuf);
static VOID    CstComErrHandle(CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf);
static VOID    CstComSendConfirm(CLICOMIF *pCliMsg, XGRAM *pCliSndBuf);
static USHORT  CstComMakeMultiDataFH(CLICOMIF *pCliMsg, XGRAM *pCliSndBuf);
static VOID    CstComSendReceiveFH(USHORT usType, CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf);
static SHORT   CstComRespHandleFH(CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf);


// New logging functions.
VOID    CstLogNetSend (char *aszPrintLineFirst)
{
	extern XGRAM       CliSndBuf;          /* Send Buffer */    
	CLIREQCOM   *pSend = (CLIREQCOM *)CliSndBuf.auchData;
	int         iLength = 0;
	PIFUSRTBLINFO  usrStats;
	char        aszPrintLine[255];

	PifNetControl(CliNetConfig.usHandle, PIF_NET_GET_USER_INFO, &usrStats);

	iLength += sprintf (aszPrintLine, " %-30.75s usrStats.usLastSenduchNo %d CLIREQCOM:usFunCode 0x%4.4x usSeqNo 0x%4.4x ",
		aszPrintLineFirst, usrStats.usLastSenduchNo, pSend->usFunCode, pSend->usSeqNo);

	CstLogLine (&aszPrintLine[0]);
}

VOID    CstLogNetRecv (char *aszPrintLineFirst)
{
	extern XGRAM       CliRcvBuf;          /* Receive Buffer */
	CLIRESCOM   *pResp = (CLIRESCOM *)CliRcvBuf.auchData;
	int         iLength = 0;
	PIFUSRTBLINFO  usrStats;
	char        aszPrintLine[255];

	PifNetControl(CliNetConfig.usHandle, PIF_NET_GET_USER_INFO, &usrStats);

	iLength += sprintf (aszPrintLine, " %-30.75s usrStats.usLastRecvuchNo %d CLIRESCOM: usFunCode 0x%4.4x usSeqNo 0x%4.4x sResCode %4.4d sReturn %4.4d",
		aszPrintLineFirst, usrStats.usLastRecvuchNo, pResp->usFunCode, pResp->usSeqNo, pResp->sResCode, pResp->sReturn);

	CstLogLine (&aszPrintLine[0]);
}

SHORT    CstLogLine (char *aszPrintLine)
{
	// sLogFile may have the following values:
	//   1 -> environment variable set, file open attempt succeeded
	//   0 -> starting position, no check yet done, no file open attempt
	//  -1 -> check on environment done and no environment variable set
	//  -2 -> environment variable set however file open attempt failed
	static  SHORT    sLogFile = 0;

	if (sLogFile == 0) {
		TCHAR tchEnvVal[10];

		sLogFile = -1;
		if (GetEnvironmentVariable(_T("PIF_LOG_ON"), tchEnvVal, 10)) {
			sLogFile = 1;
		}
	}

	if (sLogFile > 0 && aszPrintLine) {
		static  FILE * hFile = 0;
		static  CHAR   fileHeader[32] = {0};
		static  ULONG  ulFileMaxRecords = 64000L;
		static  ULONG  ulFileCurrentRecord = 0L;
		char    *pLogFileName = "C:\\FlashDisk\\NCR\\Saratoga\\Database\\PCIFLOG";
		char    xBuff[324];
		char    formatHeader[] = "%8.8d,%8.8d             ";

		formatHeader[23] = '\n';
		if (hFile == 0) {
			size_t readBytes = 0;

			hFile = fopen  (pLogFileName, "r+b");
			if (hFile) {
				readBytes = fread (fileHeader, sizeof(char), 24, hFile);
				if (readBytes < 24) {
					sprintf (fileHeader, formatHeader, ulFileMaxRecords, ulFileCurrentRecord);
					fileHeader[23] = '\n';
					fseek (hFile, 0L, SEEK_SET);
					fwrite (fileHeader, sizeof(char), 24, hFile);
				} else {
					ulFileMaxRecords = atol (fileHeader);
					ulFileCurrentRecord = atol (fileHeader + 9);
					if (ulFileMaxRecords < 10000L) ulFileMaxRecords = 32000L;
				}
			} else {
				hFile = fopen  (pLogFileName, "w+b");
				if (hFile) {
					sprintf (fileHeader, formatHeader, ulFileMaxRecords, ulFileCurrentRecord);
					fileHeader[23] = '\n';
					fseek (hFile, 0L, SEEK_SET);
					fwrite (fileHeader, sizeof(char), 24, hFile);
				}
			}
		}
		if (hFile == NULL) {
			sLogFile = -2;
		} else if (hFile && aszPrintLine) {
			int         iLength = 0;
			SYSTEMTIME  DateTime;

			GetLocalTime(&DateTime);
			//format the string to tell the time, date, thread name
			iLength += sprintf(xBuff, "%02d/%02d/%02d,%02d:%02d:%02d.%4.4d %-240.240s",
				DateTime.wMonth, DateTime.wDay, DateTime.wYear%100, DateTime.wHour, DateTime.wMinute, DateTime.wSecond, DateTime.wMilliseconds,
				aszPrintLine);
			xBuff[255] = '\n';
			fseek (hFile, (ulFileCurrentRecord * 256 + 24), SEEK_SET);
			fwrite (xBuff, sizeof(char), 256, hFile);

			ulFileCurrentRecord = ((ulFileCurrentRecord + 1) % ulFileMaxRecords);
			sprintf (fileHeader, formatHeader, ulFileMaxRecords, ulFileCurrentRecord);
			fileHeader[23] = '\n';
			fseek (hFile, 0L, SEEK_SET);
			fwrite (fileHeader, sizeof(char), 24, hFile);
			fflush (hFile);
		}
	}

	return (sLogFile);
}

/*
*===========================================================================
** Synopsis:    SHORT    CstIamMaster(VOID);
*
** Return:      STUB_SUCCESS:   I am Master Terminal
*               STUB_ILLEGAL:   I am not Mater Terminal
*
** Description:  This function ckeck to "Am I Master "
*                This function CstIamMaster() used by CLI_IAM_MASTER.
*===========================================================================
*/
SHORT   CstIamMaster(VOID)
{
    if (CLI_TGT_MASTER == CliNetConfig.auchFaddr[CLI_POS_UA]) {
        return STUB_SUCCESS;
    } 
    return STUB_ILLEGAL;
}

/*
*===========================================================================
** Synopsis:    SHORT    CstIamBMaster(VOID);
*
** Return:      STUB_SUCCESS:   I am Back Up Master Terminal
*               STUB_ILLEGAL:   I am not Back Up Mater Terminal
*
** Description:  This function check "Am I B-Master"
*===========================================================================
*/
SHORT   CstIamBMaster(VOID)
{
    if ((CLI_TGT_BMASTER == CliNetConfig.auchFaddr[CLI_POS_UA]) && (CliNetConfig.fchStatus & CLI_NET_BACKUP)) {
        return STUB_SUCCESS;
    }
    return STUB_ILLEGAL;
}

/*
*===========================================================================
** Synopsis:    SHORT    CstIamDisconnectedSatellite(VOID);
*
** Return:      STUB_SUCCESS:   I am Disconnected Satellite Terminal
*               STUB_ILLEGAL:   I am not Satellite Terminal
*
** Description:  This function check "Am I satellite"
*===========================================================================
*/
SHORT   CstIamDisconnectedSatellite(VOID)
{
    if (CliNetConfig.fchSatStatus & CLI_SAT_DISCONNECTED) {
        return STUB_SUCCESS;
    }
	return STUB_ILLEGAL;
}


/*
*===========================================================================
** Synopsis:    SHORT    CstIamDisconnectedUnjoinedSatellite(VOID);
*
** Return:      STUB_SUCCESS:   I am Disconnected Satellite Terminal that is not
*                               joined to a cluster.
*               STUB_ILLEGAL:   I am not Satellite Terminal
*
** Description:  This function check "Am I satellite"
*===========================================================================
*/
SHORT   CstIamDisconnectedUnjoinedSatellite(VOID)
{
	// If we are a disconnected Satellite and we are not joined to a cluster
	// then indicate success.
    if ((CliNetConfig.fchSatStatus & (CLI_SAT_DISCONNECTED | CLI_SAT_JOINED)) == CLI_SAT_DISCONNECTED) {
        return STUB_SUCCESS;
    }
	return STUB_ILLEGAL;
}


/*
*===========================================================================
** Synopsis:    SHORT    CstIamSatellite(VOID);
*
** Return:      STUB_SUCCESS:   I am Satellite Terminal
*               STUB_ILLEGAL:   I am not Satellite Terminal
*
** Description:  This function check "Am I satellite"
*===========================================================================
*/
SHORT   CstIamSatellite(VOID)
{
#if defined(FORCE_SATELLITE_TERMINAL)
	return STUB_SUCCESS;
#endif
	// If we are not a Master Terminal nor Backup Master Terminal nor
	// Disconnected Satellite that is not joined to a cluster, then
	// we are a Satellite Terminal and are acting as such.
    if ((STUB_SUCCESS != CstIamMaster()) && 
        (STUB_SUCCESS != CstIamBMaster()) &&
		(STUB_SUCCESS != CstIamDisconnectedUnjoinedSatellite())) {
        return STUB_SUCCESS;
    }
    return STUB_ILLEGAL;
}

USHORT CstSetMasterBackupStatus (SHORT  sErrorM, SHORT  sErrorBM)
{
	USHORT  usOutOfDateIndic = 0;

	if (sErrorM != STUB_SUCCESS && sErrorBM == STUB_SUCCESS) {
		// if there was an error talking to the Master and success talking to
		// the Backup Master and we have a Backup Master provisioned then mark
		// the Master Terminal as Out Of Date.  CstSendBMaster() will return
		// STUB_DISCOVERY if there is not a Backup provisioned.
		if (0 != (CliNetConfig.fchStatus & CLI_NET_BACKUP)) {
			CstMTOutOfDate();
			usOutOfDateIndic |= 0x01;
		}
	}

	if (sErrorBM != STUB_SUCCESS && sErrorBM != STUB_DISCOVERY && sErrorM == STUB_SUCCESS) {
		CstBMOutOfDate();
		usOutOfDateIndic |= 0x02;
	}

	return usOutOfDateIndic;
}

// indicate if we are the Master Terminal by return STUB_SELF
// if you are using defined(FORCE_SATELLITE_TERMINAL) the problem you
// will run into is that in some cases you want this function to
// indicate Satellite and sometimes you want it to indicate Master.
SHORT CstCheckMasterRoleAssummed (VOID)
{
	USHORT    usCstComReadStatus;
	SHORT     sRetStatus = STUB_NOT_MASTER;

#if !defined(FORCE_SATELLITE_TERMINAL)
	if (STUB_SUCCESS == CstIamDisconnectedUnjoinedSatellite()) {
		// We do not care about the Master update status if we
		// are an unjoined or Disconnected Satellite.
       return STUB_SELF; // do not send to master, simply return the "I am Master" message
	}
#endif

#if defined(FORCE_SATELLITE_TERMINAL)
#pragma message("    **\z")
#pragma message("    ***************                 BREAKPOINT MAY BE NEEDED HERE FOR SATELLITE.")
#pragma message("    **")
// when using this in forced satellite terminal compile, you may need to set a break point
// so that you can manage when this function returns Master versus when it returns
// Satellite indicator.
#endif
	if (CLI_IAM_MASTER) {
		sRetStatus = STUB_SELF;
	}

    usCstComReadStatus = CstComReadStatus();    // read the Notice Board system status for CstSendMaster()
    if (0 == (usCstComReadStatus & CLI_STS_M_UPDATE)) { //if notice board says Master is NOT updating
		sRetStatus = STUB_M_DOWN;
    }

    if ((usCstComReadStatus & CLI_STS_BACKUP_FOUND) && (usCstComReadStatus & CLI_STS_BM_UPDATE)) {
		// if Terminal #1 (Master Terminal) indicates it is up to date then check if we are doing inquiry.
		// if it is not up to date then we will not bother.
		// Move check on inquiry state to after check if Master terminal or not
		// since this speeds up terminal startup when using Dynamic PLU buttons at
		// some locations such as Compass.
		if (sRetStatus == STUB_M_DOWN) {
			SHORT   sChekStatus;
			if (CLI_IAM_BMASTER) {
				sRetStatus = STUB_SELF;
			}
			sChekStatus = CstComChekInquiry(); //ensure no current inquiry is running
			if (sChekStatus != STUB_SUCCESS) {
				if (sChekStatus == STUB_FAITAL) {
					if (CLI_IAM_BMASTER)
						sRetStatus = STUB_M_DOWN;
					else if (CLI_IAM_MASTER)
						sRetStatus = STUB_SELF;
					else
						sRetStatus = STUB_DUR_INQUIRY;
				}
				// If we are in inquiry mode and we get this return status
				// then we are neither Master Terminal nor Backup Master Terminal.
				// Therefore we can't be sure who to talk with so lets just
				// return this status.
			}
		}
	}

	return sRetStatus;
}

/*
*===========================================================================
** Synopsis:    SHORT    CstSendMaster(VOID);
*
** Return:      STUB_SUCCESS:   communication success
*               STUB_SELF:      I am update Master terminal      
*               STUB_M_DOWN:    Master terminal is out of service
*
** Description: This function performs checks to determine if the master
*               terminal is up to date and whether the current message that
*               is contained in the global struct CliMsg should be sent to
*               the Master terminal.
*
*               If this terminal on which the application is running is the
*               Master terminal then we return a status indicating so and do
*               not send the message.
*===========================================================================
*/
#if defined(CstSendMaster)
SHORT   CstSendMaster_Special( VOID );
SHORT   CstSendMaster_Debug( char *aszFilePath, int nLineNo)
{
	extern XGRAM       CliSndBuf;          /* Send Buffer */    
	extern XGRAM       CliRcvBuf;          /* Receive Buffer */
	extern  CLICOMIF   CliMsg;
	char  xBuffer[256];


	sprintf (xBuffer, "CstSendMaster_Debug(): File %s  lineno = %d", RflPathChop(aszFilePath), nLineNo);
	NHPOS_ASSERT_TEXT(0, xBuffer);
	return CstSendMaster_Special();
}

SHORT   CstSendMaster_Special(VOID)
#else
SHORT   CstSendMaster(VOID)
#endif
{
	extern XGRAM       CliSndBuf;          /* Send Buffer */    
	extern XGRAM       CliRcvBuf;          /* Receive Buffer */
	extern  CLICOMIF   CliMsg;

	return CstSendMasterWithArgs (&CliMsg, &CliSndBuf, &CliRcvBuf);
}

SHORT   CstSendMasterWithArgs (CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf)
{
	USHORT    usCstComReadStatus;
	SHORT     sRetStatus = STUB_SUCCESS;

#if defined(FORCE_BACKUP_TERMINAL)
//		CstMTOutOfDate();
		pCliMsg->sError = STUB_M_DOWN;
		pCliMsg->sRetCode = (STUB_M_DOWN + STUB_RETCODE);
        return STUB_M_DOWN;					// return master status as down, do not send message
#endif

#if !defined(FORCE_SATELLITE_TERMINAL)
	if (STUB_SUCCESS == CstIamDisconnectedUnjoinedSatellite()) {
		// We do not care about the Master update status if we
		// are an unjoined or Disconnected Satellite.
       return STUB_SELF; // do not send to master, simply return the "I am Master" message
	}
#endif

    usCstComReadStatus = CstComReadStatus();    // read the Notice Board system status for CstSendMaster()
    if (usCstComReadStatus & CLI_STS_BACKUP_FOUND) {
		CLIRESCOM   *pResp = (CLIRESCOM *)pCliRcvBuf->auchData;

		if (0 == (usCstComReadStatus & CLI_STS_M_UPDATE)) {  //if notice board says Master is not up to date
			SHORT sErrorTemp = STUB_M_DOWN;

			pCliMsg->sError = sErrorTemp;
			pCliMsg->sRetCode = (sErrorTemp + STUB_RETCODE);
			pResp->sResCode = pCliMsg->sRetCode;
			pResp->sReturn = pCliMsg->sRetCode;
			if (sMasterErrorSave != sErrorTemp) {
				char xBuff[128];
				sprintf(xBuff, "Master Not UpToDate: (0 == (CstComReadStatus() & CLI_STS_M_UPDATE)) - 0x%x", usCstComReadStatus);
				NHPOS_ASSERT_TEXT((sMasterErrorSave == sErrorTemp), xBuff);
			}
			sMasterErrorSave = sErrorTemp;
			return sErrorTemp;					// return master status as down, do not send message
		}

		if (usCstComReadStatus & CLI_STS_BM_UPDATE) {
			// if Terminal #1 (Master Terminal) indicates it is up to date then check if we are doing inquiry.
			// if it is not up to date then we will not bother.
			// Move check on inquiry state to after check if Master terminal or not
			// since this speeds up terminal startup when using Dynamic PLU buttons at
			// some locations such as Compass.
			sRetStatus = CstComChekInquiry(); //ensure no current inquiry is running
			if (sRetStatus != STUB_SUCCESS) {
				if (sRetStatus == STUB_FAITAL) {
					if (CLI_IAM_BMASTER)
						sRetStatus = STUB_M_DOWN;
					else if (CLI_IAM_MASTER)
						sRetStatus = STUB_SELF;
					else
						sRetStatus = STUB_DUR_INQUIRY;
				}
				// If we are in inquiry mode and we get this return status
				// then we are neither Master Terminal nor Backup Master Terminal.
				// Therefore we can't be sure who to talk with so lets just
				// return this status.
				pCliMsg->sError = sRetStatus;
				pCliMsg->sRetCode = (sRetStatus + STUB_RETCODE);
				pResp->sResCode = pCliMsg->sRetCode;
				pResp->sReturn = pCliMsg->sRetCode;
				if (sMasterErrorSave != sRetStatus) {
					char xBuff[128];
					sprintf(xBuff, "Master inquiry: CstComChekInquiry() %d, usCstComReadStatus = 0x%x", sRetStatus, usCstComReadStatus);
					NHPOS_ASSERT_TEXT((sMasterErrorSave == sRetStatus), xBuff);
				}
				sMasterErrorSave = sRetStatus;
				return sRetStatus;		// return master status as possibly down, do not send message
			}
		}
	}

	if ((sMasterErrorSave != STUB_M_DOWN) && 0 != (usCstComReadStatus & CLI_STS_M_OFFLINE)) {  //if notice board says Master is offline and unavailable
		if ((usCstfsSystemInf & CLI_STS_M_REACHABLE) == 0) {
			CLIRESCOM   *pResp = (CLIRESCOM *)pCliRcvBuf->auchData;
			SHORT sErrorTemp = STUB_M_DOWN;

			pCliMsg->sError = sErrorTemp;
			pCliMsg->sRetCode = (sErrorTemp + STUB_RETCODE);
			pResp->sResCode = pCliMsg->sRetCode;
			pResp->sReturn = pCliMsg->sRetCode;

			if (sMasterErrorSave != sErrorTemp) {
				char xBuff[128];
				sprintf(xBuff, "Master Offline: (0 != (CstComReadStatus() & CLI_STS_M_OFFLINE)) - 0x%x", usCstComReadStatus);
				NHPOS_ASSERT_TEXT((sMasterErrorSave == sErrorTemp), xBuff);
			}
			sMasterErrorSave = sErrorTemp;
			return sErrorTemp;					// return master status as down, do not send message
		}
	}

	if (0 == (usCstComReadStatus & CLI_STS_M_OFFLINE)) {
		// if Master Terminal is not off line then do normal processing of clearing
		// any error indicators.  However if it is off line then we will want to save
		// that status and go ahead and attempt this communication anyway.
		sMasterErrorSave = 0;
		if (sMasterErrorSave != 0) {
			char xBuff[128];
			sprintf(xBuff, "Master cleared: usCstComReadStatus = 0x%x", usCstComReadStatus);
			NHPOS_ASSERT_TEXT((sMasterErrorSave == 0), xBuff);
		}
	}

	// This #if construct provides a way for a developer doing testing to
	// make a Master terminal perform actions similar to a Satellite terminal.
	// To enable the functionality define  FORCE_SATELLITE_TERMINAL which will then
	// enable the #if clause.
#if defined(FORCE_SATELLITE_TERMINAL)
	// This #if clause is executed if the developer wants to do testing of
	// Satellite terminal functionality with a single terminal.
	// This functionality should never be enabled for a Release build so it
	// has a check to trigger a compiler error if the compile is being done
	// in Release mode.
	{
#pragma message("  ****   ")
		UCHAR dummyvalue = CstComReadStatus ();
#pragma message("  ****   CLI_IAM_MASTER in CstSendMaster() stubbed out.  Master sending messages like a Satellite.   ******")
	}
#if !defined(_DEBUG)
	I_Will_Create_An_Error++;
#endif
#pragma message("  ****   ")
#else
	 //if operation is occurring on Master Terminal or disconnected Satellite Terminal
	if (CLI_IAM_MASTER) {
		// Since we are the Master Terminal set the reachable flag to be on.
		usCstfsSystemInf |= CLI_STS_M_REACHABLE;
		return STUB_SELF; // do not send to master, simply return the "I am Master" message
    }
#endif

    CstComMakeMessage(CLI_TGT_MASTER, CLI_PORT_STUB, pCliMsg, pCliSndBuf); //create\fill data structure for sending message to master
    CstComSendReceive(pCliMsg, pCliSndBuf, pCliRcvBuf); //sends request to terget server and receives response (loops until replied to)

	// We are implementing a new communications status indicator of CLI_STS_M_REACHABLE
	// in order to indicate that an attempt to communicate with the Master Terminal succeeded.
	// At VCS we are seeing where some Satellite Terminals are put on a different LAN sub-net than
	// the Master Terminal.  The result is that the Satellite Terminal on a different sub-net is
	// not seeing the Notice Board messages of the Master Terminal since a UDP broadcast is not
	// being propagated to out side of the Master Terminal's sub-net.
	// Using this new indicator allows the use of a Satellite Terminal that is not in the same sub-net.
	if (STUB_TIME_OUT == pCliMsg->sError) {
		usCstfsSystemInf &= ~CLI_STS_M_REACHABLE;
	} else {
		usCstfsSystemInf |= CLI_STS_M_REACHABLE;
	}
    return pCliMsg->sError;  //message successfully sent to master (failures return above)
}

/*
*===========================================================================
** Synopsis:    SHORT    CstSendBMaster(VOID);
*
** Return:      STUB_SUCCESS:   communication success
*               STUB_SELF:      I am update Back Up Master terminal      
*               STUB_BM_DOWN:   Back Up Master terminal is out of service
*
** Description:
*   This function supports to communicate with Back Up Master terminal.
*===========================================================================
*/
	// This #if construct provides a way for a developer doing testing to
	// make a Master terminal perform actions similar to a Backup terminal.
	// To enable the functionality define  FORCE_BACKUP_TERMINAL which will then
	// enable the #if clause.
#if defined(FORCE_BACKUP_TERMINAL)
SHORT   CstSendBMaster(VOID)
{
	extern XGRAM       CliSndBuf;          /* Send Buffer */    
	extern XGRAM       CliRcvBuf;          /* Receive Buffer */
	extern  CLICOMIF   CliMsg;
	CLICOMIF *pCliMsg = &CliMsg;

	// This #if clause is executed if the developer wants to do testing of
	// Satellite terminal functionality with a single terminal.
	// This functionality should never be enabled for a Release build so it
	// has a check to trigger a compiler error if the compile is being done
	// in Release mode.
	{
#pragma message("  ****   ")
		UCHAR dummyvalue = CstComReadStatus ();
#pragma message("  ****   CLI_IAM_BMASTER in CstSendBMaster() stubbed out.  Master sending messages like a Backup Master.   ******")
	}
#if !defined(_DEBUG)
	I_Will_Create_An_Error++;
#endif
#pragma message("  ****   ")
    CstComMakeMessage(CLI_TGT_MASTER, CLI_PORT_STUB, pCliMsg, &CliSndBuf);
    CstComSendReceive(pCliMsg, &CliSndBuf, &CliRcvBuf);

	if ((pCliMsg->sError == STUB_M_DOWN) || (CstComReadStatus() & CLI_STS_M_OFFLINE)) {
//		CstBMOutOfDate();
		pCliMsg->sError = STUB_BM_DOWN;
		pCliMsg->sRetCode = (STUB_BM_DOWN + STUB_RETCODE);
	}
    return pCliMsg->sError;
}
#else
#if defined(CstSendBMaster)
SHORT   CstSendBMaster_Special( VOID );
SHORT   CstSendBMaster_Debug( char *aszFilePath, int nLineNo)
{
	extern XGRAM       CliSndBuf;          /* Send Buffer */    
	extern XGRAM       CliRcvBuf;          /* Receive Buffer */
	extern  CLICOMIF   CliMsg;
	char  xBuffer[256];

	sprintf (xBuffer, "CstSendBMaster_Debug(): File %s  lineno = %d", RflPathChop(aszFilePath), nLineNo);
	NHPOS_ASSERT_TEXT(0, xBuffer);
	return CstSendMaster_Special();
}

SHORT   CstSendBMaster_Special( VOID )
#else
SHORT   CstSendBMaster(VOID)
#endif
{
	extern XGRAM       CliSndBuf;          /* Send Buffer */    
	extern XGRAM       CliRcvBuf;          /* Receive Buffer */
	extern  CLICOMIF   CliMsg;

	return CstSendBMasterWithArgs (&CliMsg, &CliSndBuf, &CliRcvBuf);
}

SHORT   CstSendBMasterWithArgs (CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf)
{
	USHORT usCstComReadStatus;
	SHORT  sRetStatus = STUB_SUCCESS;

	if (STUB_SUCCESS == CstIamDisconnectedUnjoinedSatellite()) {
		// We do not care about the Backup Master update status if we
		// are an unjoined or Disconnected Satellite.
       return STUB_DISCOVERY; // do not send to Backup Master, simply return the SUCCESS message
	}

	if (0 == (CliNetConfig.fchStatus & CLI_NET_BACKUP)) {
        return STUB_DISCOVERY;
    }

    usCstComReadStatus = CstComReadStatus();

	// check to see if the Backup Master Terminal option was
	// detected by function CstNetOpen().  If not, then just return.
    if (0 == (CliNetConfig.fchStatus & CLI_NET_BACKUP)) {
		if ((usCstComReadStatus & CLI_STS_BACKUP_FOUND) == 0) {
			return STUB_DISCOVERY;
		}
	}

    if (0 == (usCstComReadStatus & CLI_STS_BM_UPDATE)) {
		pCliMsg->sError = STUB_BM_DOWN;
		pCliMsg->sRetCode = (STUB_BM_DOWN + STUB_RETCODE);
		if (sBackMasterErrorSave != STUB_BM_DOWN) {
			char xBuff[128];
			sprintf(xBuff, "Backup Not Updating: (0 == (CstComReadStatus() & CLI_STS_BM_UPDATE)) - 0x%x", usCstComReadStatus);
			NHPOS_ASSERT_TEXT((sBackMasterErrorSave == STUB_BM_DOWN), xBuff);
		}
		sBackMasterErrorSave = STUB_BM_DOWN;
        return STUB_BM_DOWN;
    }

    if (0 != (usCstComReadStatus & CLI_STS_M_UPDATE)) {
		// if Terminal #1 (Master Terminal) indicates it is up to date then check if we are doing inquiry.
		// if it is not up to date then we will not bother.
		sRetStatus = CstComChekInquiry();
		if (sRetStatus != STUB_SUCCESS) {
			if (sRetStatus == STUB_FAITAL) {
				if (CLI_IAM_MASTER)
					sRetStatus = STUB_BM_DOWN;
				else if (CLI_IAM_BMASTER)
					sRetStatus = STUB_SELF;
				else
					sRetStatus = STUB_DUR_INQUIRY;
			}
			// If we are in inquiry mode and we get this return status
			// then we are neither Master Terminal nor Backup Master Terminal.
			// Therefore we can't be sure who to talk with so lets just
			// return this status.
			pCliMsg->sError = sRetStatus;
			pCliMsg->sRetCode = (sRetStatus + STUB_RETCODE);
			if (sBackMasterErrorSave != sRetStatus) {
				char xBuff[128];
				sprintf(xBuff, "Backup inquiry: CstComChekInquiry() %d, usCstComReadStatus = 0x%x", sRetStatus, usCstComReadStatus);
				NHPOS_ASSERT_TEXT((sBackMasterErrorSave == sRetStatus), xBuff);
			}
			sBackMasterErrorSave = sRetStatus;
			return sRetStatus;		// return Backup Master status as possibly down, do not send message
		}
	}

	if (0 != (usCstComReadStatus & CLI_STS_BM_OFFLINE)) {  //if notice board says Backup Master is offline and unavailable
		SHORT sErrorTemp = STUB_BM_DOWN;

		pCliMsg->sError = sErrorTemp;
		pCliMsg->sRetCode = (sErrorTemp + STUB_RETCODE);
		if (sBackMasterErrorSave != sErrorTemp) {
			char xBuff[128];
			sprintf(xBuff, "Backup Master Offline: (0 != (CstComReadStatus() & CLI_STS_BM_OFFLINE)) - 0x%x", usCstComReadStatus);
			NHPOS_ASSERT_TEXT((sBackMasterErrorSave == sErrorTemp), xBuff);
		}
		sBackMasterErrorSave = sErrorTemp;
		return sErrorTemp;					// return backup master status as down, do not send message
	}

	if (sBackMasterErrorSave != 0) {
		char xBuff[128];
		sprintf(xBuff, "Backup cleared: usCstComReadStatus = 0x%x", usCstComReadStatus);
		NHPOS_ASSERT_TEXT((sBackMasterErrorSave == 0), xBuff);
	}
	sBackMasterErrorSave = 0;

	if (CLI_IAM_BMASTER) {
        return STUB_SELF;
    }

    CstComMakeMessage(CLI_TGT_BMASTER, CLI_PORT_STUB, pCliMsg, pCliSndBuf);
    CstComSendReceive(pCliMsg, pCliSndBuf, pCliRcvBuf);

    return pCliMsg->sError;
}
#endif


/*
*===========================================================================
** Synopsis:    SHORT    CstSendTerminal(VOID);
*
** Return:      STUB_SUCCESS:   communication success
*
** Description: 
*===========================================================================
*/
SHORT   CstSendTerminal(USHORT usTerminalPosition)
{
	extern XGRAM       CliSndBuf;          /* Send Buffer */    
	extern XGRAM       CliRcvBuf;          /* Receive Buffer */
	extern  CLICOMIF   CliMsg;
	CLICOMIF *pCliMsg = &CliMsg;
	SHORT  sRetStatus = STUB_SUCCESS;

    CstComMakeMessage(usTerminalPosition, CLI_PORT_STUB, pCliMsg, &CliSndBuf); //create\fill data structure for sending message to terminal
    CstComSendReceive(pCliMsg, &CliSndBuf, &CliRcvBuf); //sends request to terget server and receives response (loops until replied to)

    return CliMsg.sError;  //message successfully sent to master (failures return above)
}

/*
*===========================================================================
** Synopsis:    SHORT    CstSendBroadcast(CLICOMIF);
*
** Return:      STUB_SUCCESS:   communication success
*               STUB_SELF:      I am update Back Up Master terminal      
*               STUB_BM_DOWN:   Back Up Master terminal is out of service
*
** Description:
*   This function will do a broadcast of a message to all terminals on
*   the LAN..
*===========================================================================
*/
SHORT   CstSendBroadcast(CLICOMIF *pCliMsg)
{
	extern XGRAM       CliSndBuf;          /* Send Buffer */    
	extern XGRAM       CliRcvBuf;          /* Receive Buffer */
	SHORT  sRetStatus = STUB_SUCCESS;

    CstComMakeMessage(CLI_TGT_BROADCAST, CLI_PORT_STUB, pCliMsg, &CliSndBuf);
    CstComSendReceive(pCliMsg, &CliSndBuf, &CliRcvBuf);

    return STUB_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    SHORT    SstUpdateAsMaster(VOID);
*
** Return:      STUB_SELF:   I am As Master
*               STUB_NOT_MASTER:    I am not as master
*
** Description: This function checks whether I am master or not.
*===========================================================================
*/
SHORT   SstUpdateAsMaster(VOID)
{
    USHORT  fsComStatus;
    SHORT   sError;

    sError = STUB_NOT_MASTER;
    fsComStatus = CstComReadStatus();

    if ((CLI_IAM_MASTER) && ((fsComStatus & CLI_STS_M_UPDATE) || (fsComStatus & CLI_STS_BACKUP_FOUND) == 0)) {
        sError = STUB_SELF;
    }

    if ((CLI_IAM_BMASTER) && (fsComStatus & CLI_STS_BM_UPDATE)) {
        if (0 == (fsComStatus & CLI_STS_M_UPDATE)) {
            sError = STUB_SELF;
        }
    }

    if (STUB_SELF == sError) {
        CstComChekInquiry();
    }
    return sError;
}

/*
*===========================================================================
** Synopsis:    SHORT   SstUpdateBackUp(VOID);
*
** Return:      STUB_NOT_MASTER  - terminal is not acting as Master Terminal
*               STUB_SELF        - if terminal is actually Backup Master
*               STUB_SUCCESS     - if request succeeded
*               other            - other STUB_ error codes if error
*
** Description: This function supports to send request to back up master.
*===========================================================================
*/
SHORT  SstUpdateBackUp(VOID)
{
    SHORT   sError = STUB_NOT_MASTER;

    if (CLI_IAM_MASTER) {                           /* I am Master ? */
        sError = CstSendBMaster();                  /* Send to B Master */
		NHPOS_ASSERT (STUB_SUCCESS == sError || STUB_SELF == sError || STUB_DISCOVERY == sError);
    }

	return sError;
}

/*
*===========================================================================
** Synopsis:    SHORT    SstReadAsMaster(VOID);
*
** Return:      STUB_SELF:  Report enable to read on this terminal
*               STUB_NOT_MASTER:  Report disable to read  
*
** Description: 
*  This function checks whether the report is enable to read or not.
*===========================================================================
*/
SHORT   SstReadAsMaster(VOID)
{
    USHORT  fsComStatus = CstComReadStatus();
    SHORT   sError = STUB_NOT_MASTER;

    if ((CLI_IAM_MASTER) && ((fsComStatus & CLI_STS_M_UPDATE) || (fsComStatus & CLI_STS_BACKUP_FOUND) == 0)) {
        sError = STUB_SELF;
    }

    if (CLI_IAM_BMASTER && (fsComStatus & CLI_STS_BM_UPDATE)) {
        if (0 == (fsComStatus & CLI_STS_M_UPDATE)) {
			sError = STUB_SELF;
        }
    }
    return sError;
}

/*
*===========================================================================
** Synopsis:    SHORT    SstReadFAsMaster(VOID);
*
** Return:      STUB_SELF:  I am Master
*               STUB_SUCCESS:   read OK from master  
*
** Description: This function tries to read data from master.
*===========================================================================
*/
SHORT   SstReadFAsMaster(VOID)
{
    SHORT   sError, sErrorM;

    sError = sErrorM = CstSendMaster();                         /* Send to Master */
    if (STUB_M_DOWN == sError || STUB_TIME_OUT == sError) {   /* master down */
		if (0 != (CliNetConfig.fchStatus & CLI_NET_BACKUP)) {
			sError = CstSendBMaster();                        /* Send to B Master */
			if (STUB_BM_DOWN == sError || STUB_TIME_OUT == sError) {                     /* Backup Master down */
				sError = sErrorM;
			}
		}
    }
    return sError;                            /* Comm. Success */
}

/*
*===========================================================================
** Synopsis:    VOID    CstComMakeMessage(UCHAR uchServer, USHORT usPort, CLICOMIF *pCliMsg, XGRAM *pCliSndBuf);
*
*     Input:    uchServer   - uniuqe address of target server
*               usPort      - port on target host such as CLI_PORT_STUB
*               pCliMsg     - pointer to CliMsg struct for this message
*               pCliSndBuf  - pointer to the CliSndBuf struct containing the data to send
*
** Return:      none.
*
** Description: This function makes send message.
*===========================================================================
*/
static VOID    CstComMakeMessage(USHORT usServer, USHORT usPort, CLICOMIF *pCliMsg, XGRAM *pCliSndBuf)
{
    memcpy(pCliSndBuf->auchFaddr, CliNetConfig.auchFaddr, PIF_LEN_IP);
    pCliSndBuf->auchFaddr[CLI_POS_UA] = (UCHAR)usServer;
	if (usServer == CLI_TGT_BROADCAST) {
		// Set the send address to be a broadcast on the LAN
		// if the target server is broadcast.
		pCliSndBuf->auchFaddr[0] = 192;
		pCliSndBuf->auchFaddr[1] = 0;
		pCliSndBuf->auchFaddr[2] = 0;
		pCliSndBuf->auchFaddr[3] = 0;
	}
    pCliSndBuf->usFport = CLI_PORT_SERVER;
    pCliSndBuf->usLport = usPort;             // typically CLI_PORT_STUB

    pCliMsg->pReqMsgH->usFunCode = (pCliMsg->usFunCode & CLI_RSTBACKUPFG);
    pCliMsg->pReqMsgH->usSeqNo = 0;
    memcpy(pCliSndBuf->auchData, pCliMsg->pReqMsgH, pCliMsg->usReqMsgHLen);

    pCliMsg->usAuxLen = 0;
    pCliMsg->usRetryCo = 0;
}
    
/*
*===========================================================================
** Synopsis:    VOID    CstComSendReceive(CLICOMIF *pCliMsg);
*
** Return:      none.
*
** Description:
*   This function sends request to terget server and receives response.
*===========================================================================
*/
static VOID    CstComSendReceive(CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf)
{
	USHORT   usRetryCount;
    USHORT  usSendLen;
	SHORT   sError = PIF_OK;

    CstNetClear();                              /* clear receive buffer */ 

    pCliMsg->usRetryCo = 0;
    usRetryCount = CstReadMDCPara(CLI_MDC_RETRY);

    do {
		usSendLen = CstComMakeMultiData(pCliMsg, pCliSndBuf);      /* make request data */
		sError = CstNetSend(usSendLen, pCliSndBuf);         /*=== SEND MESSAGE ===*/
		// If this is a broadcast type of message then
		// we will ignore any errors from the send as the
		// send expects an acknowledgement and there won't
		// necessarily be one with a broadcast.
		if (pCliSndBuf->auchFaddr[CLI_POS_UA] == 255 || pCliSndBuf->auchFaddr[CLI_POS_UA] == 0 || 0 <= sError) {
			CstNetReceive(CLI_MDC_CLIENT, pCliMsg, pCliSndBuf, pCliRcvBuf);          /*=== RECEIVE RESPONSE ===*/
			CstComCheckError(pCliMsg, pCliRcvBuf);   /* error check */
			CstComRespHandle(pCliMsg, pCliSndBuf, pCliRcvBuf);   /* response handling */
			CstComErrHandle(pCliMsg, pCliSndBuf, pCliRcvBuf);   /* error handling */
			CstComSendConfirm(pCliMsg, pCliSndBuf);   /* send confirm, if need */
//			break;
		}
		else {
			PifLog(MODULE_STB_LOG, LOG_ERROR_STB_NETWORK_ERROR);
			PifLog(MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)abs(sError));
			pCliMsg->sError = STUB_ILLEGAL;
			switch (sError) {
//			case PIF_ERROR_NET_WRITE:
//				break;
			case PIF_ERROR_NET_OFFLINE:
				pCliMsg->sError = STUB_NET_NOT_OPENED;
				break;

			case PIF_ERROR_NET_ERRORS:
				// Fall thru and treat PIF_ERROR_NET_ERRORS like PIF_ERROR_NET_TIMEOUT
			case PIF_ERROR_NET_UNREACHABLE:
				// Fall thru and treat PIF_ERROR_NET_UNREACHABLE like PIF_ERROR_NET_TIMEOUT
			case PIF_ERROR_NET_TIMEOUT:
				pCliMsg->sError = STUB_TIME_OUT;
				break;

			case PIF_ERROR_NET_CLEAR:
				pCliMsg->sError = STUB_MSG_CLEARED;
				break;

			case PIF_ERROR_NET_BUSY:
				pCliMsg->sError = STUB_BUSY;
				break;

			case PIF_ERROR_NET_POWER_FAILURE:
				pCliMsg->sError = STUB_POWER_DOWN;
				break;

			default:
				{
					CLIREQCOM    *pSend = (CLIREQCOM *)pCliSndBuf->auchData;
					char xBuff[256];
					sprintf(xBuff, "CstComSendReceive: Unknown CstNetSend() error %d, usFunCode 0x%x, usRetryCo = %d, usSeqNo = 0x%x", sError, pCliMsg->usFunCode, pCliMsg->usRetryCo, pSend->usSeqNo);
					NHPOS_ASSERT_TEXT(0, xBuff);
				}
				break;
			}
//			CstComErrHandle(pCliMsg, pCliSndBuf, pCliRcvBuf);   /* error handling */
			pCliMsg->usRetryCo++;
			PifSleep(50);
		}

		if (pCliMsg->usRetryCo >= usRetryCount) {
			PifLog (MODULE_STB_LOG, LOG_EVENT_STB_CNT_DOWN_SND_RCV);
			PifLog(MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)(abs(sError)));
			PifLog(MODULE_DATA_VALUE(MODULE_STB_LOG), (USHORT)(abs(pCliMsg->sError)));
			PifLog(MODULE_LINE_NO(MODULE_STB_LOG), (USHORT)__LINE__);
		}

		if (STUB_BM_DOWN == pCliMsg->sError) {
			break;
		} else if (STUB_M_DOWN == pCliMsg->sError) {
			break;
		}

    } while (STUB_SUCCESS != pCliMsg->sError && pCliMsg->usRetryCo < usRetryCount);

	if (pCliMsg->usRetryCo >= usRetryCount || (STUB_BM_DOWN == pCliMsg->sError) || (STUB_M_DOWN == pCliMsg->sError)) {
		CLIREQCOM   *pSend = (CLIREQCOM *)pCliSndBuf->auchData;
		CLIRESCOM   *pResp = (CLIRESCOM *)pCliRcvBuf->auchData;
		char         xBuff[256];

		sprintf (xBuff, "##CstComSendReceive(): usUA %d, sError = %d, usFunCode 0x%x, usRetryCo = %d, usSeqNo = 0x%x", pCliSndBuf->auchFaddr[CLI_POS_UA], pCliMsg->sError, pCliMsg->usFunCode, pCliMsg->usRetryCo, pSend->usSeqNo);
		NHPOS_ASSERT_TEXT((STUB_SUCCESS == pCliMsg->sError), xBuff);

		pResp->sResCode = (pCliMsg->sError + STUB_RETCODE);
		pResp->sReturn = (pCliMsg->sError + STUB_RETCODE);

		if (pCliMsg->usRetryCo >= usRetryCount)
			pCliMsg->sRetCode = STUB_RETCODE_TIME_OUT_ERROR;
		else 
			pCliMsg->sRetCode = (pCliMsg->sError + STUB_RETCODE);
	}
}

/*
*===========================================================================
** Synopsis:    VOID    CstComChekInquiry(VOID);
*
** Return:      STUB_SUCCESS     - no inquiry in process between Master and Backup
*               STUB_FAITAL      - inquiry timed out and no response from other terminal
*               STUB_DUR_INQUIRY - inquiry in process and no time out yet
*               STUB_STOP_UPDATE - stop function probably due to broadcast happening
*
** Description:  This function displays error message when this terminal
*                or target server is inquiry state.  It then waits for
*                the inquiry state to clear and returns 
*===========================================================================
*/
static SHORT    CstComChekInquiry(VOID)
{
    SHORT   sError = STUB_SUCCESS;

    if (CstComReadStatus() & (CLI_STS_STOP | CLI_STS_INQUIRY))
	{
		SHORT   sRetryCount = 5;
		USHORT  fsComStatus;
		USHORT  usPrevious;

		/* allow power switch at error display */
		usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
		do {
			// Give up our time slice and wait for Notice Board change.
			// Notice Board kicks off about once a second but the Server
			// thread may update Notice Board sooner.  This does let other
			// lower priority threads run while we wait.
 			PifSleep (250);
			fsComStatus = CstComReadStatus();
			sError = STUB_SUCCESS;
			if (fsComStatus & CLI_STS_STOP)
				sError = STUB_STOP_UPDATE;

			if (fsComStatus & CLI_STS_INQUIRY)
				sError = STUB_DUR_INQUIRY;

			if (fsComStatus & CLI_STS_INQTIMEOUT)
				sError = STUB_FAITAL;

			sRetryCount--;
			if (1 > sRetryCount || sError == STUB_FAITAL)
			{
				break;
			}
		} while (fsComStatus & (CLI_STS_STOP | CLI_STS_INQUIRY));

		PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
		if (1 > sRetryCount || sError != STUB_SUCCESS)
		{
			PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
			PifLog (MODULE_STB_LOG, LOG_EVENT_STB_CNT_DOWN_CHK_INQ);
			PifLog (MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT) abs(sError));
		}
    }
	return sError;
}

/*
*===========================================================================
** Synopsis:    VOID    CstComMakeMultiData(VOID);
*
** Return:      data length to be sent.
*
** Description: This function makes request data into send buffer.
*               Any changes to this function may need to be synchronized
*               with changes to function SerRMHCheckData() in file
*               servrmh.c which contains a check on the length of the
*               data received.
*===========================================================================
*/
static USHORT  CstComMakeMultiData(CLICOMIF *pCliMsg, XGRAM *pCliSndBuf)
{
    CLIREQCOM   *pSend = (CLIREQCOM *)pCliSndBuf->auchData;
    USHORT      usDataSiz = 0, usDataMax;

    if ((0 != pCliMsg->usReqLen) && ((0 == pSend->usSeqNo) || (pCliMsg->sError == STUB_MULTI_RECV))) {
        pSend->usSeqNo ++;
        pSend->usSeqNo |= CLI_SEQ_SENDDATA;
        if (pCliMsg->usAuxLen > pCliMsg->usReqLen) {
            pCliMsg->usAuxLen = pCliMsg->usReqLen;
        }
        usDataSiz = pCliMsg->usReqLen - pCliMsg->usAuxLen;
        usDataMax = PIF_LEN_UDATA - (pCliMsg->usReqMsgHLen + PIF_LEN_IP + sizeof(XGHEADER) + 10);
        if (usDataSiz > usDataMax) {
            usDataSiz = usDataMax;
        } else {
            pSend->usSeqNo |= CLI_SEQ_SENDEND;
        }
		// cast address to UCHAR and figure offset for data based on size of request header in number of bytes.
		// add into message the size of data (USHORT) and the data itself after the data size.
        memcpy((UCHAR *)pSend + pCliMsg->usReqMsgHLen, &usDataSiz, 2);
        memcpy((UCHAR *)pSend + pCliMsg->usReqMsgHLen + 2, pCliMsg->pauchReqData + pCliMsg->usAuxLen, usDataSiz);
        pCliMsg->usAuxLen += usDataSiz;
        usDataSiz += 2;
    } else if ((0 != pCliMsg->usResLen) && (pCliMsg->usResLen != sizeof(CLIRESETK))) {
		//SR 140 cwunn 7/9/2003 BK Etk allow CliMsg Response Length to be the size of a CLIRESETK, for CliETKIndJobRead()
		pSend->usSeqNo &= CLI_SEQ_CONT;
		++ pSend->usSeqNo;
	}

    if (CLI_TGT_MASTER == pCliSndBuf->auchFaddr[CLI_POS_UA]) {
        if (0 == (CstComReadStatus() & CLI_STS_BM_UPDATE)) {
            pSend->usFunCode |= CLI_SETBMDOWN;
        } else {
            pSend->usFunCode &= ~CLI_SETBMDOWN;
        }
    } else if (CLI_TGT_BMASTER == pCliSndBuf->auchFaddr[CLI_POS_UA]) {
        if (0 == (CstComReadStatus() & CLI_STS_M_UPDATE)) {
            pSend->usFunCode &= ~CLI_FCWBACKUP;
        }
    }
    pCliMsg->sError = STUB_SUCCESS;
    return (CLI_MAX_IPDATA + pCliMsg->usReqMsgHLen + usDataSiz);
}
        
/*
*===========================================================================
** Synopsis:    VOID    CstComCheckError(VOID);
*
** Return:      none.
*
** Description:  This function checks server error by processing the server
*                response if the server did respond (CliMsg.sError == STUB_SUCCESS
*                indicating success or CliMsg.sError == STUB_BUSY if server is busy).
*                If the server did not respond due to some kind of network error
*                then CstNetReceive () will set the value of CliMsg.sError with
*                the code returned by PifNetReceive ().  This value will be
*                something like STUB_TIME_OUT or STUB_BUSY or something similar
*                if there is a network layer error.
*
*                If the network layer reports successful transport of a message,
*                then we will report to other modules the response code provided
*                by the sender of this message.  This response code is actually
*                a code from the network layer on the server side indicating whether
*                the requested message was actually processed or not.
*===========================================================================
*/
VOID    CstComCheckError(CLICOMIF *pCliMsg, XGRAM *pCliRcvBuf)
{
    if (STUB_SUCCESS == pCliMsg->sError) {
		CLIRESCOM   *pResp = (CLIRESCOM *)pCliRcvBuf->auchData;

        pCliMsg->sError = pResp->sResCode;
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
*  Comments added 12.10.2002 by cwunn for GSU SR 8
*===========================================================================
*/
VOID    CstComRespHandle(CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf)
{
    CLIREQCOM   *pSend = (CLIREQCOM *)pCliSndBuf->auchData;
    CLIRESCOM   *pResp = (CLIRESCOM *)pCliRcvBuf->auchData;
    USHORT      usDataSiz;

    if ((STUB_SUCCESS != pCliMsg->sError) && (STUB_MULTI_SEND != pCliMsg->sError)) {
        return;
    }

	if (pCliMsg->pResMsgH)
	{
		memcpy(pCliMsg->pResMsgH, pResp, pCliMsg->usResMsgHLen);
	}

    pCliMsg->sRetCode = pResp->sReturn;
    if ((CLI_TGT_BMASTER == pCliSndBuf->auchFaddr[CLI_POS_UA]) && (pSend->usFunCode & CLI_FCWBACKUP))
	{
		// Check to see if we still have connectivity to the Backup Master
		// terminal.  We will check a particular message which is used fairly
		// continously, the CLI_FCEJREAD message.
		// If the return code is negative, this may be an error or it may not.
		// Some return codes such as CAS_PERFORM or EJ_NOTHING_DATA have negative
		// values but these are not errors.
        if (0 > pResp->sReturn)
		{
            if ((pSend->usFunCode & CLI_RSTCONTCODE) == CLI_FCEJREAD)
			{
                if (pResp->sReturn == EJ_NOTHING_DATA) {
					pCliMsg->sError = STUB_SUCCESS;
				} else {
					pCliMsg->sError = STUB_BM_DOWN;
				}
            }
            else if ((pSend->usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLUPDATE)
			{
                if (pResp->sReturn == TTL_SUCCESS) {
					pCliMsg->sError = STUB_SUCCESS;
				} else {
					pCliMsg->sError = STUB_BM_DOWN;
				}
			}
        }
        return;
    }
    if (pResp->usSeqNo & CLI_SEQ_RECVDATA) {	
        if (pSend->usSeqNo & CLI_SEQ_SENDEND) {
            pCliMsg->usAuxLen = 0;
        }
		// get the number of bytes in the data by looking at the USHORT after the request header.
		// we have to take into consideration the size of a USHORT containing the number of bytes
		// of data in the message in our offset calculations below.
        memcpy(&usDataSiz, (UCHAR *)pResp + pCliMsg->usResMsgHLen, 2);
		//if returned data + previously returned data has smaller size than the response data length
		//then copy the data to CliMsg local storage area
        if ((usDataSiz + pCliMsg->usAuxLen) <= pCliMsg->usResLen) {
            memcpy(pCliMsg->pauchResData + pCliMsg->usAuxLen, (UCHAR *)pResp + pCliMsg->usResMsgHLen + 2, usDataSiz);
            pCliMsg->usAuxLen += usDataSiz;
        } else {
            if (pCliMsg->usAuxLen < pCliMsg->usResLen) {
                usDataSiz = pCliMsg->usResLen - pCliMsg->usAuxLen;
                memcpy(pCliMsg->pauchResData + pCliMsg->usAuxLen, (UCHAR *)pResp + pCliMsg->usResMsgHLen + 2, usDataSiz);
                pCliMsg->usAuxLen += usDataSiz;
            }
        }            
    }
	//SR 140 cwunn BK Etk code added 6/30/03
	//If the Response packet does not indicate recieved data, it may still contain
	//   data for use by CliETKIndJobRead().  Check if Response packet's sequence #
	//   is 0 and check that the CliMsg packet has a valid Response Data pointer.
	//   If so, copy the data, which represents the Employee Name Mnemonic for the
	//   employee who just performed an Etk action.
	//Note: This action contains 3 safeguards against causing unintended behavior:
	//    1) Most functions that use returned data will indicate so in the sequence #
	//		using a non-zero status code.
	//	  2) No known functions use a valid Response Data pointer when not expecting
	//		returned data
	//	  3) The CliMsg packet's Response Data buffer should not be accessed unless
	//		the requesting function indicates that data should be returned.
	else { //pResp->usSeqNo does not indicate recieve data
		if(pResp->usSeqNo == 0){
			if(pCliMsg->pauchResData){
				memcpy(pCliMsg->pauchResData, (UCHAR *)pResp + pCliMsg->usResMsgHLen, ETK_MAX_NAME_SIZE * sizeof(TCHAR));
			}
		}
	}
	//end new code 6/30/03 Burger King ETK Issue
}

/*
*===========================================================================
** Synopsis:    VOID    CstComErrHandle(VOID);
*
** Return:      none.
*
** Description:  This function executes error handling.
*                There are several cases in which the value of the network
*                layer status, CliMsg.sError, is changed to STUB_SUCCESS.
*                This is because the network send/receive loop in which a
*                message is sent and then the response received has a condition
*                to continue retrying until CliMsg.sError == STUB_SUCCESS.
*===========================================================================
*/
static VOID    CstComErrHandle(CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf)
{
	SYSCONFIG CONST  *SysCon = PifSysConfig();
    CLIREQCOM   *pSend = (CLIREQCOM *)pCliSndBuf->auchData;
    CLIRESCOM   *pResp = (CLIRESCOM *)pCliRcvBuf->auchData;
    USHORT      usRetry;
    USHORT      usPrevious;

    if ((STUB_SUCCESS == pCliMsg->sError) || (STUB_MULTI_SEND == pCliMsg->sError) || (STUB_MULTI_RECV == pCliMsg->sError)) {
		// if it is not an error then just return without doing anything.
		// we need to handle single and multiple sends such as sending a guest check file.
		pCliMsg->usRetryCo = 0;
        return;
    }

	{
		char                xBuff[256];
		DATE_TIME           dtCurrentDateTime;
		BOOL                bPrintAssert = 0;
		static DATE_TIME    dtSaveDateTime = {0};
		static USHORT       usSaveSeqNo = 0;
		static USHORT       usSaveRetryCo = 0;
		static USHORT       usSaveFunCode = 0;
		static SHORT        sSaveError = 0;

		PifGetDateTime(&dtCurrentDateTime);
		bPrintAssert = ((usSaveRetryCo != pCliMsg->usRetryCo) || (usSaveSeqNo != pSend->usSeqNo) || (usSaveFunCode != pCliMsg->usFunCode) || (sSaveError != pCliMsg->sError) || (memcmp (&dtSaveDateTime, &dtCurrentDateTime, sizeof(DATE_TIME)) != 0));
		dtSaveDateTime = dtCurrentDateTime;
		usSaveSeqNo = pSend->usSeqNo;
		usSaveFunCode = pCliMsg->usFunCode;
		sSaveError = pCliMsg->sError;
		usSaveRetryCo = pCliMsg->usRetryCo;

		if (bPrintAssert) {
			sprintf (xBuff, "**CstComErrHandle(): usUA %d, sError = %d, usFunCode 0x%x, usRetryCo = %d, usSeqNo = 0x%x", pCliSndBuf->auchFaddr[CLI_POS_UA], pCliMsg->sError, pCliMsg->usFunCode, pCliMsg->usRetryCo, pSend->usSeqNo);
			NHPOS_ASSERT_TEXT((STUB_SUCCESS == pCliMsg->sError), xBuff);
		}
	}
    pSend->usSeqNo = 0;
    pCliMsg->usAuxLen = 0;

	// Increment the retry counter, pCliMsg->usRetryCo, and get the system
	// defined retry count to compare it against to see if we are done with retries.
    ++ pCliMsg->usRetryCo;
    usRetry = CstReadMDCPara(CLI_MDC_RETRY);

    switch(pCliMsg->sError) {

    case    STUB_POWER_DOWN:
        CstNetClose();
        CstNetOpen();
        break; 

	// STUB_BUSY is normally a response from the Server user thread indicating that when it
	// received the message from the Client user thread, it was in the middle of some other
	// kind of processing.  Lets do a quick sleep and then retry the send/receive cycle.
    case    STUB_BUSY:
    case    (STUB_BUSY + STUB_RETCODE):
        pResp->sResCode = STUB_BUSY;               // same as CliMsg.sError
		pResp->sReturn = STUB_RETCODE_BUSY_ERROR;  // same as CliMsg.sRetCode
		pResp->usFunCode = pSend->usFunCode;
		pResp->usSeqNo = pSend->usSeqNo;

		PifSleep (150);   // received STUB_BUSY so wait a bit and then retry.
		break;

    case    STUB_DUR_INQUIRY:
        pResp->sResCode = STUB_DUR_INQUIRY;               // same as CliMsg.sError
		pResp->sReturn = STUB_RETCODE_DUR_INQUIRY;        // same as CliMsg.sRetCode
		pResp->usFunCode = pSend->usFunCode;
		pResp->usSeqNo = pSend->usSeqNo;

		PifSleep (150);   // received STUB_DUR_INQUIRY so wait a bit and then retry.
		break;

		// STUB_MSG_CLEARED is a response from the target of a send that it has discarded
		// the message by clearing its queue of received messages.
	case STUB_MSG_CLEARED:
        pResp->sResCode = STUB_MSG_CLEARED;           // same as CliMsg.sError
		pResp->sReturn = STUB_RETCODE_MSG_CLEARED;    // same as CliMsg.sRetCode
		pResp->usFunCode = pSend->usFunCode;
		pResp->usSeqNo = pSend->usSeqNo;
		break;

	// STUB_BUSY_MULTI is a response from the Server user thread indicating that
	// functions SerSendNextFrame() and SerRecvNextFrame(0 has determined that a multiple
	// packet send or receive is happening and that this packet is not from the same
	// terminal doing the multiple packet send.
	case STUB_BUSY_MULTI:                          // SerSendNextFrame() on Server side indicates during multi receive
		pCliMsg->sError = STUB_BUSY;               // force CliMsg.sError to be same as STUB_BUSY to allow higher level routines to process
        pResp->sResCode = STUB_BUSY;               // same as CliMsg.sError
		pResp->sReturn = STUB_RETCODE_BUSY_ERROR;  // same as CliMsg.sRetCode
		pResp->usFunCode = pSend->usFunCode;
		pResp->usSeqNo = pSend->usSeqNo;
		-- pCliMsg->usRetryCo;                     // back out the retry count if STUB_BUSY_MULTI

		PifSleep (150);   // received STUB_BUSY_MULTI so wait a bit more and then retry.
		break;

    case    STUB_DISCOVERY:
    case    (STUB_DISCOVERY + STUB_RETCODE):
		// fall through and treat same as STUB_TIME_OUT.  See also PIF_ERROR_NET_DISCOVERY
    case    STUB_TIME_OUT:
    case    (STUB_TIME_OUT + STUB_RETCODE):
        pCliMsg->sError = STUB_TIME_OUT;  // force error to time out in case its a STUB_DISCOVERY

        pResp->sResCode = STUB_TIME_OUT;               // same as CliMsg.sError
		pResp->sReturn = STUB_RETCODE_TIME_OUT_ERROR;  // same as CliMsg.sRetCode
		pResp->usFunCode = pSend->usFunCode;
		pResp->usSeqNo = pSend->usSeqNo;

        if ( (CLI_FCSPSPRINTKP == pCliMsg->usFunCode) ||   /* Add R3.0 */
             (CLI_FCSPSPOLLING == pCliMsg->usFunCode) ||
             (CLI_FCEJCLEAR    == ( CLI_RSTCONTCODE & pCliMsg->usFunCode)) ||  /* Add R3.1 */
             (CLI_FCEJREAD     == ( CLI_RSTCONTCODE & pCliMsg->usFunCode)))
		{
            pCliMsg->sError = STUB_SUCCESS;
            pCliMsg->sRetCode = STUB_RETCODE_TIME_OUT_ERROR;
            return;
        }  

		PifSleep (50);   // received STUB_TIME_OUT so wait a bit more and then retry.
        /* allow power switch at error display */
        usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */

        if (pCliMsg->usRetryCo > usRetry) {
            /*----  Skip if Shared or CPM function --------*/
			//We check to see if we are getting a time out error for kitchen printing
			//with sending the information to a different terminal.  If this error 
			//condition is true, we issue a time out, but still let it succedd
			//so that we dont get into an infinate loop. JHHJ
            if ( (pCliSndBuf->auchFaddr[CLI_POS_UA] != SysCon->auchLaddr[CLI_POS_UA]) &&
				 ((CLI_FCKPSPRINT    == pCliMsg->usFunCode)   || 
				  (CLI_FCSPSPRINT    == pCliMsg->usFunCode)   || 
                  (CLI_FCSPSTERMLOCK == pCliMsg->usFunCode)   || 
                  (CLI_FCSPSTERMUNLOCK == pCliMsg->usFunCode) ||
                  (CLI_FCCPMSENDMESS == pCliMsg->usFunCode) )  )
			{

                pCliMsg->sError = STUB_SUCCESS;
                pCliMsg->sRetCode = STUB_RETCODE_TIME_OUT_ERROR;
                PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
                return;
            }  

#if 0
            /*----  Skip if Shared function --------*/
            if (CLI_TGT_BMASTER == pCliSndBuf->auchFaddr[CLI_POS_UA]) {
                pCliMsg->sError = STUB_BM_DOWN;
            }
            if (CLI_TGT_MASTER == pCliSndBuf->auchFaddr[CLI_POS_UA]) {
                pCliMsg->sError = STUB_M_DOWN;
            }
#endif

			pCliMsg->sRetCode = (pCliMsg->sError + STUB_RETCODE);
        }
        
        PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
        break;

	case    STUB_UNMATCH_TRNO:
	case    (STUB_UNMATCH_TRNO + STUB_RETCODE):
#if 0
        if ( pCliMsg->usRetryCo > usRetry) {  /* Check retry counter */
            pCliMsg->sError = STUB_SUCCESS;
            pCliMsg->sRetCode = STUB_RETCODE_TIME_OUT_ERROR;
        }
#else
        pCliMsg->sError = STUB_UNMATCH_TRNO;
        if (CLI_TGT_BMASTER == pCliSndBuf->auchFaddr[CLI_POS_UA]) {
            pCliMsg->sError = STUB_BM_DOWN;
        }
        if (CLI_TGT_MASTER == pCliSndBuf->auchFaddr[CLI_POS_UA]) {
            pCliMsg->sError = STUB_M_DOWN;
        }

		pCliMsg->sRetCode = (pCliMsg->sError + STUB_RETCODE);
#endif
		break;

	case    STUB_NOT_MASTER:
        pCliMsg->sError = STUB_SUCCESS;
        pCliMsg->sRetCode = STUB_RETCODE_TIME_OUT_ERROR;
		break;

	/* avoid BM busy at AC42, 10/23/01 */
	case STUB_FRAME_SEQERR:
	case (STUB_FRAME_SEQERR + STUB_RETCODE):
		if (CLI_FCBAK == (pCliMsg->usFunCode & CLI_FCBAK)) {
	        if (pCliMsg->usRetryCo > usRetry) {
	            pCliMsg->usRetryCo = 0;
            } else {
				PifSleep (50);   // received STUB_FRAME_SEQERR so wait a bit and then retry.
			}
			break;
		}
		/* don't break, just fall through to default case */
    default:
        /*----  Skip if Shared function --------*/
        if ( (CLI_FCSPSPRINTKP == pCliMsg->usFunCode) ||   /* Add R3.0 */
             (CLI_FCSPSPOLLING == pCliMsg->usFunCode) ||
             (CLI_FCEJCLEAR    == ( CLI_RSTCONTCODE & pCliMsg->usFunCode)) ||  /* Add R3.1 */
             (CLI_FCEJREAD     == ( CLI_RSTCONTCODE & pCliMsg->usFunCode)))
		{
            pCliMsg->sError = STUB_SUCCESS;
            pCliMsg->sRetCode = STUB_RETCODE_TIME_OUT_ERROR;
            return;
        }  
        if ( (CLI_FCSPSPRINT    == pCliMsg->usFunCode) || 
             (CLI_FCSPSTERMLOCK == pCliMsg->usFunCode) || 
             (CLI_FCSPSTERMUNLOCK == pCliMsg->usFunCode) ||
             (CLI_FCCPMSENDMESS == pCliMsg->usFunCode)      )
		{
            if ( pCliMsg->usRetryCo > usRetry) {  /* Check retry counter */
                pCliMsg->sError = STUB_SUCCESS;
                pCliMsg->sRetCode = STUB_RETCODE_TIME_OUT_ERROR;
            }
            return;
        } 
 
        /*----  Skip if Shared function --------*/
        if (CLI_IAM_MASTER && CLI_TGT_BMASTER == pCliSndBuf->auchFaddr[CLI_POS_UA]) {
            pCliMsg->sError = STUB_BM_DOWN;
            pCliMsg->sRetCode = STUB_RETCODE_BM_DOWN_ERROR;
        }
        break;
    } 

    /*----  Skip if Shared or CPM function --------*/
    if ( (CLI_FCSPSPRINT    == pCliMsg->usFunCode) || 
         (CLI_FCSPSTERMLOCK == pCliMsg->usFunCode) || 
         (CLI_FCSPSTERMUNLOCK == pCliMsg->usFunCode) ||
         (CLI_FCSPSPRINTKP == pCliMsg->usFunCode) ||  /* Add R3.0 */
         (CLI_FCSPSPOLLING == pCliMsg->usFunCode) ||  /* Add R3.0 */
         (CLI_FCEJCLEAR    == ( CLI_RSTCONTCODE & pCliMsg->usFunCode)) ||  /* Add R3.1 */
         (CLI_FCEJREAD     == ( CLI_RSTCONTCODE & pCliMsg->usFunCode)) ||  /* Add R3.1 */
         (CLI_FCCPMSENDMESS == pCliMsg->usFunCode)      ) {
        return;       
    }  

    /*----  Skip if Shared function --------*/
    if (CLI_TGT_MASTER == pCliSndBuf->auchFaddr[CLI_POS_UA]) {
        if (0 == (CstComReadStatus() & CLI_STS_M_UPDATE)) {
            pCliMsg->sError = STUB_SUCCESS;
        }
    } else {
        if (0 == (CstComReadStatus() & CLI_STS_BM_UPDATE)) {
            pCliMsg->sError = STUB_SUCCESS;
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID    CstComSendConfirm(VOID);
*
** Return:      none.
*
** Description:  This function sends confirmation message if it need.
*===========================================================================
*/
static VOID    CstComSendConfirm(CLICOMIF *pCliMsg, XGRAM *pCliSndBuf)
{
    CLIREQCOM   *pSend = (CLIREQCOM *)pCliSndBuf->auchData;

    if ((STUB_SUCCESS == pCliMsg->sError) && (pSend->usFunCode & CLI_FCWCONFIRM)) {
        pSend->usFunCode |= CLI_SETCONFIRM;
        ++ pSend->usSeqNo;
        CstNetSend((USHORT)(CLI_MAX_IPDATA + pCliMsg->usReqMsgHLen), pCliSndBuf);
    }
}

/*
*===========================================================================
** Synopsis:    USHORT  CstComReadStatus(VOID);
*
** Return:      current communication status
*
** Description:  This function decodes communication status based on
				Notice Board status.  We obtain the current Notice Board
				status and then set the Client communications status based
				on the Notice Board status.
*===========================================================================
*/
USHORT  CstComReadStatus(VOID)
{
    USHORT      fsComStatus = 0;
    NBMESSAGE   NbInf;

	// Check the current Notice Board status bits to determine
	// if the Master terminal and the Backup Master terminal status
	// is up to date.  We also check to see if the Master and Backup
	// Master are off-line or not.
    NbReadAllMessage(&NbInf);
    if (NbInf.fsSystemInf & NB_MTUPTODATE) {
        fsComStatus |= CLI_STS_M_UPDATE;
    }
    if (NbInf.fsSystemInf & NB_BMUPTODATE) {
        fsComStatus |= CLI_STS_BM_UPDATE;
    }
    if (0 == (NbInf.fsSystemInf & NB_SETMTONLINE)) {
        fsComStatus |= CLI_STS_M_OFFLINE;
    }
    if (0 == (NbInf.fsSystemInf & NB_SETBMONLINE)) {
        fsComStatus |= CLI_STS_BM_OFFLINE;
    }
    if (0 == (NbInf.fsSystemInf & NB_WITHOUTBM)) {
        fsComStatus |= CLI_STS_BACKUP_FOUND;
    }
    if (0 != (NbInf.fsSystemInf & NB_INQUIRY)) {
        fsComStatus |= CLI_STS_INQUIRY;
    }
    if (CLI_IAM_SATELLITE) {
        if (fsComStatus & CLI_STS_M_UPDATE) {
            if (NbInf.fsMBackupInf & NB_STOPALLFUN) { 
                fsComStatus |= CLI_STS_STOP;
            }
        } else if (fsComStatus & CLI_STS_BM_UPDATE) {
            if (NbInf.fsBMBackupInf & NB_STOPALLFUN) {
                fsComStatus |= CLI_STS_STOP;
            }
        }
    } else if (CLI_IAM_DISCONSAT) {
	} else {
		USHORT   fsServerFlag = 0;

        SstReadFlag(&fsServerFlag);
        if (fsServerFlag & SER_SPESTS_INQUIRY) {
            fsComStatus |= CLI_STS_INQUIRY;
        }
        if (fsServerFlag & SER_SPESTS_INQTIMEOUT) {
            fsComStatus |= CLI_STS_INQTIMEOUT;
        }
    }
	// If neither the Master Terminal and the Backup Master Terminal
	// is up to date then we will assume the Master Terminal is the
	// one that is up to date.
    if (0 == (fsComStatus & (CLI_STS_M_UPDATE | CLI_STS_BM_UPDATE))) {
        fsComStatus |= CLI_STS_M_UPDATE;
    }

	fsComStatus |= usCstfsSystemInf;    // add in our special Client side status information
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
** Description:  This function supports to display error code.
*===========================================================================
*/
SHORT   CstDisplayError(SHORT sError, USHORT usTarget)
{
    USHORT  usErrCode, usAbort=0;
    UCHAR   uchFSC;
    USHORT  fsComStatus = CstComReadStatus();
    USHORT  usPrevious;

	if (sError != STUB_SUCCESS) {
		PifLog (MODULE_STB_LOG, LOG_EVENT_STB_CSTDISPLAYERROR);
		PifLog (MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)abs(sError));
	}
	else {
		return 0;
	}

    switch(sError) {
	case    STUB_RETCODE_DUR_INQUIRY:
    case    STUB_DUR_INQUIRY:
        usErrCode = LDT_LNKDWN_ADR;
        break;

    case    STUB_INQ_M_DOWN:
        usErrCode = LDT_LNKDWN_ADR;
        if ((CLI_IAM_BMASTER) && (fsComStatus & CLI_STS_BM_UPDATE)) {
            usAbort = 1;
        }
        break;

    case    STUB_RETCODE_M_DOWN_ERROR:
    case    STUB_M_DOWN:
        usErrCode = LDT_LNKDWN_ADR;
        if ((CLI_IAM_BMASTER) && (fsComStatus & CLI_STS_BM_UPDATE)) {
            usAbort = 1;
			PifLog(MODULE_STB_LOG, LOG_ERROR_STB_COM_MA_DWN_02);
//            CstMTOutOfDate();
        }
        break;

    case    STUB_INQ_BM_DOWN:
        usErrCode = LDT_SYSBSY_ADR;
        if ((CLI_IAM_MASTER) && (fsComStatus & CLI_STS_M_UPDATE)) {
            usAbort = 1;
        }
        break;

    case    STUB_RETCODE_BM_DOWN_ERROR:
    case    STUB_BM_DOWN:
        usErrCode = LDT_SYSBSY_ADR;
        if ((CLI_IAM_MASTER) && (fsComStatus & CLI_STS_M_UPDATE)) {
			PifLog(MODULE_STB_LOG, LOG_ERROR_STB_COM_BM_DWN_01);
//            CstBMOutOfDate();
            return 0;
        }
        break;

    case    STUB_IAM_BUSY:
        usErrCode = LDT_LNKDWN_ADR;
        if (CLI_IAM_BMASTER) {
            usErrCode = LDT_SYSBSY_ADR;
        }
        break;

	case STUB_KPS_BUSY:
		usErrCode = LDT_KPS_BUSY;
		break;

		//the following two codes are for delay balance issues. JHHJ
	case STUB_DB_FULL:		/*-23 the delay balance total file is full.*/
		usErrCode = LDT_FLFUL_ADR; //use this until database change
		break;

	case STUB_DB_NEARFULL:
		usErrCode = LDT_NEARFULL_ADR;
		break;

	case STUB_RETCODE_UNMATCH_TRNO:
	case STUB_UNMATCH_TRNO:
		usErrCode = LDT_GCFSUNMT_ADR;
		break;

    default:
        if (CLI_TGT_MASTER == usTarget) {
            usErrCode = LDT_LNKDWN_ADR;
        } else {
            usErrCode = LDT_SYSBSY_ADR;
        }
        break;
    }

	{
		char  xBuff[128];
		sprintf (xBuff, "==NOTE: CstDisplayError() sError %d, usErrCode %d, fsComStatus 0x%x", sError, usErrCode, fsComStatus);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

    /* allow power switch at error display */
    usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */

    uchFSC = UieErrorMsg(usErrCode, (USHORT)((usAbort) ? UIE_ABORT : UIE_WITHOUT));
    
    PifControlPowerSwitch(usPrevious);  /* V1.0.04 */

    if (FSC_CLEAR == uchFSC) {              /* CLEAR key depressed ? */
        return 1;
    }
    if (CLI_IAM_MASTER && (1 == usBMOption) && CLI_TGT_BMASTER == usTarget) {
		PifLog(MODULE_STB_LOG, LOG_ERROR_STB_COM_BM_DWN_02);
//        CstBMOutOfDate();
    }
    if (CLI_IAM_BMASTER && CLI_TGT_MASTER == usTarget) {
		PifLog(MODULE_STB_LOG, LOG_ERROR_STB_COM_MA_DWN_01);
//        CstMTOutOfDate();
    }

	return 0;
}

SHORT   CstDisplayErrorSemRelReq (SHORT  hsSemaphore, SHORT sError, USHORT usTarget)
{
	SHORT  sRetStatus;

	PifReleaseSem (hsSemaphore);
	sRetStatus = CstDisplayError (sError, usTarget);
	PifRequestSem (hsSemaphore);

	return sRetStatus;
}

/*
*===========================================================================
** Synopsis:    SHORT    CstBackUpError(VOID);
*
** Return:      none.
*
** Description:  This function supports to display Back Up error code.
*===========================================================================
*/
SHORT   CstBackUpError(VOID)
{
	extern XGRAM       CliSndBuf;          /* Send Buffer */    
	extern XGRAM       CliRcvBuf;          /* Receive Buffer */
	extern  CLICOMIF   CliMsg;

	SHORT       sRetStatus = STUB_SUCCESS;
	SHORT       sPifCountDown = PIF_LOG_COUNT_DOWN_START;
    USHORT      usPrevious;

    /* allow power switch at error display */
    usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */

    for (;;) {
        CstDisplayError(STUB_BM_DOWN, CliSndBuf.auchFaddr[CLI_POS_UA]);
        if (0 == (CstComReadStatus() & CLI_STS_BM_UPDATE)) {
            sRetStatus =  STUB_SUCCESS;
			break;
        }

        if (CLI_FCGCFLOCK == (CliMsg.usFunCode & CLI_RSTCONTCODE)) {   /* Add R3.0 */
            sRetStatus =  STUB_BUSY;
			break;
        }

		if (--sPifCountDown <= 0) {
			PifLog (MODULE_STB_LOG, LOG_EVENT_STB_CNT_DOWN_BCK_ERR);
			sPifCountDown = PIF_LOG_COUNT_DOWN_RESET;
		}
    }
    PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
    return sRetStatus;
}

/*
*===========================================================================
** Synopsis:    VOID    CstBMOutOfDate(VOID);
*
** Return:      none.
*
** Description:  This function makes Back Up Master to Out Of Date.
*===========================================================================
*/
#if defined CstBMOutOfDate
SHORT  CstBMOutOfDate_Debug(char *aszFilePath, int iLineNo)
{
	USHORT usCstComReadStatus = CstComReadStatus();

    if (0 != (usCstComReadStatus & CLI_STS_BM_UPDATE)) { //if notice board says master is up to date
		char   xBuffer[256];
		SHORT  sSstSetFlag = SstSetFlagWaitDone(SER_SPESTS_BMOD, husCliWaitDone);

		sprintf(xBuffer, "CstBMOutOfDate(): usCstComReadStatus = 0x%x  sSstSetFlag %d  File %s  lineno = %d", usCstComReadStatus, sSstSetFlag, RflPathChop(aszFilePath), iLineNo);
		NHPOS_ASSERT_TEXT(!CLI_STS_BM_UPDATE, xBuffer);
		return sSstSetFlag;
	}
	return 0;
}
#else
SHORT   CstBMOutOfDate(VOID)
{
	USHORT usCstComReadStatus = CstComReadStatus();

	if (0 != (usCstComReadStatus & CLI_STS_BACKUP_FOUND)) {
		if (0 != (usCstComReadStatus & CLI_STS_BM_UPDATE)) { //if notice board says master is up to date
			char  xBuffer[128];
			SHORT  sSstSetFlag = SstSetFlagWaitDone(SER_SPESTS_BMOD, husCliWaitDone);

			sprintf(xBuffer, "CstBMOutOfDate(): usCstComReadStatus = 0x%x, sSstSetFlag %d", usCstComReadStatus, sSstSetFlag);
			NHPOS_ASSERT_TEXT(!CLI_STS_BM_UPDATE, xBuffer);
			return sSstSetFlag;
		}
	}
	return 0;
}
#endif

/*
*===========================================================================
** Synopsis:    VOID    CstBMOffLine(VOID);
*
** Return:      none.
*
** Description:  This function makes Back Up Master to Off Line.
*===========================================================================
*/
#if defined CstBMOffLine
SHORT  CstBMOffLine_Debug(char *aszFilePath, int iLineNo)
{
	USHORT usCstComReadStatus = CstComReadStatus();

	if (CLI_IAM_SATELLITE) {
		if (0 != (usCstComReadStatus & CLI_STS_BM_OFFLINE)) {
			NbResetBMOnline();
		}
	}
	else {
		if (0 != (usCstComReadStatus & CLI_STS_BM_OFFLINE)) {
			char   xBuffer[128];
			SHORT  sSstSetFlag = SstSetFlagWaitDone(SER_SPESTS_INQUIRY, husCliWaitDone);

			sprintf(xBuffer, "CstBMOffLine(): usCstComReadStatus = 0x%x  sSstSetFlag %d  File %s  lineno = %d", usCstComReadStatus, sSstSetFlag, RflPathChop(aszFilePath), iLineNo);
			NHPOS_ASSERT_TEXT(!CLI_STS_BM_OFFLINE, xBuffer);
			return sSstSetFlag;
		}
	}
	return 0;
}
#else
SHORT   CstBMOffLine(VOID)
{
	USHORT usCstComReadStatus = CstComReadStatus();

	if (CLI_IAM_SATELLITE) {
		if (0 != (usCstComReadStatus & CLI_STS_BM_OFFLINE)) {
			NbResetBMOnline();
		}
	}
	else {
		if (0 != (usCstComReadStatus & CLI_STS_BM_OFFLINE)) {
			char  xBuffer[128];
			SHORT  sSstSetFlag = SstSetFlagWaitDone(SER_SPESTS_INQUIRY, husCliWaitDone);

			sprintf(xBuffer, "CstBMOffLine(): usCstComReadStatus = 0x%x, sSstSetFlag %d", usCstComReadStatus, sSstSetFlag);
			NHPOS_ASSERT_TEXT(!CLI_STS_BM_OFFLINE, xBuffer);
			return sSstSetFlag;
		}
	}
	return 0;
}
#endif


/*
*===========================================================================
** Synopsis:    VOID    CstMTOutOfDate(VOID);
*
** Return:      none.
*
** Description:  This function makes Master to Out Of Date.
*===========================================================================
*/
#if defined CstMTOutOfDate
SHORT  CstMTOutOfDate_Debug(char *aszFilePath, int iLineNo)
{
	USHORT usCstComReadStatus = CstComReadStatus();

    if (0 != (usCstComReadStatus & CLI_STS_M_UPDATE)) { //if notice board says master is up to date
		char   xBuffer[256];
		SHORT  sSstSetFlag = SstSetFlagWaitDone(SER_SPESTS_MTOD, husCliWaitDone);

		sprintf(xBuffer, "CstMTOutOfDate(): usCstComReadStatus = 0x%x  sSstSetFlag %d  File %s  lineno = %d", usCstComReadStatus, sSstSetFlag, RflPathChop(aszFilePath), iLineNo);
		NHPOS_ASSERT_TEXT(!CLI_STS_M_UPDATE, xBuffer);
		return sSstSetFlag;
	}
	return 0;
}
#else
SHORT   CstMTOutOfDate(VOID)
{
	USHORT usCstComReadStatus = CstComReadStatus();

    if (0 != (usCstComReadStatus & CLI_STS_M_UPDATE)) { //if notice board says master is up to date
		char  xBuffer[256];
		SHORT  sSstSetFlag = SstSetFlagWaitDone(SER_SPESTS_MTOD, husCliWaitDone);

		sprintf(xBuffer, "CstMTOutOfDate(): usCstComReadStatus = 0x%x, sSstSetFlag %d", usCstComReadStatus, sSstSetFlag);
		NHPOS_ASSERT_TEXT(!CLI_STS_M_UPDATE, xBuffer);
		return sSstSetFlag;
	}
	return 0;
}
#endif

/*
*===========================================================================
** Synopsis:    VOID    CstBMOffLine(VOID);
*
** Return:      none.
*
** Description:  This function makes Back Up Master to Off Line.
*===========================================================================
*/
#if defined CstMTOffLine
SHORT  CstMTOffLine_Debug(char *aszFilePath, int iLineNo)
{
	USHORT usCstComReadStatus = CstComReadStatus();

	if (CLI_IAM_SATELLITE) {
		if (0 != (usCstComReadStatus & CLI_STS_M_OFFLINE)) { //if notice board says master is up to date
			NbResetMTOnline();
		}
	}
	else {
		if (0 != (usCstComReadStatus & CLI_STS_M_OFFLINE)) { //if notice board says master is up to date
			char   xBuffer[256];
			SHORT  sSstSetFlag = SstSetFlagWaitDone(SER_SPESTS_INQUIRY, husCliWaitDone);

			sprintf(xBuffer, "CstMTOffLine(): usCstComReadStatus = 0x%x  sSstSetFlag %d  File %s  lineno = %d", usCstComReadStatus, sSstSetFlag, RflPathChop(aszFilePath), iLineNo);
			NHPOS_ASSERT_TEXT(!CLI_STS_M_UPDATE, xBuffer);
			return sSstSetFlag;
		}
	}
	return 0;
}
#else
SHORT   CstMTOffLine(VOID)
{
	USHORT usCstComReadStatus = CstComReadStatus();

	if (CLI_IAM_SATELLITE) {
		if (0 != (usCstComReadStatus & CLI_STS_M_OFFLINE)) { //if notice board says master is up to date
			NbResetMTOnline();
		}
	}
	else {
		if (0 != (usCstComReadStatus & CLI_STS_M_OFFLINE)) { //if notice board says master is up to date
			char  xBuffer[256];
			SHORT  sSstSetFlag = SstSetFlagWaitDone(SER_SPESTS_INQUIRY, husCliWaitDone);

			sprintf(xBuffer, "CstMTOffLine(): usCstComReadStatus = 0x%x, sSstSetFlag %d", usCstComReadStatus, sSstSetFlag);
			NHPOS_ASSERT_TEXT(!CLI_STS_M_UPDATE, xBuffer);
			return sSstSetFlag;
		}
	}
	return 0;
}
#endif

/*
*===========================================================================
** Synopsis:    USHORT    CstReadMDCPara(UCHAR address);
*
*     Input:    address - MDC address
*
** Return:      MDC Value.
*
** Description:  This function reads MDC Parameter.
*===========================================================================
*/
USHORT   CstReadMDCPara(UCHAR address)
{
    USHORT  usValue = CLI_DEFMDC_WAIT;

    switch(address) {
    case    CLI_MDC_CLIENT:         /* response timer */
        usValue = CliParaMDCCheck(MDC_CLI3_ADR, 0xff);
        if (0 == usValue) {
            usValue = CLI_DEFMDC_CLIENT;
        }
        if (60 < usValue) {
            usValue = 60;
        }
        break;

    case    CLI_MDC_RETRY:          /* retry counter */
#if 0
        usValue = (USHORT)CliParaMDCCheck(MDC_CLI5_ADR, 0xff);
        usValue %= 16;
        if (0 == usValue) {
            usValue = CLI_DEFMDC_RETRY;
        }
#else
        usValue = CLI_DEFMDC_RETRY;
#endif
        break;
    
    case    CLI_MDC_WAIT:           /* wait timer */
        usValue = CliParaMDCCheck(MDC_CLI5_ADR, 0xff);
        usValue /= 16;
        if (0 == usValue) {
            usValue = CLI_DEFMDC_WAIT;
        }
        break;

    case    CLI_MDC_CPMWAIT:        /* cpm wait timer */
        usValue = CliParaMDCCheck(MDC_CPM_WAIT_TIME, 0xff);
        usValue /= 16;
        if (0 == usValue) {
            usValue = CLI_DEFMDC_CPMWAIT;
        }
        break;

    default:
		PifLog(MODULE_STB_LOG, LOG_ERROR_CLIENT_WRNG_ADR);
		PifLog(MODULE_DATA_VALUE(MODULE_STB_LOG), address);
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
** Description:  This function sleeps.
*===========================================================================
*/
VOID    CstSleep(VOID)
{
#if 1
	PifSleep (250);   // CstSleep() function change to reduce sleep time during broadcast
#else
    LONG    ulWait;
    USHORT  usMsec;

    ulWait = CstReadMDCPara(CLI_MDC_WAIT) * 1000L;

    while(0L < ulWait) {
        usMsec = (50000L < ulWait) ? (USHORT)50000 : (USHORT)ulWait;
        PifSleep(usMsec);
        ulWait -= usMsec;
    }
#endif
}

/*
*===========================================================================
** Synopsis:    VOID    CstNetOpen(VOID);
*
** Return:      none.
*
** Description:  This function opens NET for stub.
*===========================================================================
*/
VOID    CstNetOpen(VOID)
{
    SYSCONFIG CONST *SysCon = PifSysConfig();
    XGHEADER        IPdata = {0};
    SHORT           sHandle;

    CliNetConfig.fchStatus = 0;
    CliNetConfig.fchSatStatus = 0;

	if (SysCon->usTerminalPosition & PIF_CLUSTER_DISCONNECTED_SAT)
		CliNetConfig.fchSatStatus |= CLI_SAT_DISCONNECTED;

	if (SysCon->usTerminalPosition & PIF_CLUSTER_JOINED_SAT)
		CliNetConfig.fchSatStatus |= CLI_SAT_JOINED;

    memcpy (CliNetConfig.auchFaddr, SysCon->auchLaddr, PIF_LEN_IP);

    if (((0 == SysCon->auchLaddr[0]) &&
         (0 == SysCon->auchLaddr[1]) &&
         (0 == SysCon->auchLaddr[2])) ||    /* comm. board not provide */
        (0 == SysCon->auchLaddr[3])) {      /* U.A is 0 */
        CliNetConfig.auchFaddr[0] = 0;        
        CliNetConfig.auchFaddr[1] = 0;        
        CliNetConfig.auchFaddr[2] = 0;        
        CliNetConfig.auchFaddr[3] = 1;      /* stand alone system */
		NHPOS_ASSERT_TEXT(0, "**WARNING: CstNetOpen() communications board not provided.");
        return;
    }

    if (CLI_ALLTRANSNO < SysCon->auchLaddr[3]) {
//        PifAbort(MODULE_STB_ABORT, FAULT_BAD_ENVIRONMENT);
    }    
    if ( 1 == usBMOption ) {    /* saratoga - indicate that we are a Backup System */
        CliNetConfig.fchStatus |= CLI_NET_BACKUP;
    }
    if (DISP_2X20 == SysCon->uchOperType) {
        CliNetConfig.fchStatus |= CLI_NET_DISP20;
    }

    IPdata.usLport = CLI_PORT_STUB;
    IPdata.usFport = ANYPORT;
    sHandle = PifNetOpen(&IPdata);

    if (0 <= sHandle) {
		char  xBuff[128];
        CliNetConfig.usHandle = (USHORT)sHandle;
        CliNetConfig.fchStatus |= CLI_NET_OPEN;
        PifNetControl(CliNetConfig.usHandle, PIF_NET_SET_MODE, PIF_NET_NMODE | PIF_NET_TMODE);
		sprintf (xBuff, "==NOTE: CstNetOpen() PifNetOpen fchStatus 0x%x", CliNetConfig.fchStatus);
		NHPOS_NONASSERT_TEXT(xBuff);
    } else {
		char  xBuff[128];
		sprintf (xBuff, "**ERROR: CstNetOpen() PifNetOpen failed error %d", sHandle);
		NHPOS_ASSERT_TEXT((0 <= sHandle), xBuff);

        PifLog(MODULE_STB_ABORT, FAULT_BAD_ENVIRONMENT);
        PifLog(MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sHandle));
        PifAbort(MODULE_STB_ABORT, FAULT_BAD_ENVIRONMENT);
    }
}

/*
*===========================================================================
** Synopsis:    VOID    CstNetClose(VOID);
*
** Return:      none.
*
** Description:  This function closes NET for stub.
*===========================================================================
*/
VOID    CstNetClose(VOID)
{
    PifNetClose(CliNetConfig.usHandle);
    CliNetConfig.fchStatus &= ~CLI_NET_OPEN;
}

/*
*===========================================================================
** Synopsis:    VOID    CstNetClear(VOID);
*
** Return:      none.
*
** Description:  This function clears NET for stub.
*===========================================================================
*/
VOID    CstNetClear(VOID)
{
    if (CliNetConfig.fchStatus & CLI_NET_OPEN) {
        PifNetControl(CliNetConfig.usHandle, PIF_NET_CLEAR);
    }
}

/*
*===========================================================================
** Synopsis:    VOID   CstNetSend(USHORT usSize);
*
*     InPut:    usSize  - number of bytes to be sent
*
** Return:      none.
*
** Description:  This function send the Data to specified server.
*===========================================================================
*/
SHORT    CstNetSend(USHORT usSize, XGRAM *pCliSndBuf)
{
    SHORT   sError = PIF_ERROR_NET_ACCESS_DENIED;
	SHORT   sRetry=3;

    if (CliNetConfig.fchStatus & CLI_NET_OPEN) {

        CliNetConfig.fchStatus |= CLI_NET_SEND;

		// Set a timeout for the send so that if the server is
		// busy, we will get notified immediately.  Look at
		// SerRecvNextFrame () in servrmh.c in the Server subsystem
		// for an example of Server responding with STUB_BUSY.
		if (pCliSndBuf->auchFaddr[CLI_POS_UA] != 255 && pCliSndBuf->auchFaddr[CLI_POS_UA] != 0)
			PifNetControl(CliNetConfig.usHandle, PIF_NET_SET_STIME, 3000);

        do {                                    /* send message */
            if (PIF_ERROR_NET_TIMEOUT == sError || PIF_ERROR_NET_BUSY == sError) {
				if (pCliSndBuf->auchFaddr[CLI_POS_UA] != 255 && pCliSndBuf->auchFaddr[CLI_POS_UA] != 0) {
					PifNetControl(CliNetConfig.usHandle, PIF_NET_SET_STIME, 3500);
				}
            }

            sError = PifNetSend(CliNetConfig.usHandle, pCliSndBuf, usSize);
			if (0 > sError) {
				PifSleep(50);
			}

			sRetry--;
        } while (((PIF_ERROR_NET_WRITE == sError) && (sRetry > 0)) || 
			((PIF_ERROR_NET_OFFLINE == sError) && (sRetry > 0)) ||
			((PIF_ERROR_NET_TIMEOUT == sError) && (sRetry > 0)) ||
			((PIF_ERROR_NET_BUSY == sError) && (sRetry > 0)) );
        
        CliNetConfig.fchStatus &= ~CLI_NET_SEND;
    }

	return sError;
}

/*
*===========================================================================
** Synopsis:    SHORT  CstNetReceive(USHORT usMDC);
*
*     InPut:    usMDC  - Waite Timer 
** Return:      none.
*
** Description:  This function receive the data from server.
*===========================================================================
*/
SHORT  CstNetReceive(USHORT usMDC, CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf)
{
	SHORT       sPifCountDown = PIF_LOG_COUNT_DOWN_START;
    CLIREQCOM   *pSend = (CLIREQCOM *)pCliSndBuf->auchData;
    CLIRESCOM   *pResp = (CLIRESCOM *)pCliRcvBuf->auchData;
    SHORT       sError;
    USHORT      usTimer, usPrevErrorCo;

    if (0 == (CliNetConfig.fchStatus & CLI_NET_OPEN)) {
        pCliMsg->sError = STUB_TIME_OUT;
        pCliMsg->sRetCode = STUB_RETCODE_TIME_OUT_ERROR;
        return STUB_TIME_OUT;
    }

    if ( CLI_MDC_CLIENT == usMDC ) {      /* Not CPM Function */
        usTimer = CstReadMDCPara(CLI_MDC_CLIENT) * 1000;
    } else {
                                          /* CPM function only */
        usTimer = (CLI_DEFMDC_MULTI + 5) * 1000;  /* 35 x MDC seconds */
    }

    if ( (pSend->usFunCode & CLI_RSTCONTCODE ) == CLI_FCSPSPOLLING ) { /* add R3.0 */
        usTimer = 3000;                     /* 2 seconds */
    }
    if ( (pSend->usFunCode & CLI_FCMSG_TYPE ) == CLI_FCGCF ) { /* add R3.0 */
#ifdef old_stuff_deleted
		// Improve wait time on an error condition such as Master terminal
		// or Backup Master terminal goes off-line.
        if (usTimer < 12000) {
            usTimer = 12000;                     /* 12 seconds */
        }
#else
        if (usTimer < 9000) {
            usTimer = 9000;                     /* 6 seconds */
        }
#endif
    }

    /* avoid time out, at PLU DB reset, 06/27/01 */
    /* correct masking digit for BM copy busy, 10/24/01 */
    if ( (pSend->usFunCode & CLI_RSTCONTCODE ) == CLI_FCTTLRESETTL ) { /* add R3.0 */
        usTimer = 60000;                     /* 60 seconds */
    }
    if ( (pSend->usFunCode & CLI_RSTCONTCODE ) == CLI_FCTTLPLUOPTIMIZE ) { /* add R3.0 */
        usTimer = 60000;                     /* 60 seconds */
    }

    if (usTimer < 3500) {
        usTimer = 3500;                     /* minimum time out of 3.5 seconds */
    }

    PifNetControl(CliNetConfig.usHandle, PIF_NET_SET_TIME, usTimer);

    usPrevErrorCo = 0;
    if (((pSend->usFunCode & CLI_RSTCONTCODE ) != CLI_FCSPSPOLLING ) &&   /* add R3.0 */
        ((pSend->usFunCode & CLI_RSTCONTCODE ) != CLI_FCSPSPRINTKP )) { 
        CliDispDescriptor (CLI_MODULE_STUB, CLI_DESC_START_BLINK);		//Blinks the COM descriptor light
    }

	/*
		Wait on a response from the target user which is normally the Server user thread
		on the Master terminal.  After the PifNetReceive returns, we check the return
		code for a network layer error and if that is okay (sError > 0 as sError is number
		of bytes in the received response message) then we check to see if the response
		message itself contains an error indication.

		We explicitly check to see if the response code is a STUB_BUSY as this is the
		most likely response code indicating an error condition of some kind from the
		Server user thread.  Check for the numerous cases of the Server subsystem using
		the function SerSendError () to return a STUB_BUSY response when the Server user
		on the Master terminal is busy processing a multi-send or a multi-receive.

		The function CstComCheckError () will look at CliMsg.sError and if it is
		equal to STUB_SUCCESS, CstComCheckError () will set CliMsg.sError to pResp->sResCode.
		We go ahead and do it explicitly here.  See CstComSendReceive () for how the steps
		for a Client send message, receive response, process response is handled.
	 */
    do {
        CliNetConfig.fchStatus |= CLI_NET_RECEIVE;
        sError = PifNetReceive(CliNetConfig.usHandle, pCliRcvBuf, sizeof(XGRAM));
        CliNetConfig.fchStatus &= ~CLI_NET_RECEIVE;

        if (0 > sError) {
            if (PIF_ERROR_NET_POWER_FAILURE == sError) {
                sError = STUB_POWER_DOWN;
            } else if (PIF_ERROR_NET_TIMEOUT == sError || PIF_ERROR_NET_CLEAR == sError) {
                sError = STUB_TIME_OUT;
            } else if (PIF_ERROR_NET_BUSY == sError) {
				// if we get a busy indication then we need to retry the send and receive.
				// this error indicates the target was busy and did not get to our message.
                PifLog(MODULE_STB_LOG, LOG_ERROR_STB_3TIME_ERROR);
                PifLog(MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)abs(sError));
                PifLog(MODULE_LINE_NO(MODULE_STB_LOG), (USHORT)__LINE__);
                sError = STUB_BUSY;
            } else if (PIF_ERROR_NET_DISCOVERY == sError) {
                sError = STUB_DISCOVERY;
            } else { 
                PifLog(MODULE_STB_LOG, LOG_ERROR_STB_3TIME_ERROR);
                PifLog(MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)abs(sError));
                PifLog(MODULE_LINE_NO(MODULE_STB_LOG), (USHORT)__LINE__);
            }
        } else {
			if (pCliSndBuf->auchFaddr[CLI_POS_UA] != 255 && pCliSndBuf->auchFaddr[CLI_POS_UA] != 0 &&
				pCliSndBuf->auchFaddr[CLI_POS_UA] != pCliRcvBuf->auchFaddr[CLI_POS_UA]) {
				PifLog (MODULE_STB_LOG, LOG_EVENT_STB_IP_MISMATCH);
                sError = STUB_ILLEGAL;
            } else if ( (pSend->usFunCode & CLI_RSTCONTCODE ) != (pResp->usFunCode & CLI_RSTCONTCODE) ) {
				PifLog (MODULE_STB_LOG, LOG_EVENT_STB_RSTCONTCODE_MISMATCH);
                sError = STUB_ILLEGAL;
            } else if ((pSend->usSeqNo & CLI_SEQ_CONT) != (pResp->usSeqNo & CLI_SEQ_CONT)) {
				PifLog (MODULE_STB_LOG, LOG_EVENT_STB_SEQNUMBER_MISMATCH);
                sError = STUB_ILLEGAL;
            } else {
                sError = STUB_SUCCESS;
            }
        }
		if (--sPifCountDown <= 0) {
			PifLog (MODULE_STB_LOG, LOG_EVENT_STB_CNT_DOWN_NET_RCV);
			sPifCountDown = PIF_LOG_COUNT_DOWN_RESET;
		}
    } while (STUB_ILLEGAL == sError);

    CliDispDescriptor(CLI_MODULE_STUB, CLI_DESC_STOP_BLINK);
    pCliMsg->sError = sError;

	return sError;
}

/*
*===========================================================================
** Synopsis:    SHORT    CstSendTarget(UCHAR uchUaddr);
*
*     Input:    uchUaddr   - uniuqe address of treget terminal
*
** Return:      STUB_SUCCESS:   communication success
*               STUB_SELF:      I am update myself terminal      
*               STUB_M_DOWN:    Master terminal is out of service
*               STUB_ILLEGAL:   Target address is 0.
*
** Description:  This function supports to communicate with Target terminal.
*                To check the status of the send and receive, you will need
*                to check CliMsg.sError for network layer errors and the
*                value of CliMsg.sRetCode for the function return status
*                meaning the lower level function such as ShrPrint () which
*                was invoked by the server to handle the request.
*===========================================================================
*/
SHORT   CstSendTarget(UCHAR uchUaddr, CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf)
{
    if ( uchUaddr == CliNetConfig.auchFaddr[CLI_POS_UA]) {
        return STUB_SELF;
    }

    if ( 0 == uchUaddr) {
        return STUB_ILLEGAL;
    }

    CstComMakeMessage(uchUaddr, CLI_PORT_STUB, pCliMsg, pCliSndBuf);
    CstComSendReceive(pCliMsg, pCliSndBuf, pCliRcvBuf);

	// Transform a Backup down or Master down message into
	// a timeout message as that is what users of CstSendTarget()
	// will be looking for.
	if (STUB_BM_DOWN == pCliMsg->sError) {
		pCliMsg->sError = STUB_TIME_OUT;
		pCliMsg->sRetCode = STUB_RETCODE_TIME_OUT_ERROR;
	} else if (STUB_M_DOWN == pCliMsg->sError) {
		pCliMsg->sError = STUB_TIME_OUT;
		pCliMsg->sRetCode = STUB_RETCODE_TIME_OUT_ERROR;
	}

    return pCliMsg->sError;  //message successfully sent to target (failures return above)
}

/*
*===========================================================================
** Synopsis:    VOID    CstCpmRecMessage(VOID);
*
** Return:      none.
*
** Description: This function waits for respose from CPM Host.
*===========================================================================
*/
VOID    CstCpmRecMessage(VOID)
{
	extern XGRAM       CliSndBuf;          /* Send Buffer */    
	extern XGRAM       CliRcvBuf;          /* Receive Buffer */
	extern  CLICOMIF   CliMsg;
	SHORT       sRetCode;
    CLIREQCOM   *pSend;
 
    pSend = (CLIREQCOM *)CliSndBuf.auchData;
    pSend->usFunCode = CLI_FCCPMRECVMESS;

    sRetCode = CstComChekInquiry();             /* check inquiry condition */
    CstNetReceive(CLI_MDC_CPMWAIT, &CliMsg, &CliSndBuf, &CliRcvBuf);             /*=== RECEIVE RESPONSE ===*/
    CstComCheckError(&CliMsg, &CliRcvBuf);                         /* error check */
    CstComRespHandle(&CliMsg, &CliSndBuf, &CliRcvBuf);                         /* response handling */

    if ( STUB_SUCCESS != CliMsg.sError )  {
        CliMsg.sRetCode = CliMsg.sError ;      /* Copy Stub Error */
        
        if ( STUB_POWER_DOWN == CliMsg.sError ) {
            CstNetClose();
            CstNetOpen();
        } else if ( STUB_TIME_OUT == CliMsg.sError || STUB_DISCOVERY == CliMsg.sError ) {
            CliMsg.sRetCode = CPM_RET_TIMEOUT;
        }
    }
}

/*
*===========================================================================
** Synopsis:    SHORT    CstSendMasterFH( USHORT usType );
*                           
*     Input:    usType      - type of function
*
** Return:      STUB_SUCCESS:   communication success
*               STUB_SELF:      I am update Master terminal      
*               STUB_M_DOWN:    Master terminal is out of service
*
** Description: This function supports to communicate with Master terminal
*               (file handle I/F version).
*===========================================================================
*/
#if defined(CstSendMasterFH)

SHORT   CstSendMasterFH_Special( USHORT usType );
SHORT   CstSendMasterFH_Debug( USHORT usType, char *aszFilePath, int nLineNo)
{
	extern XGRAM       CliSndBuf;          /* Send Buffer */    
	extern XGRAM       CliRcvBuf;          /* Receive Buffer */
	extern  CLICOMIF   CliMsg;
	char  xBuffer[256];

	sprintf (xBuffer, "CstSendMasterFH_Debug(): File %s  lineno = %d", RflPathChop(aszFilePath), nLineNo);
	NHPOS_ASSERT_TEXT(0, xBuffer);
	return CstSendMasterFH_Special(usType);
}

SHORT   CstSendMasterFH_Special( USHORT usType )
#else
SHORT   CstSendMasterFH( USHORT usType )
#endif
{
	extern XGRAM       CliSndBuf;          /* Send Buffer */    
	extern XGRAM       CliRcvBuf;          /* Receive Buffer */
	extern  CLICOMIF   CliMsg;
	USHORT             usCstComReadStatus;
	SHORT              sRetCode;

#if !defined(FORCE_SATELLITE_TERMINAL)
	if (STUB_SUCCESS == CstIamDisconnectedUnjoinedSatellite()) {
		// We do not care about the Master update status if we
		// are an unjoined or Disconnected Satellite.
       return STUB_SELF; // do not send to master, simply return the "I am Master" message
	}
#endif

    usCstComReadStatus = CstComReadStatus();    // read the Notice Board system status for CstSendMasterFH()
    if (usCstComReadStatus & CLI_STS_BACKUP_FOUND) { //if notice board says Backup Master provisioned
		CLIRESCOM   *pResp = (CLIRESCOM *)CliRcvBuf.auchData;

		if (0 == (usCstComReadStatus & CLI_STS_M_UPDATE)) { //if notice board says Master is off line and unavailable
			CliMsg.sError = STUB_M_DOWN;
			CliMsg.sRetCode = (STUB_M_DOWN + STUB_RETCODE);
			pResp->sResCode = CliMsg.sRetCode;
			pResp->sReturn = CliMsg.sRetCode;
			if (sMasterErrorSave != STUB_M_DOWN) {
				char xBuff[128];
				sprintf(xBuff, "**WARNING: CstSendMasterFH: (0 == (CstComReadStatus() & CLI_STS_M_UPDATE)) - 0x%x", usCstComReadStatus);
				NHPOS_ASSERT_TEXT((sMasterErrorSave == STUB_M_DOWN), xBuff);
			}
			sMasterErrorSave = STUB_M_DOWN;
			return STUB_M_DOWN;					// return master status as down, do not send message
		}

		if (usCstComReadStatus & CLI_STS_BM_UPDATE) {
			// if Terminal #1 (Master Terminal) indicates it is up to date then check if we are doing inquiry.
			// if it is not up to date then we will not bother.
			// Move check on inquiry state to after check if Master terminal or not
			// since this speeds up terminal startup when using Dynamic PLU buttons at
			// some locations such as Compass.
			sRetCode = CstComChekInquiry();
			if (sRetCode != STUB_SUCCESS) {
				if (sRetCode == STUB_FAITAL) {
					if (CLI_IAM_BMASTER)
						sRetCode = STUB_M_DOWN;
					else if (CLI_IAM_MASTER)
						sRetCode = STUB_SELF;
					else
						sRetCode = STUB_DUR_INQUIRY;
				}
				// If we are in inquiry mode and we get this return status
				// then we are neither Master Terminal nor Backup Master Terminal.
				// Therefore we can't be sure who to talk with so lets just
				// return this status.
				CliMsg.sError = sRetCode;
				CliMsg.sRetCode = (sRetCode + STUB_RETCODE);
				pResp->sResCode = CliMsg.sRetCode;
				pResp->sReturn = CliMsg.sRetCode;
				if (sMasterErrorSave != sRetCode) {
					char xBuff[128];
					sprintf(xBuff, "CstSendMasterFH: CstComChekInquiry() %d, usCstComReadStatus = 0x%x", sRetCode, usCstComReadStatus);
					NHPOS_ASSERT_TEXT((sMasterErrorSave == sRetCode), xBuff);
				}
				sMasterErrorSave = sRetCode;
				return sRetCode;		// return master status as possibly down, do not send message
			}
		}
	}

	// This #if construct provides a way for a developer doing testing to
	// make a Master terminal perform actions similar to a Satellite terminal.
	// To enable the functionality define  FORCE_SATELLITE_TERMINAL which will then
	// enable the #if clause.
#if defined(FORCE_SATELLITE_TERMINAL)
	// This #if clause is executed if the developer wants to do testing of
	// Satellite terminal functionality with a single terminal.
	// This functionality should never be enabled for a Release build so it
	// has a check to trigger a compiler error if the compile is being done
	// in Release mode.
	{
#pragma message("  ****   ")
		UCHAR dummyvalue = CstComReadStatus ();
#pragma message("  ****   CLI_IAM_MASTER in CstSendMasterFH() stubbed out.  Master sending messages like a Satellite.   ******")
	}
#if !defined(_DEBUG)
	I_Will_Create_An_Error++;
#endif
#pragma message("  ****   ")
#else
	 //if operation is occurring on Master Terminal or disconnected Satellite Terminal
	if (CLI_IAM_MASTER) {
		usCstfsSystemInf |= CLI_STS_M_REACHABLE;
		return STUB_SELF; // do not send to master, simply return the "I am Master" message
    }
#endif

	// Let us see if we are receiving Notice Board messages from the Master Terminal.
	// If not, indicated by Master marked as off line, then check to see if we have already
	// processed this condition.  If not, then process it.
	// However if we have then we want to go ahead and try this action anyway.
	// What can happen is that the Master Terminal is visible for a direct network message however
	// the broadcast UDP messages from the Master Terminal are not visible due to a subnetting
	// problem in which this terminal is on a different subnet than the Master Terminal which
	// may block broadcast UDP messages from the Master Terminal.
	if ((sMasterErrorSave != STUB_M_DOWN) && 0 != (usCstComReadStatus & CLI_STS_M_OFFLINE)) {  //if notice board says Master is offline and unavailable
		if ((usCstfsSystemInf & CLI_STS_M_REACHABLE) == 0) {
			CLIRESCOM   *pResp = (CLIRESCOM *)CliRcvBuf.auchData;
			SHORT sErrorTemp = STUB_M_DOWN;

			CliMsg.sError = sErrorTemp;
			CliMsg.sRetCode = (sErrorTemp + STUB_RETCODE);
			pResp->sResCode = CliMsg.sRetCode;
			pResp->sReturn = CliMsg.sRetCode;

			if (sMasterErrorSave != sErrorTemp) {
				char xBuff[128];
				sprintf(xBuff, "Master Offline: (0 != (CstComReadStatus() & CLI_STS_M_OFFLINE)) - 0x%x", usCstComReadStatus);
				NHPOS_ASSERT_TEXT((sMasterErrorSave == sErrorTemp), xBuff);
			}
			sMasterErrorSave = sErrorTemp;
			return sErrorTemp;					// return master status as down, do not send message
		}
	}

	if (0 == (usCstComReadStatus & CLI_STS_M_OFFLINE)) {
		// if Master Terminal is not off line then do normal processing of clearing
		// any error indicators.  However if it is off line then we will want to save
		// that status and go ahead and attempt this communication anyway.
		sMasterErrorSave = 0;
		if (sMasterErrorSave != 0) {
			char xBuff[128];
			sprintf(xBuff, "Master cleared: CstSendMasterFH() usCstComReadStatus = 0x%x", usCstComReadStatus);
			NHPOS_ASSERT_TEXT((sMasterErrorSave == 0), xBuff);
		}
	}

	CstComMakeMessage(CLI_TGT_MASTER, CLI_PORT_STUB, &CliMsg, &CliSndBuf);
    CstComSendReceiveFH(usType, &CliMsg, &CliSndBuf, &CliRcvBuf);

	// We are implementing a new communications status indicator of CLI_STS_M_REACHABLE
	// in order to indicate that an attempt to communicate with the Master Terminal succeeded.
	// At VCS we are seeing where some Satellite Terminals are put on a different LAN sub-net than
	// the Master Terminal.  The result is that the Satellite Terminal on a different sub-net is
	// not seeing the Notice Board messages of the Master Terminal since a UDP broadcast is not
	// being propagated to out side of the Master Terminal's sub-net.
	// Using this new indicator allows the use of a Satellite Terminal that is not in the same sub-net.
	if (STUB_TIME_OUT == CliMsg.sError) {
		usCstfsSystemInf &= ~CLI_STS_M_REACHABLE;
	} else {
		usCstfsSystemInf |= CLI_STS_M_REACHABLE;
	}
    return CliMsg.sError;
}

/*
*===========================================================================
** Synopsis:    SHORT    CstSendBMasterFH( USHORT usType );
*                           
*     Input:    usType      - type of function
*
** Return:      STUB_SUCCESS:   communication success
*               STUB_SELF:      I am update Back Up Master terminal      
*               STUB_BM_DOWN:   Back Up Master terminal is out of service
*
** Description: This function supports to communicate with Back Up Master terminal.
*               (file handle I/F version).
*===========================================================================
*/
#if defined(CstSendBMasterFH)

SHORT   CstSendBMasterFH_Special( USHORT usType );
SHORT   CstSendBMasterFH_Debug( USHORT usType, char *aszFilePath, int nLineNo)
{
	extern XGRAM       CliSndBuf;          /* Send Buffer */    
	extern XGRAM       CliRcvBuf;          /* Receive Buffer */
	extern  CLICOMIF   CliMsg;
	char  xBuffer[256];

	sprintf (xBuffer, "CstSendBMasterFH_Debug(): File %s  lineno = %d", RflPathChop(aszFilePath), nLineNo);
	NHPOS_ASSERT_TEXT(0, xBuffer);
	return CstSendBMasterFH_Special(usType);
}

SHORT   CstSendBMasterFH_Special( USHORT usType )
#else
SHORT   CstSendBMasterFH( USHORT usType )
#endif
{
	extern XGRAM       CliSndBuf;          /* Send Buffer */    
	extern XGRAM       CliRcvBuf;          /* Receive Buffer */
	extern  CLICOMIF   CliMsg;
	USHORT             usCstComReadStatus;
	SHORT              sRetCode = STUB_SUCCESS;

	if (STUB_SUCCESS == CstIamDisconnectedUnjoinedSatellite()) {
		// We do not care about the Backup Master update status if we
		// are an unjoined or Disconnected Satellite.
       return STUB_DISCOVERY; // do not send to Backup Master, simply return the STUB_DISCOVERY message
	}

	if (0 == (CliNetConfig.fchStatus & CLI_NET_BACKUP)) {
        return STUB_DISCOVERY;
    }

    usCstComReadStatus = CstComReadStatus();    // read the Notice Board system status for CstSendMasterFH()
    if (0 == (usCstComReadStatus & CLI_STS_BM_UPDATE)) { //if notice board says Backup Master is NOT updating
		CliMsg.sError = STUB_BM_DOWN;
		CliMsg.sRetCode = (STUB_BM_DOWN + STUB_RETCODE);
		if (sBackMasterErrorSave != STUB_BM_DOWN) {
			char xBuff[128];
			sprintf(xBuff, "CstSendBMasterFH: (0 == (CstComReadStatus() & CLI_STS_BM_UPDATE)) - 0x%x", usCstComReadStatus);
			NHPOS_ASSERT_TEXT((sBackMasterErrorSave == STUB_BM_DOWN), xBuff);
		}
		sBackMasterErrorSave = STUB_BM_DOWN;
        return STUB_BM_DOWN;					// return master status as down, do not send message
    }

    if (CLI_IAM_BMASTER) {
        return STUB_SELF;
    }

	if (0 != (usCstComReadStatus & CLI_STS_BM_OFFLINE)) {  //if notice board says Backup Master is offline and unavailable
		CLIRESCOM   *pResp = (CLIRESCOM *)CliRcvBuf.auchData;
		SHORT sErrorTemp = STUB_BM_DOWN;

		CliMsg.sError = sErrorTemp;
		CliMsg.sRetCode = (sErrorTemp + STUB_RETCODE);
		pResp->sResCode = CliMsg.sRetCode;
		pResp->sReturn = CliMsg.sRetCode;

		if (sBackMasterErrorSave != sErrorTemp) {
			char xBuff[128];
			sprintf(xBuff, "Backup Master Offline: (0 != (CstComReadStatus() & CLI_STS_BM_OFFLINE)) - 0x%x", usCstComReadStatus);
			NHPOS_ASSERT_TEXT((sBackMasterErrorSave == sErrorTemp), xBuff);
		}
		sBackMasterErrorSave = sErrorTemp;
		return sErrorTemp;					// return master status as down, do not send message
	}

	if (usCstComReadStatus & CLI_STS_M_UPDATE) {
		// if Terminal #1 (Master Terminal) indicates it is up to date then check if we are doing inquiry.
		// if it is not up to date then we will not bother.
		// Move check on inquiry state to after check if Master terminal or not
		// since this speeds up terminal startup when using Dynamic PLU buttons at
		// some locations such as Compass.
		sRetCode = CstComChekInquiry();
		if (sRetCode != STUB_SUCCESS) {
			if (sRetCode == STUB_FAITAL) {
				if (CLI_IAM_MASTER)
					sRetCode = STUB_BM_DOWN;
				else if (CLI_IAM_BMASTER)
					sRetCode = STUB_SELF;
				else
					sRetCode = STUB_DUR_INQUIRY;
			}
			// If we are in inquiry mode and we get this return status
			// then we are neither Master Terminal nor Backup Master Terminal.
			// Therefore we can't be sure who to talk with so lets just
			// return this status.
			CliMsg.sError = sRetCode;
			CliMsg.sRetCode = (sRetCode + STUB_RETCODE);
			if (sBackMasterErrorSave != sRetCode) {
				char xBuff[128];
				sprintf(xBuff, "CstSendBMasterFH: CstComChekInquiry() %d, usCstComReadStatus = 0x%x", sRetCode, usCstComReadStatus);
				NHPOS_ASSERT_TEXT((sBackMasterErrorSave == STUB_DUR_INQUIRY), xBuff);
			}
			sBackMasterErrorSave = sRetCode;
			return sRetCode;		// return master status as possibly down, do not send message
		}
	}
	if (sBackMasterErrorSave != 0) {
		char xBuff[128];
		sprintf(xBuff, "CstSendBMasterFH cleared: usCstComReadStatus = 0x%x", usCstComReadStatus);
		NHPOS_ASSERT_TEXT((sBackMasterErrorSave == 0), xBuff);
	}
	sBackMasterErrorSave = 0;

    CstComMakeMessage(CLI_TGT_BMASTER, CLI_PORT_STUB, &CliMsg, &CliSndBuf);
    CstComSendReceiveFH(usType, &CliMsg, &CliSndBuf, &CliRcvBuf);

    return CliMsg.sError;
}


/*
*===========================================================================
** Synopsis:    SHORT    CstSendTerminalFH(USHORT usType, USHORT usTerminalPosition);
*
*     Input:    usType              - type of function
*               usTerminalPosition  - terminal number 1 through 16 to send to
*
** Return:      STUB_SUCCESS:   communication success
*
** Description: 
*===========================================================================
*/
SHORT   CstSendTerminalFH(USHORT usType, USHORT usTerminalPosition)
{
	extern XGRAM       CliSndBuf;          /* Send Buffer */    
	extern XGRAM       CliRcvBuf;          /* Receive Buffer */
	extern  CLICOMIF   CliMsg;
	SHORT  sRetStatus = STUB_SUCCESS;

    CstComMakeMessage(usTerminalPosition, CLI_PORT_STUB, &CliMsg, &CliSndBuf);
    CstComSendReceiveFH(usType, &CliMsg, &CliSndBuf, &CliRcvBuf);

    return CliMsg.sError;  //message successfully sent to master (failures return above)
}

/*
*===========================================================================
** Synopsis:    VOID    CstComSendReceiveFH( USHORT usType );
*                           
*     Input:    usType      - type of function
*
** Return:      none.
*
** Description:
*   This function sends request to target server and receives response
*   (file handle I/F version).
*===========================================================================
*/
static VOID    CstComSendReceiveFH( USHORT usType, CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf )
{
    USHORT  usSendLen;
	USHORT  usRetryCount;
	SHORT   sError = PIF_OK;

    CstNetClear();                              /* clear receive buffer */ 

    pCliMsg->usRetryCo = 0;
    usRetryCount = CstReadMDCPara(CLI_MDC_RETRY);

    do {
		if (usType == CLI_FCBAKGCF) {
			usSendLen = CstComMakeMultiData(pCliMsg, pCliSndBuf);  /* make request data */
		} else {
			usSendLen = CstComMakeMultiDataFH(pCliMsg, pCliSndBuf);    /* make request data for file handle I/F */
		}
		sError = CstNetSend(usSendLen, pCliSndBuf);         /*=== SEND MESSAGE ===*/
		if (0 <= sError) {
			CstNetReceive(CLI_MDC_CLIENT, pCliMsg, pCliSndBuf, pCliRcvBuf);          /*=== RECEIVE RESPONSE ===*/
			CstComCheckError(pCliMsg, pCliRcvBuf);  /* error check */
			CstComRespHandleFH(pCliMsg, pCliSndBuf, pCliRcvBuf);                   /* response handling */
			CstComErrHandle(pCliMsg, pCliSndBuf, pCliRcvBuf);                      /* error handling */
			CstComSendConfirm(pCliMsg, pCliSndBuf); /* send confirm, if need */    
		}
		else {
			PifLog(MODULE_STB_LOG, LOG_ERROR_STB_NETWORK_ERROR_FH);
			PifLog(MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)abs(sError));
			pCliMsg->sError = STUB_ILLEGAL;
			switch (sError) {
//			case PIF_ERROR_NET_WRITE:
//				break;
			case PIF_ERROR_NET_OFFLINE:
				pCliMsg->sError = STUB_NET_NOT_OPENED;
				break;

			case PIF_ERROR_NET_ERRORS:
			case PIF_ERROR_NET_UNREACHABLE:
				// Fall thru and treat PIF_ERROR_NET_UNREACHABLE like PIF_ERROR_NET_TIMEOUT
			case PIF_ERROR_NET_TIMEOUT:
				pCliMsg->sError = STUB_TIME_OUT;
				break;

			case PIF_ERROR_NET_BUSY:
				pCliMsg->sError = STUB_BUSY;
				break;

			case PIF_ERROR_NET_POWER_FAILURE:
				pCliMsg->sError = STUB_POWER_DOWN;
				break;

			default:
				{
					CLIREQCOM    *pSend = (CLIREQCOM *)pCliSndBuf->auchData;
					char xBuff[256];
					sprintf(xBuff, "CstComSendReceiveFH: Unknown CstNetSend() error %d, usFunCode 0x%x, usRetryCo = %d, usSeqNo = 0x%x", sError, pCliMsg->usFunCode, pCliMsg->usRetryCo, pSend->usSeqNo);
					NHPOS_ASSERT_TEXT(0, xBuff);
				}
				break;
			}
//			CstComErrHandle(pCliMsg, pCliSndBuf, pCliRcvBuf);                      /* error handling */
			pCliMsg->usRetryCo++;
			PifSleep(50);
		}

		if (pCliMsg->usRetryCo >= usRetryCount) {
			PifLog (MODULE_STB_LOG, LOG_EVENT_STB_CNT_DOWN_NET_RCV_FH);
			PifLog(MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)(abs(sError)));
			PifLog(MODULE_DATA_VALUE(MODULE_STB_LOG), (USHORT)(abs(pCliMsg->sError)));
			PifLog(MODULE_LINE_NO(MODULE_STB_LOG), (USHORT)__LINE__);
		}

		NHPOS_ASSERT((STUB_BM_DOWN != pCliMsg->sError) && (STUB_M_DOWN != pCliMsg->sError));

		if (STUB_BM_DOWN == pCliMsg->sError) {
			break;
		} else if (STUB_M_DOWN == pCliMsg->sError) {
			break;
		}
    } while (STUB_SUCCESS != pCliMsg->sError && pCliMsg->usRetryCo < usRetryCount);

	if (pCliMsg->usRetryCo >= usRetryCount || (STUB_BM_DOWN == pCliMsg->sError) || (STUB_M_DOWN == pCliMsg->sError)) {
		CLIREQCOM    *pSend = (CLIREQCOM *)pCliSndBuf->auchData;
		char         xBuff[256];
		sprintf (xBuff, "##CstComSendReceiveFH(): usUA %d, sError = %d, usFunCode 0x%x, usRetryCo = %d, usSeqNo = 0x%x", pCliSndBuf->auchFaddr[CLI_POS_UA], pCliMsg->sError, pCliMsg->usFunCode, pCliMsg->usRetryCo, pSend->usSeqNo);
		NHPOS_ASSERT_TEXT((STUB_SUCCESS == pCliMsg->sError), xBuff);

		if (pCliMsg->usRetryCo >= usRetryCount)
			pCliMsg->sRetCode = STUB_RETCODE_TIME_OUT_ERROR;
		else 
			pCliMsg->sRetCode = (pCliMsg->sError + STUB_RETCODE);
	}
}

/*
*===========================================================================
** Synopsis:    VOID    CstComMakeMultiDataFH(VOID);
*
** Return:      data length to be sent.
*
** Description: This function makes request data into send buffer
*               (file handle I/F version).  Any changes to this function
*               may need to be synchronized with changes to function SerRMHCheckData()
*               in file servrmh.c which contains a check on the length of the
*               data received.
*
*               This function updates the message sequence number and indicators and
*               reads the next packet from the temporary file that is being transferred
*               into the CliMsg buffer.  The function that handles the received
*               data packet on the Server side is function SerRecvMultiple().
*===========================================================================
*/
static USHORT  CstComMakeMultiDataFH(CLICOMIF *pCliMsg, XGRAM *pCliSndBuf)
{
    CLIREQCOM   *pSend;
    USHORT      usDataSiz=0;
    USHORT      usDataMax;
    USHORT      usStartPoint=0;

    pSend = (CLIREQCOM *)pCliSndBuf->auchData;

    if ((0 != pCliMsg->usReqLen) &&
        ((0 == pSend->usSeqNo) || (pCliMsg->sError == STUB_MULTI_RECV))) {

        pSend->usSeqNo ++;
        pSend->usSeqNo |= CLI_SEQ_SENDDATA;
        if (pCliMsg->usAuxLen > pCliMsg->usReqLen) {
            pCliMsg->usAuxLen = pCliMsg->usReqLen;
        }
        usDataSiz = pCliMsg->usReqLen - pCliMsg->usAuxLen;
        usDataMax = PIF_LEN_UDATA - (pCliMsg->usReqMsgHLen + PIF_LEN_IP + sizeof(XGHEADER) + 10);
        if (usDataSiz > usDataMax) {
            usDataSiz = usDataMax;
        } else {
            pSend->usSeqNo |= CLI_SEQ_SENDEND;
        }
        memcpy((UCHAR *)pSend + pCliMsg->usReqMsgHLen, &usDataSiz, 2);
        if ((pCliMsg->usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLUPDATE) {
            usStartPoint = sizeof(USHORT);
		} else if ((pCliMsg->usFunCode & CLI_RSTCONTCODE) == CLI_FCCOPONNENGINEMSGFH) {
            usStartPoint = 0;
        } else {
            usStartPoint = sizeof(USHORT) + sizeof(USHORT);
        }

        SstReadFileFH((pCliMsg->usAuxLen + usStartPoint), (UCHAR *)pSend + pCliMsg->usReqMsgHLen + 2, usDataSiz, (SHORT)*(pCliMsg->pauchReqData));

        pCliMsg->usAuxLen += usDataSiz;
        usDataSiz += 2;

    } else if (0 != pCliMsg->usResLen) {
        pSend->usSeqNo &= CLI_SEQ_CONT;
        ++ pSend->usSeqNo;
    }
    if (CLI_TGT_MASTER == pCliSndBuf->auchFaddr[CLI_POS_UA]) {
        if (0 == (CstComReadStatus() & CLI_STS_BM_UPDATE)) {
            pSend->usFunCode |= CLI_SETBMDOWN;
        } else {
            pSend->usFunCode &= ~CLI_SETBMDOWN;
        }
    } else {
        if (0 == (CstComReadStatus() & CLI_STS_M_UPDATE)) {
            pSend->usFunCode &= ~CLI_FCWBACKUP;
        }
    }
    pCliMsg->sError = STUB_SUCCESS;
    return (CLI_MAX_IPDATA + pCliMsg->usReqMsgHLen + usDataSiz);
}
        
/*
*===========================================================================
** Synopsis:    SHORT    SstReadFAsMasterFH(VOID);
*
** Return:      STUB_SELF:  I am Master
*               STUB_SUCCESS:   read OK from master  
*
** Description: This function tries to read data from master.
*               (file handle I/F version).  This function should only be for
*               a read from the Master Terminal which may need to fall back
*               to the Backup Master, if provisioned, in the case of being
*               unable to get a response from the Master Terminal.
*===========================================================================
*/
SHORT   SstReadFAsMasterFH(USHORT usType)
{
    SHORT   sError, sErrorM;

    sError = sErrorM = CstSendMasterFH(usType);                   /* Send to Master */
    if (STUB_M_DOWN == sErrorM || STUB_TIME_OUT == sErrorM) {     /* master down */
		if (0 != (CliNetConfig.fchStatus & CLI_NET_BACKUP)) {
			sError = CstSendBMasterFH(usType);                    /* Send to B Master */
			if (STUB_SUCCESS != sError) {                         /* Backup Master down */
				char xBuff[128];
				sprintf (xBuff, "==NOTE: SstReadFAsMasterFH() sErrorM %d, sError %d", sErrorM, sError);
				NHPOS_NONASSERT_TEXT(xBuff);
				sError = sErrorM;
			}
		}
    }
    return sError;                            /* Comm. Success */
}

/*
*===========================================================================
** Synopsis:    VOID    CstComRespHandleFH(VOID);
*
** Return:      none.
*
** Description:
*   This function copies received message and data to specified buffer.
*   (file handle I/F version).
*===========================================================================
*/
SHORT    CstComRespHandleFH(CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf)
{

    CLIREQCOM   *pSend;
    CLIRESCOM   *pResp;
    USHORT      usDataSiz;
    USHORT      usOffset = sizeof(USHORT);

    pSend = (CLIREQCOM *)pCliSndBuf->auchData;
    pResp = (CLIRESCOM *)pCliRcvBuf->auchData;

    if ((STUB_SUCCESS != pCliMsg->sError) && (STUB_MULTI_SEND != pCliMsg->sError)) {
//		char  xBuff[128];
//		sprintf (xBuff, "CstComRespHandleFH(): sError = %d, usFunCode 0x%x, usSeqNo 0x%x", pCliMsg->sError, pResp->usFunCode, pResp->usSeqNo);
        return pCliMsg->sError;
    }

    if ((CLI_TGT_BMASTER == pCliSndBuf->auchFaddr[CLI_POS_UA]) && (pSend->usFunCode & CLI_FCWBACKUP)) {
        if (0 > pResp->sReturn) {
            if (((pSend->usFunCode & CLI_RSTCONTCODE) == CLI_FCEJREAD) && (pResp->sReturn == EJ_NOTHING_DATA)) {
                pCliMsg->sError = STUB_SUCCESS;
            } else {
                pCliMsg->sError = STUB_BM_DOWN;
            }
        }
        /******** store retrieved ej data to user's buffer (R3.1) *******/
        if ((pSend->usFunCode & CLI_RSTCONTCODE) == CLI_FCEJREAD) {
            memcpy(pCliMsg->pResMsgH, pResp, pCliMsg->usResMsgHLen);
            pCliMsg->sRetCode = pResp->sReturn;
        }
        /******** store retrieved ej data to user's buffer (R3.1) *******/
        return pCliMsg->sError;
    }
    memcpy(pCliMsg->pResMsgH, pResp, pCliMsg->usResMsgHLen);
    pCliMsg->sRetCode = pResp->sReturn;
    if (pResp->usSeqNo & CLI_SEQ_RECVDATA) {
        if (pSend->usSeqNo & CLI_SEQ_SENDEND) {
            pCliMsg->usAuxLen = 0;
        }

        memcpy(&usDataSiz, (UCHAR *)pResp + pCliMsg->usResMsgHLen, 2);
        if (((pSend->usFunCode & CLI_RSTCONTCODE) == CLI_FCGCFREADLOCK) ||
            ((pSend->usFunCode & CLI_RSTCONTCODE) == CLI_FCGCFINDREAD)  ||
            ((pSend->usFunCode & CLI_RSTCONTCODE) == CLI_FCBAKGCF) ||
            ((pSend->usFunCode & CLI_RSTCONTCODE) == CLI_FCGCFCHECKREAD)) {
            if ((pSend->usFunCode & CLI_RSTCONTCODE) != CLI_FCBAKGCF) {
                usOffset += sizeof(USHORT);
            }
            if ((usDataSiz + pCliMsg->usAuxLen) <= pCliMsg->usResLen) {
                SstWriteFileFH((pCliMsg->usAuxLen + usOffset), (UCHAR *)pResp + pCliMsg->usResMsgHLen + 2,
                               usDataSiz, ( SHORT)*(pCliMsg->pauchResData));
                pCliMsg->usAuxLen += usDataSiz;
            } else {
                if (pCliMsg->usAuxLen < pCliMsg->usResLen) {
                    usDataSiz = pCliMsg->usResLen - pCliMsg->usAuxLen;
                    SstWriteFileFH((pCliMsg->usAuxLen + usOffset), (UCHAR *)pResp + pCliMsg->usResMsgHLen + 2,
                                   usDataSiz, ( SHORT)*(pCliMsg->pauchResData));
                    pCliMsg->usAuxLen += usDataSiz;
                }
            }
        } 
	}

	return pCliMsg->sError;
}

/*
*==========================================================================
**    Synopsis:   SHORT SstReadFileFH(USHORT  offusSeekPos, 
*                                     VOID    *pReadBuffer, 
*                                     USHORT  usReadSize, 
*                                     SHORT   hsFileHandle)
*
*       Input:    USHORT    offusSeekPos   Seek value from file head
*                 USHORT    usReadSize     Read buffer size 
*                 SHORT     hsFileHandle   File handle of file to be read data
*                 
*      Output:    VOID      *pReadBuffer   Read buffer
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*               Read data from target file.              
*==========================================================================
*/      
VOID SstReadFileFH(ULONG     offulSeekPos,
                   VOID      *pReadBuffer, 
                   ULONG     ulReadSize,
                   SHORT     hsFileHandle)
{
    ULONG   ulActualPosition;
	ULONG	ulActualBytesRead; //RPH 11-7-3

    if (ulReadSize == 0) {
        return;
    }

    if ((PifSeekFile(hsFileHandle, offulSeekPos, &ulActualPosition)) < 0) {
        PifLog(MODULE_SER_ABORT, FAULT_ERROR_FILE_SEEK);
        PifLog(MODULE_DATA_VALUE(MODULE_SER_ABORT), (USHORT)offulSeekPos);
        PifLog(MODULE_DATA_VALUE(MODULE_SER_ABORT), (USHORT)ulActualPosition);
        PifLog(MODULE_LINE_NO(MODULE_SER_ABORT), (USHORT)__LINE__);
        PifAbort(MODULE_SER_ABORT, FAULT_ERROR_FILE_SEEK);
    }

	//RPH 11-7-3 Changes for PifReadFile
	PifReadFile(hsFileHandle, pReadBuffer, ulReadSize, &ulActualBytesRead);
    if ( ulActualBytesRead != ulReadSize) {
        PifAbort(MODULE_SER_ABORT, FAULT_ERROR_FILE_READ);
    }

}

/*
*==========================================================================
**    Synopsis:   VOID SstWriteFileFH(USHORT offusSeekPos, 
*                                     VOID   *pWriteBuffer, 
*                                     USHORT usWriteSize,
*                                     SHORT  hsFileHandle)
*
*       Input:    USHORT    offusSeekPos   Seek value from file head for writing.
*                 VOID      *pWriteBuffer  Write buffer
*                 USHORT    usWriteSize    Write buffer size 
*                 
*      Output:    Nothing 
*       InOut:    SHORT     hsFileHandle   File handle of file to be written data 
*
**  Return   :    Nothing
*
**  Description:
*               Write data to target file.               
*==========================================================================
*/                          
VOID SstWriteFileFH(ULONG  offulSeekPos,
                    VOID   *pWriteBuffer,
                    USHORT usWriteSize,
                    SHORT  hsFileHandle)
{
    ULONG   ulActualPosition;           /* for seek */

    if ((PifSeekFile(hsFileHandle, offulSeekPos, &ulActualPosition)) < 0) {
        PifLog(MODULE_SER_ABORT, FAULT_ERROR_FILE_SEEK);
        PifLog(MODULE_DATA_VALUE(MODULE_SER_ABORT), (USHORT)offulSeekPos);
        PifLog(MODULE_DATA_VALUE(MODULE_SER_ABORT), (USHORT)ulActualPosition);
        PifLog(MODULE_LINE_NO(MODULE_SER_ABORT), (USHORT)__LINE__);
        PifAbort(MODULE_SER_ABORT, FAULT_ERROR_FILE_SEEK);
    }
    if (usWriteSize == 0) {      /* PifWriteFile() is occurred system-error by read-size = 0 */
        return;
    }
    PifWriteFile(hsFileHandle, pWriteBuffer, usWriteSize);
}

/*===== END OF SOURCE =====*/