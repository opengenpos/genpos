/*
**********************************************************************
**                                                                  **
**  Title:      File Server                                         **
**                                                                  **
**  File Name:  ISPRMT.C                                            **
**                                                                  **
**  Categoly:   Remote File System                                  **
**                                                                  **
**  Abstruct:   This module provides the functions of the remote    **
**              file system.                                        **
**              This module provides the following functions:       **
**                                                                  **
**                  RmtFileServer():    Closes, deletes, opens,     **
**                                      reads, seeks and writes     **
**                                      the file                    **
**                                                                  **
**********************************************************************
**                                                                  **
**  Modification History:
**
**      Ver.      Date        Name      Description
**      1.00    Sep/11/92   T.Kojima    Initial release
**   02.02.01   Apr/29/15   R.Chambers  added ASSRTLOG logs for open and delete files
**********************************************************************
mhmh*/

/*******************\
|   Include Files   |
\*******************/
#include	<windows.h>
#include	<tchar.h>
#include    <string.h>
#include    <stdio.h>
#include    <memory.h>

#include    "ecr.h"
#include    "pif.h"
#include    "paraequ.h"
#include    "plu.h"
#include    "csstbfcc.h"
#include    "storage.h"
#include    "isp.h"
#include    "ispcom.h"
#include    "rmt.h"
#include	"blfwif.h"
#include	"appllog.h"

/***********************\
|   Static Variables    |
\***********************/
RMTFILETBL  aRmtFileTbl[RMT_NUM_FILETBL];

