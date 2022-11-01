/*mhmh
**********************************************************************
**                                                                  **
**  Title:      Remote File Control                                 **
**                                                                  **
**  File Name:  STUBRMT.C                                           **
**                                                                  **
**  Categoly:   PC STUB                                             **
**                                                                  **
**  Abstruct:   This module provides the functions of the remote    **
**              file system.                                        **
**              This module provides the following functions:       **
**                                                                  **
**                  RmtOpenFile():      Open a file on the 2170     **
**                  RmtReadFile():      Read a file on the 2170     **
**                  RmtWriteFile():     Write a file on the 2170    **
**                  RmtSeekFile():      Seek a file on the 2170     **
**                  RmtCloseFile():     Close a file on the 2170    **
**                  RmtDeleteFile():    Delete a file on the 2170   **
**                  RmtErrCtl():        Control error value         **
**                                                                  **
**  Compiler's options: /c /Aflw /W4 /Zp /Ot /G2s                   **
**                                                                  **
**********************************************************************
**                                                                  **
**  Modification History:                                           **
**                                                                  **
**      Ver.      Date        Name      Description                 **
**      1.00    Sep/24/92   T.Kojima    Initial release             **
**      2.00    May/19/93   T.Kojima    Add to clear "CliMsg"       **
**                                                                  **
**********************************************************************
mhmh*/

/*******************\
|   Include Files   |
\*******************/
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include    <pc2170.h>
#endif
#include    <pc2172.h>

#include    <pif.h>
#include    <csstubin.h>
#include    <rmt.h>
#include    <string.h>
#include "mypifdefs.h"

