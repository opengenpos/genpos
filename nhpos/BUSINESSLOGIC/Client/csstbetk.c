/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-1995       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server STUB, Source File of ETK Function                   
* Category    : Client/Server STUB, US Hospitality Mode
* Program Name: CSSTBETK.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*       CliEtkTimeIn()         Time In    W/Backup
*       CliEtkTimeOut()        Time Out   W/Backup
*       SerEtkAssign()         Assign     W/Bcakup
*       SerEtkDelete()         Delete     W/Bcakup
*       SerEtkAllReset()       Reset      W/Bcakup
*       SerEtkIssuedSet()      Issued set W/Bcakup
*
*       CliEtkStatRead()       Read Job at Reg Mode
*       CliEtkIndJobRead()     Ind. Job code read at Reg Mode   Add R3.1
*
*       SerEtkIndJobRead()     Ind. Job code read     by Super
*       SerEtkCurAllIdRead()   Read all current ID    by Super
*       SerEtkSavAllIdRead()   Read all saved   ID    by Super
*       SerEtkCurIndRead()     Read current Ind. read by Super
*       SerEtkSavIndRead()     Read saved   Ind. read by Super
*       SerEtkAllLock()        All lock               by Super
*       SerEtkAllUnLock()      All unlock             by Super
*       SerEtkIssuedCheck()    Chec issued flag       by Super
*       SerEtkAllDataCheck()   Check data entry or not by ISP
*       SerEtkStatRead()       Read Job by Super 
*       SerEtkCurIndWrite()    Write a designate field by Super 
*
*          CstInitEtkMsgH()    Initialize Request/Response Message
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jun-18-93:H.Yamaguchi: initial                                   
* Jul-27-93:H.Yamaguchi: Adds CstInitEtkMsgH                                   
* Oct-05-93:H.Yamaguchi: Adds ???EtkStatread/SerEtkCurIndWrite fof FVT                                   
* Nov-10-95:T.Nakahata : R3.1 Initial
*                           Increase No. of Employees (200 => 250)
*                           Increase Employee No. ( 4 => 9digits)
*                           Add Employee Name (Max. 16 Char.)
*
** NCR2171 **                                         
* Aug-26-99:M.Teraki   :initial (for 2171)
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
#include    <uie.h>
#include    <regstrct.h>
#include    <paraequ.h>
#include	<para.h>
#include    <csetk.h>
#include    <csstbetk.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    "csstubin.h"
#include    "transact.h"

extern XGRAM       CliSndBuf;          /* Send Buffer */    
extern XGRAM       CliRcvBuf;          /* Receive Buffer */
extern  CLICOMIF   CliMsg;

