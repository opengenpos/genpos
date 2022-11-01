/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server STUB, Source File of ETK Function                   
* Category    : Client/Server STUB, US Hospitality Mode
* Program Name: PCSTBETK.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*       SerEtkAssign()         Assign     
*       SerEtkDelete()         Delete     
*       SerEtkAllReset()       Reset      
*       SerEtkIssuedSet()      Issued set 
*
*       SerEtkIndJobRead()     Ind. Job code read     
*       SerEtkCurAllIdRead()   Read all current ID    
*       SerEtkSavAllIdRead()   Read all saved   ID    
*       SerEtkCurIndRead()     Read current Ind. read 
*       SerEtkSavIndRead()     Read saved   Ind. read 
*       SerEtkAllLock()        All lock               
*       SerEtkAllUnLock()      All unlock             
*       SerEtkIssuedCheck()    Check issued flag       
*
*       SerEtkStatRead()       Read status and Job code       
*       SerEtkCurIndWrite()    Maint Etk file by field      
*
*       SerEtkCreateTotalIndex Create total file index (PEP unique)       
*       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* JuL-15-93:H.Yamaguchi: initial                                   
* Oct-06-93:H.Yamaguchi: Adds SerEtkStatRead/SerCurIndWrite                                   
* Mar-14-96:M.Suzuki   : Adds R3.1                                   
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
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include    <pc2170.h>
#endif
#include    <r20_pc2172.h>

#include    <r20_pif.h>
#include    <memory.h>
#include    "csstubin.h"

#define		_fmemcpy	memcpy
#include "mypifdefs.h"

/*
*==========================================================================
**    Synopsis:    SHORT   STUBENTRY  SerEtkAssign(ULONG ulEmployeeNo,
*                                                  ETK_JOB  *pEtkJob)
*                                                  UCHAR    *pauchEmployeeName );
*
*       Input:      ulEmployeeNo    -   Employee Number to be assign
*                   pEtkJob         -   Job Code
*                   pauchEmployeeName -   Employee Name   
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_LOCK
*                           ETK_FILE_NOT_FOUND
*                           ETK_FULL
*
**  Description: Add, Regist Inputed Etk.
*==========================================================================
*/
SHORT  STUBENTRY  SerEtkAssign(ULONG   ulEmployeeNo,
                               ETK_JOB  FAR *pEtkJob,
                               LPWSTR pauchEmployeeName )
{
    CLIREQETK   ReqMsgH;
    CLIRESETK   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQETK));
    memset(&ResMsgH, 0, sizeof(CLIRESETK));

    ReqMsgH.ulEmployeeNo  = ulEmployeeNo;
	memcpy(ReqMsgH.auchNewJob, pEtkJob, sizeof(ETK_JOB));

    /* ===== Add Employee Name (Release3.1) BEGIN ===== */
    wcsncpy(ReqMsgH.auchEmployeeName, pauchEmployeeName, CLI_EMPLOYEE_NAME);
    /* ===== Add Employee Name (Release3.1) END ===== */

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKASSIGN;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:    SHORT   STUBENTRY SerEtkDelete(ULONG ulEmployeeNo)
*
*       Input:    ULONG ulEtkNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_LOCK
*                           ETK_FILE_NOT_FOUND
*                           ETK_NOT_IN_FILE
*                           ETK_DATA_EXIST
**  Description:
*                Delete Inputed Etk No from index table
*==========================================================================
*/
SHORT   STUBENTRY  SerEtkDelete(ULONG ulEmployeeNo)
{
    CLIREQETK   ReqMsgH;
    CLIRESETK   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQETK));
    memset(&ResMsgH, 0, sizeof(CLIRESETK));

    ReqMsgH.ulEmployeeNo  = ulEmployeeNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKDELETE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:    SHORT   STUBENTRY  SerEtkAllReset(VOID)