static SHORT	RmtFindDebugFileNames(TCHAR *aszNames, CONST CHAR *aszMode, CONST TCHAR *aszExtension);
static SHORT   RmtFindSavFileNames(TCHAR aszNames[RMT_NUM_SAV_FILE_NM][STD_FILENAME_LENGTH_MAX]);
static SHORT   RmtCheckOpenMode(CONST CHAR *pchMode);

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID RmtCloseAllFile(VOID)                          **
**                                                                  **
** Description: This function closes all files.                     **
**                                                                  **
**********************************************************************
fhfh*/
VOID RmtCloseAllFile(VOID)
{
    USHORT   i;

    /* closes all files */
    for (i = 0; i < RMT_NUM_FILETBL; i++) {
        if (aRmtFileTbl[i].chOpenFile == TRUE) {
            PifCloseFile(aRmtFileTbl[i].usFile);
        }
    }
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID RmtInitFileTbl(VOID)                           **
**                                                                  **
** Description: This function initializes file table.               **
**                                                                  **
**********************************************************************
fhfh*/
VOID RmtInitFileTbl(VOID)
{
    USHORT   i;

    /* initialize file table    */
    for (i = 0; i < RMT_NUM_FILETBL; i++) {
        aRmtFileTbl[i].chOpenFile = FALSE;
        aRmtFileTbl[i].ulPosition = 0L;
    }
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID RmtFileServer(VOID)                            **
**                                                                  **
** Description: This function executes each functions.              **
**                                                                  **
**********************************************************************
fhfh*/
VOID RmtFileServer(VOID)
{
    VOID    *pReqMsgH, *pResMsgH;
    ULONG   ulActPos;
	ULONG	ulActualBytesRead; //11-7-3 RPH
    USHORT  usC, usLen;
    SHORT   sResult;

	NHPOS_ASSERT_ARRAYSIZE(auchIspTmpBuf, PIF_LEN_UDATA);

    /* clear response message   */
    usLen = 0;
    memset(auchIspTmpBuf, 0, sizeof(auchIspTmpBuf));

    pReqMsgH = IspRcvBuf.auchData;
    pResMsgH = auchIspTmpBuf;

    ((CLIRESCOM *)pResMsgH)->usFunCode = ((CLIREQCOM *)pReqMsgH)->usFunCode;
    ((CLIRESCOM *)pResMsgH)->usSeqNo   = ((CLIREQCOM *)pReqMsgH)->usSeqNo & CLI_SEQ_CONT;
    ((CLIRESCOM *)pResMsgH)->sResCode  = STUB_SUCCESS;

    switch(((CLIREQCOM *)pReqMsgH)->usFunCode & CLI_RSTCONTCODE) {
	/* reload layout file */
	case CLI_FCRMTRELOADLAY:
		usLen = sizeof(RMTRESCLOSE);
        ((RMTRESCLOSE *)pResMsgH)->sReturn = RMT_OK;

		PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_RELOADLAYOUT);
		BlFwIfReloadLayout2();
		break;
	
	case CLI_FCRMTRELOADOEP:
		usLen = sizeof(RMTRESCLOSE);
		((RMTRESCLOSE *)pResMsgH)->sReturn = RMT_OK;
		PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_RELOADOEP);
		BlFwIfReloadOEPGroup();
		break;

    /* close the file   */
    case CLI_FCRMTCLOSEFILE:
        usLen = sizeof(RMTRESCLOSE);
        usC   = ((RMTREQCLOSE *)pReqMsgH)->usFile;

        /* check file handle    */
        if (aRmtFileTbl[usC].chOpenFile != TRUE) {
            ((RMTRESCLOSE *)pResMsgH)->sReturn = RMT_ERROR_INVALID_HANDLE;
			PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_CLI_FCRMT_HANDLE);
            break;
        }

        /* close the file       */
        PifCloseFile(aRmtFileTbl[usC].usFile);

        aRmtFileTbl[usC].chOpenFile = FALSE;
        ((RMTRESCLOSE *)pResMsgH)->sReturn = RMT_OK;
        break;

    /* delete the file  */
    case CLI_FCRMTDELETEFILE:
        usLen = sizeof(RMTRESDELETE);

        /* check the file   */
        if (RmtCheckOpenedFile() == TRUE) {
            ((RMTRESDELETE *)pResMsgH)->sReturn = RMT_ERROR_OPENED_FILE;
            break;
        }

        /* delete the file  */
        PifDeleteFile(((RMTREQDELETE *)pReqMsgH)->aszFileName);

		PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_DELETE_FILE);
        ((RMTRESDELETE *)pResMsgH)->sReturn = RMT_OK;
		{
			char  xBuff[128];
			sprintf (xBuff, "==NOTE: CLI_FCRMTDELETEFILE file %-.32S.", ((RMTREQDELETE *)pReqMsgH)->aszFileName);
			NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
		}
        break;

    /* open the file    */
    case CLI_FCRMTOPENFILE:
        usLen = sizeof(RMTRESOPEN);

        /* search for room in the file table    */
        for (usC = 0; usC < RMT_NUM_FILETBL; usC++) {
            if (aRmtFileTbl[usC].chOpenFile != TRUE)    break;
        }

        if (usC >= RMT_NUM_FILETBL) {
            ((RMTRESOPEN *)pResMsgH)->sReturn = RMT_ERROR_OPENED_FILE;
			PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_CLI_FCRMT_TABLEFULL);
			NHPOS_ASSERT_TEXT(usC < RMT_NUM_FILETBL, "==ERROR: RMT_ERROR_OPENED_FILE - aRmtFileTbl[] full.");
            break;
        }

        /* check open mode  */
        if (RmtCheckOpenMode(((RMTREQOPEN *)pReqMsgH)->aszMode) < 0) {
            ((RMTRESOPEN *)pResMsgH)->sReturn = RMT_ERROR_INVALID_MODE;
            break;
        }

        /* open a file  */
        if ((sResult = PifOpenFile(((RMTREQOPEN *)pReqMsgH)->aszFileName, ((RMTREQOPEN *)pReqMsgH)->aszMode)) >= 0) {
			CHAR    aszMode[RMT_MODE_SIZE + 1] = {0};

            aRmtFileTbl[usC].usFile           = (USHORT)sResult;
            aRmtFileTbl[usC].chOpenFile       = TRUE;
            aRmtFileTbl[usC].ulPosition       = 0L;
            ((RMTRESOPEN *)pResMsgH)->sReturn = usC;

            /* --- FVT #5,  Jan/20/99 --- */
            memcpy(aszMode, ((RMTREQOPEN *)pReqMsgH)->aszMode, RMT_MODE_SIZE);
            if (aszMode[0] == 'w' || aszMode[1] == 'w' || 
                aszMode[2] == 'w' || aszMode[3] == 'w') {
                memset(IspResetLog.aszMnemonic, 0, sizeof(IspResetLog.aszMnemonic));
                _tcsncpy(IspResetLog.aszMnemonic, ((RMTREQOPEN *)pReqMsgH)->aszFileName, RMT_FILENAME_SIZE);
                IspResetLog.ulNumber = (ULONG)sResult;
            }
            /* --- FVT #5,  Jan/20/99 --- */
        } else {
			/* fail to open */
            ((RMTRESOPEN *)pResMsgH)->sReturn = sResult;
        }
		{
			char  xBuff[128];
			sprintf (xBuff, "==NOTE: CLI_FCRMTOPENFILE file %-.32S as %-.4s sResult %d.", ((RMTREQOPEN *)pReqMsgH)->aszFileName, ((RMTREQOPEN *)pReqMsgH)->aszMode, sResult);
			NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
		}
        break;

    /* read the file    */
    case CLI_FCRMTREADFILE:
        usLen = sizeof(RMTRESREAD);
        usC   = ((RMTREQREAD *)pReqMsgH)->usFile;

        /* check file handle    */
        if (aRmtFileTbl[usC].chOpenFile != TRUE) {
            ((RMTRESREAD *)pResMsgH)->lReturn = RMT_ERROR_INVALID_HANDLE;
			PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_CLI_FCRMT_HANDLE);
            break;
        }

        /* check the read pointer of the file   */
        if (aRmtFileTbl[usC].ulPosition != ((RMTREQREAD *)pReqMsgH)->ulPosition) {
            if ((sResult = PifSeekFile(aRmtFileTbl[usC].usFile, ((RMTREQREAD *)pReqMsgH)->ulPosition, &ulActPos)) < 0) {
                ((RMTRESREAD *)pResMsgH)->lReturn = sResult;
                break;
            }

            aRmtFileTbl[usC].ulPosition = ulActPos;
        }

        /* read the file    */// 11-7-3 RPH Modified for Change if PifReadFile see new usage of PifReadFile in piffile.c
        sResult = PifReadFile(aRmtFileTbl[usC].usFile, ((RMTRESREAD *)pResMsgH)->auchData,
                                   ((RMTREQREAD *)pReqMsgH)->usBytes, &ulActualBytesRead);
		if ( ulActualBytesRead >= 0) {
            /* update offset of the file    */
            aRmtFileTbl[usC].ulPosition += ulActualBytesRead;
            ((RMTRESREAD *)pResMsgH)->ulPosition = ((RMTREQREAD *)pReqMsgH)->ulPosition;
        }

        ((RMTRESREAD *)pResMsgH)->lReturn = ulActualBytesRead;

        break;

    /* seek the file    */
    case CLI_FCRMTSEEKFILE:
        usLen = sizeof(RMTRESSEEK);
        usC   = ((RMTREQSEEK *)pReqMsgH)->usFile;

        /* check file handle    */
        if (aRmtFileTbl[usC].chOpenFile != TRUE) {
            ((RMTRESSEEK *)pResMsgH)->sReturn = RMT_ERROR_INVALID_HANDLE;
			PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_CLI_FCRMT_HANDLE);
            break;
        }

        /* seek the file    */
        if ((sResult = PifSeekFile(aRmtFileTbl[usC].usFile, ((RMTREQSEEK *)pReqMsgH)->ulBytes, &ulActPos)) >= 0) {
            /* set offset of the file   */
            aRmtFileTbl[usC].ulPosition      = ulActPos;
            ((RMTRESSEEK *)pResMsgH)->ulBytes = ulActPos;
        }

        ((RMTRESSEEK *)pResMsgH)->sReturn = sResult;
        break;

	case CLI_FCRMTDBGFILENM:
		usLen = sizeof(RMTRESNAME);
		((RMTRESNAME *)pResMsgH)->sReturn = RmtFindDebugFileNames(((RMTRESNAME *)pResMsgH)->aszFileName[0],((RMTREQNAME *)pReqMsgH)->aszMode, ((RMTREQNAME *)pReqMsgH)->aszExtension);
		break;

	case CLI_FCRMTSAVFILENM:
		usLen = sizeof(RMTRESSAVNAME);
		((RMTRESSAVNAME *)pResMsgH)->sReturn = RmtFindSavFileNames(((RMTRESSAVNAME *)pResMsgH)->aszFileName);
		break;

    /* write the file   */
    case CLI_FCRMTWRITEFILE:
        usLen = sizeof(RMTRESWRITE);
        usC   = ((RMTREQWRITE *)pReqMsgH)->usFile;

        /* check file handle    */
        if (aRmtFileTbl[usC].chOpenFile != TRUE) {
            ((RMTRESWRITE *)pResMsgH)->sReturn = RMT_ERROR_INVALID_HANDLE;
			PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_CLI_FCRMT_HANDLE);
            break;
        }

        /* check KB lock    */
        if ((fsIspCleanUP & ISP_LOCKED_KEYBOARD) != ISP_LOCKED_KEYBOARD) {
            ((RMTRESWRITE *)pResMsgH)->sReturn = RMT_ERROR_NOT_LOCKED;
            break;
        }

        /* check the write pointer of the file  */
        if (aRmtFileTbl[usC].ulPosition != ((RMTREQWRITE *)pReqMsgH)->ulPosition) {
            if ((sResult = PifSeekFile(aRmtFileTbl[usC].usFile, ((RMTREQWRITE *)pReqMsgH)->ulPosition, &ulActPos)) < 0) {
                ((RMTRESREAD *)pResMsgH)->lReturn = sResult;
                break;
            }

            aRmtFileTbl[usC].ulPosition = ulActPos;
        }

        /* write the file   */
        PifWriteFile(aRmtFileTbl[usC].usFile, ((RMTREQWRITE *)pReqMsgH)->auchData, ((RMTREQWRITE *)pReqMsgH)->usBytes);

        /* update offset of the file    */
        aRmtFileTbl[usC].ulPosition += (ULONG)((RMTREQWRITE *)pReqMsgH)->usBytes;

        ((RMTRESWRITE *)pResMsgH)->ulPosition = ((RMTREQWRITE *)pReqMsgH)->ulPosition;
        ((RMTRESWRITE *)pResMsgH)->sReturn = RMT_OK;

        /* --- FVT #5,  Jan/20/99 --- */
        if (IspResetLog.ulNumber != 0L) {
            IspResetLog.ulNumber = 0L;
            IspRecvResetLogInternalNote(UIF_UCHRESETLOG_LOGONLY, 0);
        }
        /* --- FVT #5,  Jan/20/99 --- */
        break;

	default:
        usLen = sizeof(RMTRESCLOSE);
        ((RMTRESCLOSE *)pResMsgH)->sReturn = RMT_ERROR_INVALID_HANDLE;
		PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_CLI_FCRMT_ERROR);
		break;
    }

    /* send response message to PC  */
    IspSendResponse((CLIRESCOM *)pResMsgH, usLen, NULL, 0);
}
        