/*
*==========================================================================
**    Synopsis:    SHORT   CliEtkTimeIn(ULONG      ulEmployeeNo,
*                                       UCHAR      uchJobCode,
*                                       ETK_FIELD  *pEtkField)
*
*       Input:     ulEmployeeNo    -   Employee NO to be time in
*                  uchJobCode      -   Job Code to be time in 
*        InOut:    *pEtkField
*
**  Return   
*     Normal End: ETK_SUCCESS
*                 ETK_NOT_TIME_OUT
*   Abnormal End: ETK_LOCK
*                 ETK_FILE_NOT_FOUND
*                 ETK_NOT_IN_FILE
*                 ETK_NOT_IN_JOB
*                 ETK_FIELD_OVER
*
**  Description: Etk time-in function.
*   Comments added 12.10.2002 by cwunn at GSU SR 8
*==========================================================================
*/
SHORT  CliEtkTimeIn(ULONG      ulEmployeeNo,
                    UCHAR      uchJobCode,
                    ETK_FIELD  *pEtkField)
{
	CLIREQETK       ReqMsgH = {0};
    CLIRESETKTIME   ResMsgH = {0};
    CLIRESETKTIME   ResMsgHSave = {0};
    SHORT           sError;
    SHORT           sCliRetCode;
    SHORT           sErrorM = STUB_SUCCESS, sErrorBM = STUB_SUCCESS;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulEmployeeNo  = ulEmployeeNo;
    ReqMsgH.uchJobCode    = uchJobCode;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKTIMEIN | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETKTIME);

	//if(operating on master terminal), CstSendMaster returns -21, meaning "I am master"
	//  in such a case the terminal will perform time in operations on itself
    if (STUB_SELF == (sErrorM = CstSendMaster())) {
        CliMsg.sRetCode = EtkTimeIn(ulEmployeeNo, uchJobCode, (ETK_FIELD *)ResMsgH.auchETKData);
		ResMsgH.usDataLen = sizeof(ETK_FIELD);
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

	sCliRetCode = CliMsg.sRetCode;
	ResMsgHSave = ResMsgH;
    if (STUB_SELF == (sErrorBM = CstSendBMaster())) { 
        CliMsg.sRetCode = EtkTimeIn(ulEmployeeNo, uchJobCode, (ETK_FIELD *)ResMsgH.auchETKData);
		ResMsgH.usDataLen = sizeof(ETK_FIELD);
		sErrorBM = STUB_SUCCESS;
        if (STUB_SUCCESS == sErrorM) {
			CliMsg.sRetCode = sCliRetCode;
			ResMsgH = ResMsgHSave;
        }
    } else if (sErrorBM != STUB_SUCCESS) {
		// If there was an error talking to the Backup Master Terminal
		// then we will return the status of the Master Terminal request.
		CliMsg.sRetCode = sCliRetCode;
		ResMsgH = ResMsgHSave;
	}

    memcpy(pEtkField, ResMsgH.auchETKData, ResMsgH.usDataLen);
    sError = CliMsg.sRetCode;    
    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		char xBuff[128];
		sprintf (xBuff, "==NOTE: CliEtkTimeIn() sError %d, sErrorM %d, sErrorBM %d", sError, sErrorM, sErrorBM);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return sError;    
}

/*                 
*==========================================================================
**    Synopsis:    SHORT   CliEtkTimeOut(ULONG      ulEmployeeNo,
*                                        ETK_FIELD  *pEtkField)
*       Input:    ulEmployeeNo
*      Output:    pEtkField
*       InOut:    Nothing
*
**  Return 
*     Normal End: ETK_SUCCESS
*                 ETK_NOT_TIME_IN
*   Abnormal End: ETK_LOCK
*                 ETK_FILE_NOT_FOUND
*                 ETK_NOT_IN_FILE
*                 ETK_FIELD_OVER
*
**  Description: Etk Sign-out function
*==========================================================================
*/
SHORT   CliEtkTimeOut(ULONG  ulEmployeeNo, ETK_FIELD  *pEtkField)
{
	CLIREQETK       ReqMsgH = {0};
    CLIRESETKTIME   ResMsgH = {0};
    CLIRESETKTIME   ResMsgHSave = {0};
    SHORT           sError;
    SHORT           sCliRetCode;
	SHORT			sErrorM = STUB_SUCCESS, sErrorBM = STUB_SUCCESS;

	// do not allow a Cashier who is Signed-in to perform a Time-out.  They must Sign-out first.
	// this applies to Amtrak and probably should apply to all customers.
	if (TranModeQualPtr->ulCashierID == ulEmployeeNo) {
		return LDT_PROHBT_ADR;
	}

	PifRequestSem(husCliExeNet);

    ReqMsgH.ulEmployeeNo  = ulEmployeeNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKTIMEOUT | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETKTIME);

    if (STUB_SELF == (sErrorM = CstSendMaster())) {
        CliMsg.sRetCode = EtkTimeOut(ulEmployeeNo, (ETK_FIELD *)ResMsgH.auchETKData);
		ResMsgH.usDataLen = sizeof(ETK_FIELD);
		sErrorM = STUB_SUCCESS;
    }

	sCliRetCode = CliMsg.sRetCode;
	ResMsgHSave = ResMsgH;
    if (STUB_SELF == (sErrorBM = CstSendBMaster())) {
        CliMsg.sRetCode = EtkTimeOut(ulEmployeeNo, (ETK_FIELD *)ResMsgH.auchETKData);
		ResMsgH.usDataLen = sizeof(ETK_FIELD);
		sErrorBM = STUB_SUCCESS;
        if (STUB_SUCCESS == sErrorM) {
            CliMsg.sRetCode = sCliRetCode;
			ResMsgH = ResMsgHSave;
        }
    } else if (sErrorBM != STUB_SUCCESS) {
		// If there was an error talking to the Backup Master Terminal
		// then we will return the status of the Master Terminal request.
		CliMsg.sRetCode = sCliRetCode;
		ResMsgH = ResMsgHSave;
	}

    memcpy(pEtkField, ResMsgH.auchETKData, ResMsgH.usDataLen);
    sError = CliMsg.sRetCode;    
    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		char xBuff[128];
		sprintf (xBuff, "==NOTE: CliEtkTimeOut() sError %d, sErrorM %d, sErrorBM %d", sError, sErrorM, sErrorBM);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return sError;    
}