/***********************\
|   Static Variables    |
\***********************/
ULONG   aulPosition[RMT_NUM_FILETBL];

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT STUBENTRY RmtCloseFile(USHORT usFile)         **
**              usFile:     file handle                             **
**                                                                  **
**  Return:     see the function of "RmtErrCtl()"                   **
**                                                                  **
** Description: This function closes the file on 2170.              **
**                                                                  **
**********************************************************************
fhfh*/
SHORT STUBENTRY RmtCloseFile(USHORT usFile)
{
    RMTREQCLOSE     ReqMsg;
    RMTRESCLOSE     ResMsg;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    /* set the request and response message */
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    ReqMsg.usFile       = usFile;
    CliMsg.usFunCode    = CLI_FCRMTCLOSEFILE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsg;
    CliMsg.usReqMsgHLen = sizeof(RMTREQCLOSE);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsg;
    CliMsg.usResMsgHLen = sizeof(RMTRESCLOSE);

    /* send 2170    */
    CstSendMaster();
	sRetCode = RmtErrCtl(CliMsg.sRetCode, ResMsg.sReturn);

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID STUBENTRY RmtReloadLayout()			        **
**                                                                  **
**  Return:     NONE							                    **
**                                                                  **
** Description: This function reloads the layout	                **
**                                                                  **
**********************************************************************
fhfh*/
VOID STUBENTRY RmtReloadLayout(VOID)
{
    RMTREQCLOSE     ReqMsg;
    RMTRESCLOSE     ResMsg;

    PifRequestSem(husCliExeNet);

    /* set the request and response message */
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCRMTRELOADLAY;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsg;
    CliMsg.usReqMsgHLen = sizeof(RMTREQCLOSE);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsg;
    CliMsg.usResMsgHLen = sizeof(RMTRESCLOSE);

    /* send 2170    */
    CstSendMaster();

    PifReleaseSem(husCliExeNet);
}

/*fhfh
******************************************************************************
**                                                                          **
**  Synopsis:   SHORT STUBENTRY RmtDeleteFile(CONST CHAR FAR *paszFileName) **
**              paszFileName:   the pointer of file name                    **
**                                                                          **
**  Return:     see the function of "RmtErrCtl()"                           **
**                                                                          **
** Description: This function deletes the file on 2170.                     **
**                                                                          **
******************************************************************************
fhfh*/
SHORT STUBENTRY RmtDeleteFile(CONST CHAR FAR *paszFileName)
{
    RMTREQDELETE    ReqMsg;
    RMTRESDELETE    ResMsg;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    /* set the request and response message */
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    strncpy(ReqMsg.aszFileName, paszFileName, RMT_FILENAME_SIZE);
    CliMsg.usFunCode    = CLI_FCRMTDELETEFILE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsg;
    CliMsg.usReqMsgHLen = sizeof(RMTREQDELETE);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsg;
    CliMsg.usResMsgHLen = sizeof(RMTRESDELETE);

    /* send 2170    */
    CstSendMaster();
	sRetCode = RmtErrCtl(CliMsg.sRetCode, ResMsg.sReturn);

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}
/*fhfh
******************************************************************************
**                                                                          **
**  Synopsis:   SHORT STUBENTRY RmtGetFileNames(WCHAR *paszFileNames, WCHAR *paszMode) **
**              paszFileName:   the pointer of file name                    **
**                                                                          **
**  Return:     see the function of "RmtErrCtl()"                           **
**                                                                          **
** Description: This function deletes the file on 2170.                     **
**                                                                          **
******************************************************************************
fhfh*/
SHORT STUBENTRY RmtGetFileNames(WCHAR *paszFileNames, WCHAR *paszMode, WCHAR *paszExtension)
{
    RMTREQDBGNAME    ReqMsg;
    RMTRESDBGNAME    ResMsg;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    /* set the request and response message */
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCRMTDBGFILENM;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsg;
    CliMsg.usReqMsgHLen = sizeof(RMTREQDBGNAME);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsg;
    CliMsg.usResMsgHLen = sizeof(RMTRESDBGNAME);

    /* send 2170    */
    CstSendMaster();
	sRetCode = RmtErrCtl(CliMsg.sRetCode, ResMsg.sReturn);

	if(sRetCode)
	{
		memcpy(paszFileNames, ResMsg.aszFileName, sizeof(ResMsg.aszFileName));
	}

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}
/*fhfh
******************************************************************************
**                                                                          **
**  Synopsis:   SHORT STUBENTRY RmtOpenFile(CONST CHAR FAR *paszFileName,   **
**                                          CONST CHAR FAR *paszMode)       **
**              paszFileName:   the pointer of the file name                **
**              paszMode:       the pointer of the open mode                **
**                                                                          **
**  Return:     see the function of "RmtErrCtl()"                           **
**                                                                          **
** Description: This function opens the file or creates a file              **
**              on 2170.                                                    **
**                                                                          **
******************************************************************************
fhfh*/
SHORT STUBENTRY RmtOpenFile(CONST CHAR FAR *paszFileName,
                            CONST CHAR FAR *paszMode)
{
    RMTREQOPEN      ReqMsg;
    RMTRESOPEN      ResMsg;
    SHORT           sResult;

    PifRequestSem(husCliExeNet);

    /* set the request and response message */
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    strncpy(ReqMsg.aszFileName, paszFileName, RMT_FILENAME_SIZE);
    strncpy(ReqMsg.aszMode, paszMode, RMT_MODE_SIZE);
    CliMsg.usFunCode    = CLI_FCRMTOPENFILE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsg;
    CliMsg.usReqMsgHLen = sizeof(RMTREQOPEN);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsg;
    CliMsg.usResMsgHLen = sizeof(RMTRESOPEN);

    /* send 2170    */
    CstSendMaster();

    /* check the return value   */
    if ((sResult = RmtErrCtl(CliMsg.sRetCode, ResMsg.sReturn)) >= 0) {
        aulPosition[sResult] = 0L;
    }

    PifReleaseSem(husCliExeNet);
    return(sResult);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT STUBENTRY RmtReadFile(USHORT usFile,          **
**                                          VOID FAR *pBuffer,      **
**                                          USHORT usBytes)         **
**              usFile:     file handle                             **
**              pBuffer:    the pointer of the read buffer          **
**              usBytes:    the size of the read data               **
**                                                                  **
**  Return:     the length of the actual read data, or see the      **
**              function of "RmtErrCtl()"                           **
**                                                                  **
** Description: This function reads the file on 2170.               **
**                                                                  **
**********************************************************************
fhfh*/
SHORT STUBENTRY RmtReadFile(USHORT usFile, VOID FAR *pBuffer, USHORT usBytes)
{
    RMTREQREAD      ReqMsg;
    RMTRESREAD      ResMsg;
    UCHAR           *puchPosition;
    SHORT           sBytes, sReadBytes, sCurrentBytes, sResult;
    ULONG           ulCurrentPos;

    /* check the read data size */
    if (usBytes == 0 || usBytes > RMT_MAXBYTES_SIZE) {
        return(RMT_ERROR_INVALID_BYTES);
    }

    PifRequestSem(husCliExeNet);

    /* set the request and response message */
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    ReqMsg.usFile       = usFile;
    CliMsg.usFunCode    = CLI_FCRMTREADFILE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsg;
    CliMsg.usReqMsgHLen = sizeof(RMTREQREAD);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsg;
    CliMsg.usResMsgHLen = sizeof(RMTRESREAD);
    
    /* initial  */
    puchPosition  = (UCHAR *)pBuffer;
    sBytes        = (SHORT)usBytes;
    sCurrentBytes = 0;
    ulCurrentPos  = aulPosition[usFile];

    while (sBytes > 0) {

        /* check the read data size */
        if (sBytes > RMT_MSGDATA_SIZE)  sReadBytes = RMT_MSGDATA_SIZE;
        else                            sReadBytes = sBytes;

        /* set the length of the read data  */
        ReqMsg.usBytes    = (USHORT)sReadBytes;
        ReqMsg.ulPosition = ulCurrentPos;

        /* send 2170    */
        CstSendMaster();

        /* error ocuurs */
        if ((sResult = RmtErrCtl(CliMsg.sRetCode, ResMsg.sReturn)) < 0) {
		    PifReleaseSem(husCliExeNet);
            return(sResult);
        }

        if (sResult > sReadBytes || ResMsg.ulPosition != ReqMsg.ulPosition) {
		    PifReleaseSem(husCliExeNet);
            return(RMT_ERROR_COM_ERRORS);

        /* end file */
        } else if (sResult < sReadBytes) {
            memcpy(puchPosition, ResMsg.auchData, sResult);
            aulPosition[usFile] = ulCurrentPos + (ULONG)sResult;
		    PifReleaseSem(husCliExeNet);
            return((SHORT)(sResult + sCurrentBytes));

        /* continue */
        } else /* if (sResult == sReadBytes) */ {
            memcpy(puchPosition, ResMsg.auchData, sResult);
            sCurrentBytes += sResult;
            puchPosition  += sResult;
            sBytes        -= sResult;
            ulCurrentPos  += (ULONG)sResult;
        }
        
        /* loop */
    }

    aulPosition[usFile] = ulCurrentPos;
    PifReleaseSem(husCliExeNet);
    return(sCurrentBytes);
}
    
/*fhfh
******************************************************************************
**                                                                          **
**  Synopsis:   SHORT STUBENTRY RmtSeekFile(USHORT usFile,                  **
**                                          ULONG ulPosition,               **
**                                          ULONG FAR *pulActualPosition)   **
**              usFile:             file handle                             **
**              ulPosition:         the size of the movable position        **
**              pulActualPosition:  the pointer of the real position        **
**                                                                          **
**  Return:     see the function of "RmtErrCtl()"                           **
**                                                                          **
** Description: This function seeks the file on 2170.                       **
**                                                                          **
******************************************************************************
fhfh*/
SHORT STUBENTRY RmtSeekFile(USHORT usFile, ULONG ulPosition,
                            ULONG FAR *pulActualPosition)
{
    RMTREQSEEK      ReqMsg;
    RMTRESSEEK      ResMsg;
    SHORT           sResult;

    PifRequestSem(husCliExeNet);

    /* set the request and response message */
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    ReqMsg.usFile       = usFile;
    ReqMsg.ulBytes      = ulPosition;
    CliMsg.usFunCode    = CLI_FCRMTSEEKFILE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsg;
    CliMsg.usReqMsgHLen = sizeof(RMTREQSEEK);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsg;
    CliMsg.usResMsgHLen = sizeof(RMTRESSEEK);

    /* send 2170    */
    CstSendMaster();

    /* check the return status  */
    if ((sResult = RmtErrCtl(CliMsg.sRetCode, ResMsg.sReturn)) >= 0) {
        *pulActualPosition  = ResMsg.ulBytes;
        aulPosition[usFile] = ResMsg.ulBytes;
    }

    PifReleaseSem(husCliExeNet);
    return(sResult);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT STUBENTRY RmtWriteFile(USHORT usFile,         **
**                                           VOID FAR *pBuffer,     **
**                                           USHORT usBytes)        **
**              usFile:     file handle                             **
**              pBuffer:    the pointer of the write buffer         **
**              usBytes:    the size of the read data               **
**                                                                  **
**  Return:     the length of the actual read data, or see the      **
**              function of "RmtErrCtl()"                           **
**                                                                  **
** Description: This function reads the file on 2170.               **
**                                                                  **
**********************************************************************
fhfh*/
SHORT STUBENTRY RmtWriteFile(USHORT usFile, VOID FAR *pBuffer, USHORT usBytes)
{
    RMTREQWRITE     ReqMsg;
    RMTRESWRITE     ResMsg;
    UCHAR           *puchPosition;
    SHORT           sBytes, sWriteBytes, sResult, sCurrentBytes;
    ULONG           ulCurrentPos;
    
    /* check the write data size    */
    if (usBytes == 0 || usBytes > RMT_MAXBYTES_SIZE) {
        return(RMT_ERROR_INVALID_BYTES);
    }

    PifRequestSem(husCliExeNet);

    /* set the request and response message */
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    ReqMsg.usFile       = usFile;
    CliMsg.usFunCode    = CLI_FCRMTWRITEFILE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsg;
    CliMsg.usReqMsgHLen = sizeof(RMTREQWRITE);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsg;
    CliMsg.usResMsgHLen = sizeof(RMTRESWRITE);
    
    /* initial  */
    puchPosition  = (UCHAR *)pBuffer;
    sBytes        = (SHORT)usBytes;
    sCurrentBytes = 0;
    ulCurrentPos  = aulPosition[usFile];

    while (sBytes > 0) {

        /* check the read data size */
        if (sBytes > RMT_MSGDATA_SIZE)  sWriteBytes = RMT_MSGDATA_SIZE;
        else                            sWriteBytes = sBytes;

        /* set the length of the write data and copy the write data */
        ReqMsg.usBytes    = sWriteBytes;
        ReqMsg.ulPosition = ulCurrentPos;
        memcpy(ReqMsg.auchData, puchPosition, sWriteBytes);

        /* send 2170    */
        CstSendMaster();

        /* error ocuurs */
        if ((sResult = RmtErrCtl(CliMsg.sRetCode, ResMsg.sReturn)) < 0) {
		    PifReleaseSem(husCliExeNet);
            return(sResult);

        } else {
            if (ReqMsg.ulPosition != ResMsg.ulPosition) {
			    PifReleaseSem(husCliExeNet);
                return(RMT_ERROR_COM_ERRORS);
            }

            /* success      */
            puchPosition  += sWriteBytes;
            sCurrentBytes += sWriteBytes;
            sBytes        -= sWriteBytes;
            ulCurrentPos  += (ULONG)sWriteBytes;
        }

        /* loop */
    }

    aulPosition[usFile] = ulCurrentPos;
    PifReleaseSem(husCliExeNet);
    return(sCurrentBytes);
}
    
/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT RmtErrCtl(SHORT sStubErr, SHORT sPifErr)      **
**              sStubErr:   status of STUB                          **
**              sPifErr:    status of PIF                           **
**                                                                  **
**  Return:     error value or length                               **
**                                                                  **
** Description: This function checks status.                        **
**                                                                  **
**********************************************************************
fhfh*/
SHORT RmtErrCtl(SHORT sStubErr, SHORT sPifErr)
{
    /* error occurs on STUB */
    if (sStubErr < 0)   return(sStubErr);

    /* success              */
    if (sPifErr >= 0)   return(sPifErr);

    /* error occurs on PIF  */
    switch(sPifErr) {
    case PIF_ERROR_FILE_NOT_FOUND:
        return(RMT_ERROR_FILE_NOT_FOUND);

    case PIF_ERROR_FILE_EXIST:
        return(RMT_ERROR_FILE_EXIST);

    case PIF_ERROR_FILE_EOF:
        return(RMT_ERROR_FILE_EOF);

    case PIF_ERROR_FILE_DISK_FULL:
        return(RMT_ERROR_FILE_DISK_FULL);

    default:
        return(sPifErr);
    }
}