*
*       Input:    USHORT    usEtkNo
*      Output:
*       InOut:
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_FILE_NOT_FOUND
*
**  Description:
*                Reset All employee records.
*==========================================================================
*/
SHORT  STUBENTRY  SerEtkAllReset(VOID)
{
    CLIREQETK   ReqMsgH;
    CLIRESETK   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQETK));
    memset(&ResMsgH, 0, sizeof(CLIRESETK));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKALLRESET;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:    SHORT   STUBENTRY  SerEtkIssuedSet(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS        
*          Abnormal End:    ETK_NOTISSUED      
*                           ETK_FILE_NOT_FOUND 
*
**  Description: Check issued flag. 
*==========================================================================
*/
SHORT   STUBENTRY  SerEtkIssuedSet(VOID)
{
    CLIREQETK   ReqMsgH;
    CLIRESETK   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQETK));
    memset(&ResMsgH, 0, sizeof(CLIRESETK));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKISSUEDSET;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:    SHORT   STUBENTRY  SerEtkIndJobRead(ULONG ulEmployeeNo,
*                                                      ETK_JOB FAR  *pEtkJob)
*                                                      UCHAR  FAR *apuchEmployeeName);
*
*       Input:    USHORT    usEmployeeNo
*                 UHARA FAR *puchEmployeeName
*      Output:    ETK_JOB   FAR *pEtkJob
*       InOut:    Nothing
*
**  Return   Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_FILE_NOT_FOUND
*
**  Description:  Read all allowance Job Code.
*==========================================================================
*/
SHORT  STUBENTRY  SerEtkIndJobRead(ULONG ulEmployeeNo,
                                   ETK_JOB FAR *pEtkJob,
                                   LPWSTR pauchEmployeeName)
{
    CLIREQETK   ReqMsgH;
    CLIRESETK   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQETK));
    memset(&ResMsgH, 0, sizeof(CLIRESETK));

    ReqMsgH.ulEmployeeNo  = ulEmployeeNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKINDJOBREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    if (ETK_SUCCESS == CliMsg.sRetCode) {
        memcpy(pEtkJob, ResMsgH.auchETKData, ResMsgH.usDataLen);
        wcsncpy(pauchEmployeeName, ResMsgH.auchEmployeeName, CLI_EMPLOYEE_NAME);
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:   SHORT  STUBENTRY SerEtkCurAllIdRead(USHORT usRcvBufferSize,
*                                                     ULONG  FAR *aulRcvBuffer)
*       Input:    USHORT  usRcvBufferSize
*       InOut:    ULONG  FAR *aulRcvBuffer
*
**  Return    :
*           Number of Etk          < 0
*           NOT_IN_FILE            = 0
*           ETK_FILE_NOT_FOUND     > 0
*
**  Description:
*                Read all Etk No. in this file.
*==========================================================================
*/
SHORT  STUBENTRY SerEtkCurAllIdRead(USHORT usRcvBufferSize, 
                                    ULONG  FAR *aulRcvBuffer)
{
    CLIREQETK   ReqMsgH;
    CLIRESETK   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQETK));
    memset(&ResMsgH, 0, sizeof(CLIRESETK));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKCURALLIDRD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;
    CliMsg.pauchResData = ( UCHAR * )aulRcvBuffer;
    CliMsg.usResLen     = usRcvBufferSize;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}
                      