/*
*==========================================================================
**    Synopsis:    SHORT   SerEtkAssign(ULONG  ulEmployeeNo,
*                                       ETK_JOB  *pEtkJob,
*                                       UCHAR    *pauchEmployeeName)
*
*       Input:     ulEmployeeNo    -   Employee Number to be assign
*                  pEtkJob         -   Job Code
*                  pauchEmployeeName - Employee Name
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: ETK_SUCCESS
*   Abnormal End: ETK_LOCK
*                 ETK_FILE_NOT_FOUND
*                 ETK_FULL
*
**  Description: Add, Regist Inputed Etk.
*==========================================================================
*/
SHORT  SerEtkAssign(ULONG    ulEmployeeNo,
                    ETK_JOB  *pEtkJob,
                    TCHAR    *pauchEmployeeName )
{
	CLIREQETK       ReqMsgH = {0};
    CLIRESETKTIME   ResMsgH = {0};
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulEmployeeNo  = ulEmployeeNo;

	NHPOS_ASSERT(sizeof(ETK_JOB) <= sizeof(ReqMsgH.auchNewJob));
    memcpy(ReqMsgH.auchNewJob, pEtkJob, sizeof(ETK_JOB));

    /* ===== Add Employee Name (Release3.1) BEGIN ===== */
    _tcsncpy(ReqMsgH.auchEmployeeName, pauchEmployeeName, CLI_EMPLOYEE_NAME);
    /* ===== Add Employee Name (Release3.1) END ===== */

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKASSIGN | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETKTIME);

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = EtkAssign(ulEmployeeNo, pEtkJob, pauchEmployeeName);
    } else {
        sRetCode = ETK_NOT_MASTER;
    }
    if (ETK_SUCCESS == sRetCode) {
        SstUpdateBackUp();
    }

    if (PifSysConfig()->ausOption[UIE_TEST_OPTION8] == UIE_TEST_PASS) {  /* SerEtkAssign() Debug Option, Generate Employee records */

    /*====================== TEST START ========================*/
        if ((ETK_SUCCESS == sRetCode) &&
            (pauchEmployeeName[0] == _T('A')) &&
            (pauchEmployeeName[1] == _T('0')) &&
            (pauchEmployeeName[2] == _T('A')) &&
            (pauchEmployeeName[3] == _T('1')))
		{
			ULONG   usI = 1;
			SHORT   sRetCodeTemp = sRetCode;
			TCHAR   tchEmployeeName[STD_CASHIERNAME_LEN] = {0};

			NHPOS_NONASSERT_NOTE("==TEST ENABLE", "==TEST ENABLE: Generate FLEX_ETK_MAX Employee records.");
			for (usI = 1; (ETK_SUCCESS == sRetCodeTemp) && (usI <= FLEX_ETK_MAX); usI++) {
                tchEmployeeName[0] = _T('E');
                tchEmployeeName[1] = _T('M');
                tchEmployeeName[2] = _T('P');
                tchEmployeeName[3] = _T('L');
                tchEmployeeName[4] = _T('O');
                tchEmployeeName[5] = _T('Y');
                tchEmployeeName[6] = _T('E');
                tchEmployeeName[7] = _T('E');
                tchEmployeeName[8] = (TCHAR)(usI / 100) + _T('0');
                tchEmployeeName[9] = (TCHAR)((usI / 10) % 10) + _T('0');
                tchEmployeeName[10] = (TCHAR)(usI % 10) + _T('0');
                sRetCodeTemp = EtkAssign(usI, pEtkJob, tchEmployeeName);
            }
       }
    }
    /*========================= TEST END =============================*/

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:    SHORT   SerEtkDelete(ULONG  ulEmployeeNo)
*
*       Input:    ULONG ulEmployeeNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: ETK_SUCCESS
*   Abnormal End: ETK_LOCK
*                 ETK_FILE_NOT_FOUND
*                 ETK_NOT_IN_FILE
*                 ETK_DATA_EXIST
*
**  Description:  Delete Inputed Etk No from index table
*==========================================================================
*/
SHORT   SerEtkDelete(ULONG  ulEmployeeNo)
{
	CLIREQETK       ReqMsgH = {0};
    CLIRESETKTIME   ResMsgH = {0};
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulEmployeeNo  = ulEmployeeNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKDELETE | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETKTIME);

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = EtkDelete(ulEmployeeNo);
    } else {
        sRetCode = ETK_NOT_MASTER;
    }
    if (ETK_SUCCESS == sRetCode) {
        SstUpdateBackUp();
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:    SHORT   SerEtkAllReset(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: ETK_SUCCESS
*   Abnormal End: ETK_FILE_NOT_FOUND
*
**  Description:  Reset All employee records.
*==========================================================================
*/
SHORT  SerEtkAllReset(VOID)
{
	CLIREQETK       ReqMsgH = {0};
    CLIRESETKTIME   ResMsgH = {0};
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKALLRESET | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETKTIME);

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = EtkAllReset();
    } else {
        sRetCode = ETK_NOT_MASTER;
    }
    if (ETK_SUCCESS == sRetCode) {
        SstUpdateBackUp();
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:    SHORT   SerEtkIssuedSet(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: ETK_SUCCESS        
*   Abnormal End: ETK_NOTISSUED      
*                 ETK_FILE_NOT_FOUND 
*
**  Description: Check issued flag. 
*==========================================================================
*/
SHORT   SerEtkIssuedSet(VOID)
{
	CLIREQETK       ReqMsgH = {0};
    CLIRESETKTIME   ResMsgH = {0};
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKISSUEDSET | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETKTIME);

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = EtkIssuedSet();
    } else {
        sRetCode = ETK_NOT_MASTER;
    }
    if (ETK_SUCCESS == sRetCode) {
        SstUpdateBackUp();
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:    SHORT   CliEtkIndJobRead(ULONG   ulEmployeeNo,
*                                           ETK_JOB *pEtkJob,
*                                           UCHAR   *pauchEmployeeName)
*
*       Input:    ulEmployeeNo
*      Output:    *pEtkJob
*                 *pEmployeeName
*       InOut:    Nothing
*
**  Return 
*     Normal End: ETK_SUCCESS
*   Abnormal End: ETK_FILE_NOT_FOUND
*
**  Description:  Read Job Code & Employee Name
*==========================================================================
*/
SHORT   CliEtkIndJobRead(ULONG   ulEmployeeNo,
                         ETK_JOB *pEtkJob,
                         TCHAR   *pauchEmployeeName)
{
    SHORT           sRetCode;
	CLIREQETK       ReqMsgH = {0};
    CLIRESETKTIME   ResMsgH = {0};

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulEmployeeNo  = ulEmployeeNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKINDJOBREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETKTIME);

    if (STUB_SELF == SstReadFAsMaster()) {
        sRetCode = EtkIndJobRead(ulEmployeeNo, pEtkJob, pauchEmployeeName);
    } else {
        sRetCode = CliMsg.sRetCode;

        if (sRetCode == ETK_SUCCESS) {
            memcpy(pEtkJob, ResMsgH.auchETKData, sizeof(ETK_JOB));
			memcpy(pauchEmployeeName, ResMsgH.auchEmployeeName, sizeof(ResMsgH.auchEmployeeName));
        }
    }

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:    SHORT   SerEtkIndJobRead(ULONG  ulEmployeeNo,
*                                           ETK_JOB  *pEtkJob,
*                                           UCHAR    *pauchEmployeeName)
*
*       Input:    ulEmployeeNo
*      Output:    *pEtkJob
*                 *pauchEmployeeName
*       InOut:    Nothing
*
**  Return
*     Normal End: ETK_SUCCESS
*   Abnormal End: ETK_FILE_NOT_FOUND
*
**  Description:  Read all allowance Job Code.
*==========================================================================
*/
SHORT   SerEtkIndJobRead(ULONG  ulEmployeeNo,
                         ETK_JOB  *pEtkJob,
                         TCHAR    *pauchEmployeeName)
{
	return CliEtkIndJobRead(ulEmployeeNo, pEtkJob, pauchEmployeeName);
}

/*
*==========================================================================
**    Synopsis:    SHORT   SerEtkCurIndWrite(ULONG     ulEmployeeNo,
*                                            USHORT    *pusFieldNo,
*                                            ETK_FIELD *pEtkField)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: ETK_SUCCESS        
*   Abnormal End: ETK_NOTISSUED      
*                 ETK_FILE_NOT_FOUND 
*
**  Description: Check issued flag. 
*==========================================================================
*/
SHORT   SerEtkCurIndWrite(ULONG     ulEmployeeNo,
                          USHORT    *pusFieldNo,
                          ETK_FIELD *pEtkField)
{
	CLIREQETK       ReqMsgH = {0};
    CLIRESETKTIME   ResMsgH = {0};
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulEmployeeNo  = ulEmployeeNo;
    ReqMsgH.usFieldNo     = *pusFieldNo;
	NHPOS_ASSERT(sizeof(ETK_FIELD) <= sizeof(ReqMsgH.auchNewJob));
    memcpy(ReqMsgH.auchNewJob, pEtkField, sizeof(ETK_FIELD));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKCURINDWRITE | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETKTIME);

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode =  EtkCurIndWrite(ulEmployeeNo, pusFieldNo, pEtkField);

    } else {
        sRetCode = ETK_NOT_MASTER;
    }
    if (ETK_SUCCESS == sRetCode) {
        SstUpdateBackUp();
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}


/*
*==========================================================================
**    Synopsis:    SHORT   CliEtkStatRead(ULONG   ulEmployeeNo,
*                                         SHORT   *psTimeIOStat,
*                                         USHORT  *pusMaxFieldNo,
*                                         ETK_JOB *pEtkJob);
*
*       Input:    ulEmployeeNo
*      Output:    *pusTimeIOStat
*                 *pusMaxFieldNo
*                 *pEtkJob
*       InOut:    Nothing
*
**  Return 
*     Normal End: ETK_SUCCESS
*   Abnormal End: ETK_FILE_NOT_FOUND
*
**  Description:  Read status & Job Code.
*==========================================================================
*/
SHORT   CliEtkStatRead(ULONG   ulEmployeeNo,
                       SHORT   *psTimeIOStat,
                       USHORT  *pusMaxFieldNo,
                       ETK_JOB *pEtkJob)
{
	SHORT           sRetCode;
    CLIREQETK       ReqMsgH = {0};
    CLIRESETKTIME   ResMsgH = {0};

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulEmployeeNo  = ulEmployeeNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKSTATREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETKTIME);

    if (STUB_SELF == SstReadFAsMaster()) {
        sRetCode = EtkStatRead(ulEmployeeNo, psTimeIOStat, pusMaxFieldNo,  pEtkJob);
    } else {
        sRetCode = CliMsg.sRetCode;
        if (sRetCode == ETK_SUCCESS) {
            memcpy(pEtkJob, ResMsgH.auchETKData, ResMsgH.usDataLen);
            *psTimeIOStat = ResMsgH.sTMStatus;
            *pusMaxFieldNo = ResMsgH.usFieldNo;
        }
    }

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}
/*
*==========================================================================
**    Synopsis:    SHORT CliEtkCurAllIdRead(USHORT usRcvBufferSize, 
*                                   ULONG  *aulRcvBuffer)
*

*
**  Return 
*     Normal End: number of ETK records put into aulRcvBuffer
*   Abnormal End: ETK_FILE_NOT_FOUND
*
**  Description:  Read The entire list of employees
*==========================================================================
*/
SHORT CliEtkCurAllIdRead(USHORT usRcvBufferSize, ULONG  *aulRcvBuffer)
{
	ULONG       *aulRcvBufferLast;
	CLIREQETK   ReqMsgH = {0};
	CLIRESETK   ResMsgH = {0};
 	SHORT		sNumOfIdsRead = 0;

    PifRequestSem(husCliExeNet);
	
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKCURALLIDRD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;
    CliMsg.pauchResData = ( UCHAR * )aulRcvBuffer;
    CliMsg.usResLen     = usRcvBufferSize;

	aulRcvBufferLast = aulRcvBuffer + usRcvBufferSize / sizeof(ULONG);

    if (STUB_SELF == CstSendMaster()) {
        sNumOfIdsRead = EtkCurAllIdRead(usRcvBufferSize, aulRcvBuffer);
	}
	else {
		if (CliMsg.sRetCode > 0)
		{
			// There were some ETK records available and we have read some of
			// them.  Now transfer to the caller's buffer area and then continue
			// reading ETK records until we are done.
			while(CliMsg.sRetCode > 0)
			{
				int  iBufferLeft = (aulRcvBufferLast - aulRcvBuffer);
				
				if (CliMsg.sRetCode > iBufferLeft) {
					CliMsg.sRetCode = iBufferLeft;
				}

				memcpy(aulRcvBuffer, ResMsgH.auchETKData, (CliMsg.sRetCode * sizeof(ULONG)));
				sNumOfIdsRead += CliMsg.sRetCode;
				aulRcvBuffer += CliMsg.sRetCode;
				ReqMsgH.usFieldNo += CliMsg.sRetCode;
				if (aulRcvBuffer < aulRcvBufferLast)
				{
					CstSendMaster();
				}
				else {
					CliMsg.sRetCode = 0;
				}
			}
		}
		else
		{
			sNumOfIdsRead = ETK_FILE_NOT_FOUND;
		}
	}

    PifReleaseSem(husCliExeNet);
    return sNumOfIdsRead;    
}
/*
*==========================================================================
**    Synopsis:  SHORT   STUBENTRY CliEtkCurIndRead(ULONG     ulEmployeeNo,
*                                                   USHORT    FAR  *pusFieldNo,
*                                                   ETK_FIELD FAR  *pEtkField,
*                                                   ETK_TIME  FAR  *pEtkTime)
*
*       Input:    ULONG     ulEmployeeNo
*      InOut :    USHORT    FAR *pusFieldNo    
*      Output:    ETK_FIELD FAR *pEtkField
*                 ETK_TIME  FAR *pEtkTime
*
**  Return    :
*            Normal End:    ETK_SUCCESS          All field is read.
*                           ETK_CONTINUE         This is still far from complete.
*          Abnormal End:    ETK_FILE_NOT_FOUND   file is not found.
*                           ETK_NOT_IN_FILE      This employee no is not in file.
*                           ETK_NOT_IN_FIELD     Data is not on and after this field.
*
**  Description: Read Individual ETK record Field in current buffer
*                by usEmployeeNo.
*==========================================================================
*/
SHORT  CliEtkCurIndRead(ULONG     ulEmployeeNo,
                                    USHORT     FAR *pusFieldNo,
                                    ETK_FIELD  FAR *pEtkField,
                                    ETK_TIME   FAR *pEtkTime)
{
	CLIREQETK   ReqMsgH = {0};
    CLIRESETK   ResMsgH = {0};
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulEmployeeNo  = ulEmployeeNo;
    ReqMsgH.usFieldNo     = *pusFieldNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKCURINDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;

    if (STUB_SELF == CstSendMaster()) {
        CliMsg.sRetCode = EtkCurIndRead(ulEmployeeNo, pusFieldNo, pEtkField, pEtkTime);
	}
	else {
		memcpy(&ResMsgH, CliRcvBuf.auchData, CliMsg.usResMsgHLen);

		memcpy(pEtkField, ResMsgH.auchETKData, sizeof(ETK_FIELD));
		memcpy(pEtkTime, &ResMsgH.auchETKData[sizeof(ETK_FIELD)], sizeof(ETK_TIME));
		*pusFieldNo = ResMsgH.usFieldNo;
	}

	sRetCode = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

//<=========================BEGIN SERVER FUNCTIONS============================>//
/*
*==========================================================================
**    Synopsis:    SHORT   SerEtkStatRead(ULONG   ulEmployeeNo,
*                                         SHORT   *psTimeIOStat,
*                                         USHORT  *pusMaxFieldNo,
*                                         ETK_JOB *pEtkJob);
*
*       Input:    ulEmployeeNo
*      Output:    *pusTimeIOStat
*                 *pusMaxFieldNo
*                 *pEtkJob
*       InOut:    Nothing
*
**  Return
*     Normal End: ETK_SUCCESS
*   Abnormal End: ETK_FILE_NOT_FOUND
*
**  Description:  Read status & Job Code.
*==========================================================================
*/
SHORT   SerEtkStatRead(ULONG   ulEmployeeNo,
                       SHORT   *psTimeIOStat,
                       USHORT  *pusMaxFieldNo,
                       ETK_JOB *pEtkJob)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return EtkStatRead(ulEmployeeNo, psTimeIOStat, pusMaxFieldNo,  pEtkJob);
    } 
    return ETK_NOT_MASTER;
}