/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT RmtCheckOpenMode(CHAR *pchMode)               **
**                                                                  **
**  Return:     TRUE    OK                                          **
**              FALSE   invalid open mode                           **
**                                                                  **
** Description: This function checks the open mode of the file.     **
**              See also function dfckmd() in piffile.c             **
**              Changes to RmtCheckOpenMode() may require changes   **
**              to the corresponding remote function dfckmd().      **
**********************************************************************
fhfh*/
static SHORT RmtCheckOpenMode(CONST CHAR *pchMode) 
{
    CHAR    chMode;
    SHORT   sBits = 0;
	// following list of all folders used to determine if any folder other than specified folder has already been indicated.
	// typical usage: if (sBits & (sBitsFolders & ~RMT_PRINTFILES)) sBits = (-1); // folder other than RMT_PRINTFILES already specified.
	SHORT   sBitsFolders = (RMT_TEMP | RMT_FLASH | RMT_SAVTTL | RMT_ICON | RMT_PRINTFILES | RMT_CONFIG | RMT_HISFOLDER | RMT_IMAGESFOLDER);

    /* check the mode       */
    for (chMode = *pchMode; chMode > 0 && sBits >= 0; chMode = *(++pchMode)) {
        switch (chMode) {
        case 'r':
			sBits |= RMT_READ;
            break;

        case 'w':
			sBits |= RMT_WRITE;
            break;

        case 'o':
            if (sBits & RMT_NEW)
				sBits = (-1);
            else
				sBits |= RMT_OLD;
            break;

        case 'n':
            if (sBits & RMT_OLD)
				sBits = (-1);
            else
				sBits |= RMT_NEW;
			break;

		case 'i':         // FLICON - icon files folder
			if (sBits & (sBitsFolders & ~RMT_ICON))         // check that no other folder than Icons folder indicated
				sBits = (-1);
			else
				sBits |= RMT_ICON;
			break;

		case 'p':         // FLPRINT - file is in PrintFiles folder
			if (sBits & (sBitsFolders & ~RMT_PRINTFILES))     // check that no other folder than PrintFiles folder indicated
				sBits = (-1);
			else
				sBits |= RMT_PRINTFILES;
			break;

		case 's':         // FLSAVTTL - file is in Database\SavedTotal folder
			if (sBits & (sBitsFolders & ~RMT_SAVTTL))         // check that no other folder than Database\SavedTotal folder indicated
				sBits = (-1);
			else
				sBits |= RMT_SAVTTL;
			break;

		case 'f':
			if (sBits & (sBitsFolders & ~RMT_FLASH))         // check that no other folder than Database folder indicated
				sBits = (-1);
			else
				sBits |= RMT_FLASH;
			break;

		case 't':         // FLTMP - temporary files folder
			if (sBits & (sBitsFolders & ~RMT_TEMP))          // check that no other folder than TempDisk folder indicated
				sBits = (-1);
			else
				sBits |= RMT_TEMP;
            break;

		case 'b':         // FLIMAGESFOLDER - image files folder
			if (sBits & (sBitsFolders & ~RMT_IMAGESFOLDER))  // check that no other folder than Database\Images folder indicated
				sBits = (-1);
			else
				sBits |= RMT_IMAGESFOLDER;
            break;

        default:
			sBits = (-1);
			break;
        }
    }

    return(sBits);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   BOOL RmtCheckOpenedFile(VOID)                       **
**                                                                  **
**  Return:     TRUE    opened a file                               **
**              FALSE   OK                                          **
**                                                                  **
** Description: This function checkes the file status.              **
**                                                                  **
**********************************************************************
fhfh*/
BOOL RmtCheckOpenedFile(VOID)
{
    USHORT   i;

    for (i = 0; i < RMT_NUM_FILETBL; i++) {
        /* if the file is opened    */
        if (aRmtFileTbl[i].chOpenFile == TRUE)  return(TRUE);
    }

    /* not opened files */
    return(FALSE);
}
/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT RmtFindDebugFileNames(TCHAR *aszNames,		**
**				CHAR *aszMode, TCHAR *aszExtension)					**
**                                                                  **
**  Return:     TRUE    opened a file                               **
**              FALSE   aszMode not valid                           **
**				RMT_ERROR_END_OF_FILE_SEARCH	no more files		**
**                                                                  **
** Description: This function checks for debug file names			**
**               A Remote File Server command of CLI_FCRMTDBGFILENM **
**               is used along with a mode string that specifies    **
**               the single letter identifier of the folder to      **
**               search.                                            **
**                                                                  **
**               See also function RmtFindSavFileNames() which is   **
**               specifically for Saved Totals, .sav, files.        **
**                                                                  **
**              WARNING: This functionality does not support more   **
**                       than a single remote process at a time.    **
**                       No more than one remote process should be  **
**                       accessing this functionality at any time.  **
**                                                                  **
**********************************************************************
fhfh*/
static SHORT RmtFindDebugFileNames(TCHAR *aszNames, CONST CHAR *aszMode, CONST TCHAR *aszExtension)
{
	static HANDLE fileSearch = 0;         // fileFindNext() search handle
	CONST  TCHAR  *myFileName;
	CONST  TCHAR  *tcsFolderName = 0;
	ULONG	ulBytesWritten = 0;
	DWORD   viSize = 0;
	TCHAR   infoBuffer2[RMT_NUM_DBG_FILE][RMT_FILENAME_SIZE] = {0};
	WIN32_FIND_DATA myFoundFile;
	BOOL    doit = TRUE;
	SHORT   sOpenMode = RmtCheckOpenMode(aszMode);
	SHORT   sAllowedModes = (RMT_FLASH |
							RMT_SAVTTL |
							RMT_ICON |
							RMT_PRINTFILES |
							RMT_CONFIG |
							RMT_HISFOLDER |
							RMT_IMAGESFOLDER);

	switch (sOpenMode & (sAllowedModes)) {
		case RMT_FLASH:
			 myFileName = _T("DD*");    // find all DD debug files only. Ignore requested filter.
			break;
		case RMT_SAVTTL:
			myFileName = _T("*.sav");   // find all saved total files only. Ignore requested filter.
			break;
		case RMT_ICON:
		case RMT_PRINTFILES:
		case RMT_CONFIG:
		case RMT_HISFOLDER:
		case RMT_IMAGESFOLDER:
			myFileName = aszExtension;   // allow any files the user has requested.
			break;
		default:
			if(fileSearch) {
				FindClose (fileSearch);
				fileSearch = NULL;
			}
			return FALSE;
	}
	tcsFolderName = PifGetFilePath (NULL, aszMode, NULL);
	_tchdir(tcsFolderName);

	if (!fileSearch)
	{
		fileSearch = FindFirstFile (myFileName, &myFoundFile);
		doit = (fileSearch != INVALID_HANDLE_VALUE);
		if (fileSearch == INVALID_HANDLE_VALUE)
		{
			DWORD  dwLastError = GetLastError();
			if (dwLastError != ERROR_FILE_NOT_FOUND)
			{
				char xBuff[128];
				sprintf (xBuff, "==ERROR: RmtFindDebugFileNames() FindFirstFile() INVALID_HANDLE_VALUE GetLastError() %d", dwLastError);
				NHPOS_ASSERT_TEXT(fileSearch != INVALID_HANDLE_VALUE, xBuff);
			}
			fileSearch = 0;    // indicate that we do not have an active search in progress.
		}
	} else
	{
		doit = FindNextFile (fileSearch, &myFoundFile);
	}
	
	if (doit)
	{
		// the original source only provided one file at a time so that is what we
		// are doing here in order to maintain backwards compatibility for Third Party
		// remote applications such as CWS/MWS or RSI.
		memcpy(&infoBuffer2[0], myFoundFile.cFileName, sizeof(infoBuffer2[0]));
		memcpy(aszNames, &infoBuffer2, sizeof(infoBuffer2));
	} else
	{
		// determine if we are done searching or not and if so then close out
		// the search and indicate that we no longer have an active search going.
		fileSearch && (FindClose (fileSearch), (fileSearch = 0));
		return RMT_ERROR_END_OF_FILE_SEARCH;
	}

    return(TRUE);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT RmtFindSavFileNames(TCHAR *aszNames)			**
**																	**
**				aszNames will hold names of sav files				**
**                                                                  **
**  Return:     number of file names in aszNames					**
**                                                                  **
** Description: This function checks for .sav file names			**
**              The user can repeatedly call this function which    **
**              will provide a list of saved total file names.      **
**              The user will know that there are no more filenames **
**              when a call returns a value of zero.                **
**                                                                  **
**              if the return value is zero then the next time the  **
**              function is called, it will restart from the first. **
**                                                                  **
**              WARNING: This functionality does not support more   **
**                       than a single remote process at a time.    **
**                       No more than one remote process should be  **
**                       accessing this functionality at any time.  **
**                                                                  **
**********************************************************************
fhfh*/
static SHORT RmtFindSavFileNames(TCHAR aszNames[RMT_NUM_SAV_FILE_NM][RMT_FILENAME_SIZE])
{
	static WIN32_FIND_DATA myFoundFile = {0};
	static HANDLE fileSearch = 0;      // indicate we do not have an active search ongoing
	TCHAR  infoBuffer2[RMT_NUM_SAV_FILE_NM][RMT_FILENAME_SIZE] = {0};
	SHORT  sRet = 0;
	BOOL   doIt = FALSE;

	_tchdir(STD_FOLDER_SAVTTLFILES);

	// fileSearch is static, so if num of .sav files on terminal is greater
	// than RMT_NUM_SAV_FILE_NM, this function can be called multiple times
	// until all file names are found.
	if (fileSearch == 0){
		TCHAR * myFileName = _T("*.sav");

		doIt = ((fileSearch = FindFirstFile (myFileName, &myFoundFile)) != INVALID_HANDLE_VALUE);
		if(fileSearch == INVALID_HANDLE_VALUE) {
			DWORD  dwLastError = GetLastError();
			if (dwLastError != ERROR_FILE_NOT_FOUND)
			{
				char xBuff[128];
				sprintf (xBuff, "==ERROR: RmtFindSavFileNames() FindFirstFile() INVALID_HANDLE_VALUE GetLastError() %d", dwLastError);
				NHPOS_ASSERT_TEXT(fileSearch != INVALID_HANDLE_VALUE, xBuff);
			}
			fileSearch = 0;    // indicate that we do not have an active search in progress.
		}
	} else {
		// we are in the process of doing a search so lets continue where we last left off.
		doIt = FindNextFile (fileSearch, &myFoundFile);
	}

	sRet = 0;
	while ( doIt ) {
		memcpy(&infoBuffer2[sRet], myFoundFile.cFileName, sizeof(infoBuffer2[sRet]));
		if (++sRet >= RMT_NUM_SAV_FILE_NM) break;
		doIt = FindNextFile (fileSearch, &myFoundFile);
	}

	if (! doIt) {
		// determine if we are done searching or not and if so then close out
		// the search and indicate that we no longer have an active search going.
		fileSearch && (FindClose (fileSearch), (fileSearch = 0));
	}

	memcpy(aszNames, &infoBuffer2, sizeof(infoBuffer2));
    return(sRet);
}


/****  End of Source ****************/