/*
*==========================================================================
**    Synopsis:   SHORT  STUBENTRY SerEtkSavAllIdRead(USHORT usRcvBufferSize,
*                                                     ULONG  FAR *aulRcvBuffer)
*       Input:    USHORT usRcvBufferSize
*       InOut:    ULONG  *aulRcvBuffer
*
**  Return    :
*           Number of Etk          < 0
*           NOT_IN_FILE            = 0
*           ETK_FILE_NOT_FOUND     > 0
*
**  Description: Read all Etk No. in this file.
*==========================================================================
*/
SHORT  STUBENTRY SerEtkSavAllIdRead(USHORT usRcvBufferSize, 
                                    ULONG  FAR *aulRcvBuffer)
{
    CLIREQETK   ReqMsgH;
    CLIRESETK   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQETK));
    memset(&ResMsgH, 0, sizeof(CLIRESETK));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKSAVALLIDRD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;
    CliMsg.pauchResData = ( UCHAR * )aulRcvBuffer;
    CliMsg.usResLen     = usRcvBufferSize;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:  SHORT   STUBENTRY SerEtkCurIndRead(ULONG     ulEmployeeNo,
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
SHORT   STUBENTRY  SerEtkCurIndRead(ULONG     ulEmployeeNo,
                                    USHORT     FAR *pusFieldNo,
                                    ETK_FIELD  FAR *pEtkField,
                                    ETK_TIME   FAR *pEtkTime)
{
    CLIREQETK   ReqMsgH;
    CLIRESETK   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQETK));
    memset(&ResMsgH, 0, sizeof(CLIRESETK));

    ReqMsgH.ulEmployeeNo  = ulEmployeeNo;
    ReqMsgH.usFieldNo     = *pusFieldNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKCURINDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    if ( ETK_CONTINUE ==  CliMsg.sRetCode) {
        memcpy(&ResMsgH, CliRcvBuf.auchData, CliMsg.usResMsgHLen);
    }
    if ( ( ETK_SUCCESS  ==  CliMsg.sRetCode) || 
         ( ETK_CONTINUE ==  CliMsg.sRetCode)    )  {
        memcpy(pEtkField, ResMsgH.auchETKData, sizeof(ETK_FIELD));
        memcpy(pEtkTime,  
               &ResMsgH.auchETKData[sizeof(ETK_FIELD)], 
               sizeof(ETK_TIME));
        *pusFieldNo = ResMsgH.usFieldNo;
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:  SHORT  STUBENTRY  SerEtkSavIndRead(ULONG      ulEmployeeNo,
*                                                   USHORT     FAR *pusFieldNo,
*                                                   ETK_FIELD  FAR *pEtkField,
*                                                   ETK_TIME   FAR *pEtkTime)
*
*       Input:    ULONG     ulEmployeeNo
*      InOut :    USHORT    *pusFieldNo    
*      Output:    ETK_FIELD *pEtkField
*                 ETK_TIME  *pEtkTime
*
**  Return    :
*            Normal End:    ETK_SUCCESS          All field is read.
*                           ETK_CONTINUE         This is still far from complete.
*          Abnormal End:    ETK_FILE_NOT_FOUND   file is not found.
*                           ETK_NOT_IN_FILE      This employee no is not in file.
*                           ETK_NOT_IN_FIELD     Data is not on and after this field.
*
**  Description: Read Individual ETK record Field in saved buffer
*                by usEmployeeNo.
*==========================================================================
*/
SHORT  STUBENTRY  SerEtkSavIndRead(ULONG      ulEmployeeNo,
                                   USHORT     FAR *pusFieldNo,
                                   ETK_FIELD  FAR *pEtkField,
                                   ETK_TIME   FAR *pEtkTime)
{
    CLIREQETK   ReqMsgH;
    CLIRESETK   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQETK));
    memset(&ResMsgH, 0, sizeof(CLIRESETK));

    ReqMsgH.ulEmployeeNo  = ulEmployeeNo;
    ReqMsgH.usFieldNo     = *pusFieldNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKSAVINDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    if ( ETK_CONTINUE ==  CliMsg.sRetCode) {
        memcpy(&ResMsgH, CliRcvBuf.auchData, CliMsg.usResMsgHLen);
    }
    if ( ( ETK_SUCCESS  ==  CliMsg.sRetCode) || 
         ( ETK_CONTINUE ==  CliMsg.sRetCode)    )  {
        memcpy(pEtkField, ResMsgH.auchETKData, sizeof(ETK_FIELD));
        memcpy(pEtkTime,  
               &ResMsgH.auchETKData[sizeof(ETK_FIELD)], 
               sizeof(ETK_TIME));
        *pusFieldNo = ResMsgH.usFieldNo;
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:   SHORT  STUBENTRY  SerEtkAllLock(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_LOCK
*                           ETK_FILE_NOT_FOUND
*
**  Description: Lock All Etk. 
*==========================================================================
*/
SHORT   STUBENTRY  SerEtkAllLock(VOID)
{
    CLIREQETK   ReqMsgH;
    CLIRESETK   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQETK));
    memset(&ResMsgH, 0, sizeof(CLIRESETK));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKALLLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:   VOID  STUBENTRY  SerEtkAllUnLock(VOID)
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
VOID   STUBENTRY  SerEtkAllUnLock(VOID)
{
    CLIREQETK   ReqMsgH;
    CLIRESETK   ResMsgH;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQETK));
    memset(&ResMsgH, 0, sizeof(CLIRESETK));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKALLUNLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;

    CstSendMaster();
    PifReleaseSem(husCliExeNet);
    return;
}