/*
*==========================================================================
**    Synopsis:   SHORT  SerEtkCurAllIdRead(USHORT usRcvBufferSize,
*                                           ULONG  *aulRcvBuffer)
*       Input:    USHORT usRcvBufferSize
*       InOut:    ULONG  *auchRcvBuffer
*
**  Return   :
*           Number of Etk          < 0
*           NOT_IN_FILE            = 0
*           ETK_FILE_NOT_FOUND     > 0
*
**  Description: Read all Etk No. in this file.
*==========================================================================
*/
SHORT  SerEtkCurAllIdRead(USHORT usRcvBufferSize, ULONG  *aulRcvBuffer)
{

	//US CUSTOMS SCER For Release 2.X JHHJ 7-25-05
	//If we are not the master, then send a request to the master for the information required
	return CliEtkCurAllIdRead(usRcvBufferSize, aulRcvBuffer);
}
                      
/*
*==========================================================================
**    Synopsis:   SHORT  SerEtkSavAllIdRead(USHORT usRcvBufferSize,
*                                           ULONG  *aulRcvBuffer)
*       Input:    USHORT usRcvBufferSize
*       InOut:    ULONG  *auchRcvBuffer
*
**  Return    :
*           Number of Etk          < 0
*           NOT_IN_FILE            = 0
*           ETK_FILE_NOT_FOUND     > 0
*
**  Description: Read all Etk No. in this file.
*==========================================================================
*/
SHORT  SerEtkSavAllIdRead(USHORT usRcvBufferSize, ULONG  *aulRcvBuffer)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return EtkSavAllIdRead(usRcvBufferSize, aulRcvBuffer);
    } 
    return ETK_NOT_MASTER;
}