/*
*==========================================================================
**    Synopsis:   SHORT   STUBENTRY SerEtkIssuedCheck(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_ISSUED
*                           ETK_NOTISSUED
*          Abnormal End:    ETK_FILE_NOT_FOUND
*
**  Description: Check issued flag. 
*==========================================================================
*/
SHORT   STUBENTRY  SerEtkIssuedCheck(VOID)
{
    CLIREQETK   ReqMsgH;
    CLIRESETK   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQETK));
    memset(&ResMsgH, 0, sizeof(CLIRESETK));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKISSUEDCHK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:    SHORT   STUBENTRY SerEtkCurIndWrite(ULONG     ulEmployeeNo,
*                                                      USHORT    *pusFieldNo,
*                                                      ETK_FIELD *pEtkField)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS        
*          Abnormal End:    ETK_NOTISSUED      
*                           ETK_FILE_NOT_FOUND 
*
**  Description: Check issued flag. 
*==========================================================================
*/
SHORT   STUBENTRY  SerEtkCurIndWrite(ULONG     ulEmployeeNo,
                                     USHORT    *pusFieldNo,
                                     ETK_FIELD *pEtkField)
{
    CLIREQETK       ReqMsgH;
    CLIRESETK       ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQETK));
    memset(&ResMsgH, 0, sizeof(CLIRESETK));

    ReqMsgH.ulEmployeeNo  = ulEmployeeNo;
    ReqMsgH.usFieldNo     = *pusFieldNo;
	memcpy(ReqMsgH.auchNewJob, pEtkField, sizeof(ETK_FIELD));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKCURINDWRITE ;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;
    
    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    if (ETK_SUCCESS == CliMsg.sRetCode) {
        *pusFieldNo = ResMsgH.usFieldNo ;
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:    SHORT   STUBENTRY  SerEtkStatRead(ULONG   ulEmployeeNo,
*                                                    SHORT   *psTimeIOStat,
*                                                    USHORT  *pusMaxFieldNo,
*                                                    ETK_JOB *pEtkJob);
*
*       Input:    ULONG     ulEmployeeNo
*      Output:    USHORT    *pusTimeIOStat
*                 USHORT    *pusMaxFieldNo
*                 ETK_JOB   *pEtkJob
*       InOut:    Nothing
*
**  Return   Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_FILE_NOT_FOUND
*
**  Description:  Read status & Job Code.
*==========================================================================
*/
SHORT  STUBENTRY   SerEtkStatRead(ULONG   ulEmployeeNo,
                                  SHORT   *psTimeIOStat,
                                  USHORT  *pusMaxFieldNo,
                                  ETK_JOB *pEtkJob)
{
    CLIREQETK       ReqMsgH;
    CLIRESETK       ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQETK));
    memset(&ResMsgH, 0, sizeof(CLIRESETK));

    ReqMsgH.ulEmployeeNo  = ulEmployeeNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKSTATREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    if (ETK_SUCCESS == CliMsg.sRetCode) {
        memcpy(pEtkJob, ResMsgH.auchETKData, ResMsgH.usDataLen);
        *psTimeIOStat = ResMsgH.sTMStatus;
        *pusMaxFieldNo = ResMsgH.usFieldNo;
    }

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
**    Synopsis:    SHORT   STUBENTRY  SerEtkCreateTotalIndex(VOID)
*
*       Input:    nothing
*      Output:    nothing
*       InOut:    Nothing
*
**  Return   Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_FILE_NOT_FOUND
*
**  Description:  Create total file index.
*==========================================================================
*/
SHORT  STUBENTRY   SerEtkCreateTotalIndex(VOID)
{
    CLIREQETK       ReqMsgH;
    CLIRESETK       ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQETK));
    memset(&ResMsgH, 0, sizeof(CLIRESETK));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCETKCREATTLIDX;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQETK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESETK);
    CliMsg.sRetCode     = ETK_SUCCESS;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*===== END OF SOURCE =====*/