/*
*==========================================================================
**    Synopsis:    SHORT   SerEtkCurIndRead(ULONG      ulEmployeeNo,
*                                           USHORT     *pusFieldNo,
*                                           ETK_FIELD  *pEtkField,
*                                           ETK_TIME   *pEtkTime)
*
*       Input:    ULONG     ulEmployeeNo
*      InOut :    USHORT    *pusFieldNo    
*      Output:    ETK_FIELD *pEtkField
*                 ETK_TIME  *pEtkTime
*
**  Return    :
*     Normal End: ETK_SUCCESS          All field is read.
*                 ETK_CONTINUE         This is still far from complete.
*   Abnormal End: ETK_FILE_NOT_FOUND   file is not found.
*                 ETK_NOT_IN_FILE      This employee no is not in file.
*                 ETK_NOT_IN_FIELD     Data is not on and after this field.
*
**  Description: Read Individual ETK record Field in current buffer
*                by ulEmployeeNo.
*==========================================================================
*/
SHORT   SerEtkCurIndRead(ULONG      ulEmployeeNo,
                         USHORT     *pusFieldNo,
                         ETK_FIELD  *pEtkField,
                         ETK_TIME   *pEtkTime)
{
    //US CUSTOMS SCER For Release 2.X JHHJ 7-25-05
	//If we are not the master, then send a request to the master for the information required
	return CliEtkCurIndRead(ulEmployeeNo, pusFieldNo, pEtkField, pEtkTime);
}

/*
*==========================================================================
**    Synopsis:    SHORT   SerEtkSavIndRead(ULONG      ulEmployeeNo,
*                                           USHORT     *pusFieldNo,
*                                           ETK_FIELD  *pEtkField,
*                                           ETK_TIME   *pEtkTime)
*
*       Input:    ulEmployeeNo
*      InOut :    *pusFieldNo    
*      Output:    *pEtkField
*                 *pEtkTime
*
**  Return    :
*     Normal End: ETK_SUCCESS          All field is read.
*                 ETK_CONTINUE         This is still far from complete.
*   Abnormal End: ETK_FILE_NOT_FOUND   file is not found.
*                 ETK_NOT_IN_FILE      This employee no is not in file.
*                 ETK_NOT_IN_FIELD     Data is not on and after this field.
*
**  Description: Read Individual ETK record Field in saved buffer
*                by ulEmployeeNo.
*==========================================================================
*/
SHORT   SerEtkSavIndRead(ULONG      ulEmployeeNo,
                         USHORT     *pusFieldNo,
                         ETK_FIELD  *pEtkField,
                         ETK_TIME   *pEtkTime)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return EtkSavIndRead(ulEmployeeNo, pusFieldNo, pEtkField, pEtkTime);
    } 
    return ETK_NOT_MASTER;
}

/*
*==========================================================================
**    Synopsis:    SHORT   SerEtkAllLock(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: ETK_SUCCESS
*   Abnormal End: ETK_LOCK
*                 ETK_FILE_NOT_FOUND
*
**  Description: Lock All Etk. 
*==========================================================================
*/
SHORT   SerEtkAllLock(VOID)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return EtkAllLock();
    } 
    return ETK_NOT_MASTER;
}

/*
*==========================================================================
**    Synopsis:    VOID   SerEtkAllUnLock(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:  Unlock All Etk
*==========================================================================
*/
VOID   SerEtkAllUnLock(VOID)
{
    if (STUB_SELF == SstReadAsMaster()) {
        EtkAllUnLock();
    } 
}

/*
*==========================================================================
**    Synopsis:    SHORT   SerEtkIssuedCheck(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: ETK_ISSUED
*                 ETK_NOTISSUED
*   Abnormal End: ETK_FILE_NOT_FOUND
*
**  Description: Check issued flag. 
*==========================================================================
*/
SHORT   SerEtkIssuedCheck(VOID)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return EtkIssuedCheck();
    } 
    return ETK_NOT_MASTER;
}

/*
*==========================================================================
**    Synopsis:    SHORT   SerEtkAllDataCheck(UCHAR uchMinor)
*
*       Input:    uchMinor  - Minor class (CLASS_TTLCURDAY)
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: ETK_SUCCESS
*   Abnormal End: ETK_FILE_NOT_FOUND
*                 ETK_DATA_EXIST
*
**  Description: Check data etred or not.
*==========================================================================
*/
SHORT   SerEtkAllDataCheck(UCHAR uchMinor)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return EtkAllDataCheck(uchMinor);    
    } 
    return ETK_NOT_MASTER;
}

/*===== END OF SOURCE =====*/

