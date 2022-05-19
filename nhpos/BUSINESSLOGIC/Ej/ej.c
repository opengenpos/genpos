/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1997            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : EJ, Electoric Journal Function Library
* Category    : EJMGR, NCR 2170 INT'L Hospitarity Application
* Program Name: EJ.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:
*       EJInit()            : Create Electronic Journal Semaphore
*       EJCreat()           : Create Electronic Journal File
*       EJCheckD()          : Delete Electronic Journal File
*       EJCheckC()          : Create Electronic Journal File
*       EJClear()*          : Reset Electronic Journal File
*       EJWrite()*          : Write data to EJ File
*       EJReverse()*        : Read Last E.J. Report Data from the EJ File
*       EJRead()*           : Read E.J. Report Data from the EJ File
*       EJGetFlag()*        : Return EJ File status flag
*       EJConvertError()    : This function converts error code to leadthru number
*       EJReadFile()*       : Read E.J. File Data from the EJ File (for ISP)
*       EJRead1Line()*      : Read E.J. 1 Line Report Data from the EJ File Add R3.0
*           EJ_W_Close()
*           EJ_Set_End()
*           EJ_Set_Left()
*           EJ_Set_Right()
*           EJ_Set_1Tr()
*           EJ_DelBlk()
*           EJ_WriteFile()
*           EJ_ReadFileA()
*           EJ_ReadPrtCtl()
*           EJ_SetDesc()
*           EJ_ResetDesc()
* note: * is controlled by semaphore
* --------------------------------------------------------------------------
* Update Histories
*    Data  :Ver.Rev.:   Name    : Description
* Jan-28-93:00.00.01:T.Asawa    : Initial
* Feb-02-93:00.00.01:K.you      : Update
* Jul-08-93:01.00.12:K.you      : Migration to US
* Aug-26-93:01.00.13:K.you      : mod. error code ( mod. EJWrite() )
* Sep-23-93:02.00.01:K.you      : bug fixed for FAULT 49532 ( mod. EJRead() )
* Oct-04-93:02.00.01:K.you      : bug fixed for E-93 ( mod. EJReadFile() )
* Mar-01-94:00.00.04:K.you      : bug fixed ( mod. EJGetFlag() )
* Jul-21-95:03.00.03:M.Ozawa    : Enhanced for LCD descriptor
* Aug-09-95:03.00.03:M.Suzuki   : Add EJRead1Line
* Dec-08-95:03.01.00:M.Ozawa    : Change EJWrite argument to FAR
* Jun-25-96:03.01.09:M.Ozawa    : Expand EJ Reverse Print to set parameter
* Mar-14-97:02.00.04:hrkato     : R2.0(Migration)
* May-21-97:02.00.06:M.Ozawa    : Add EJLock(), EJUnLock()
*
** NCR2171 **
* Aug-26-99:01.00.00:M.Teraki   : initial (for 2171)
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
#include	<stdio.h>
#include	<stdlib.h>

#include <ecr.h>
#include <rfl.h>
#include <log.h>
#include <pif.h>
#include <uie.h>
#include <appllog.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <ej.h>
#include <pmg.h>
#include <appllog.h>
#include <string.h>
#include "plu.h"
#include "pararam.h"
#include "cscas.h"
#include "csstbej.h"
#include "maint.h"

/*
*===========================================================================
*   Extern Declarations
*===========================================================================
*/
static UCHAR   fchEJLock = 0;                   /* E.J. Lock Flag, R2.0 GCA */

static UCHAR    fchEJFlag;                      /* E.J. Status Used by EJRead() */
                                                /* EJ_Set_End(), EJ_Set_Left(), */
                                                /* EJ_Set_Right() */

static SHORT    hsEJFileHandle = PIF_FILE_INVALID_HANDLE;     /* E.J. File Handle */
static SHORT	hsEJDBFileHandle = PIF_FILE_INVALID_HANDLE;   /* DB E.J. File Handle */
static USHORT   husEJSem = PIF_FILE_INVALID_HANDLE;           /* E.J. Semaphore */

static TCHAR FARCONST aszEJFileName[] = _T("TOTALEJF");       /* File Name of Electronic Journal */
static TCHAR FARCONST aszEJDBFileName[]  = _T("TOTALEJFDB");  /*File Name of EJ Delay Balance*/

USHORT   usIspEJState = 0;            /* E/J read/reset state, V3.3 */


struct {
	TCHAR	*aszFileName;             // pointer to file name
	SHORT   phsHandle;               // pointer to global file handle
} stEJFiles [] = {
	{aszEJFileName, -1},
	{aszEJDBFileName, -1},
	{0, -1}
};

static VOID  EJInitFileHeader (EJF_HEADER  *pFHeader)
{
    DATE_TIME    Date_Time;

	// Only initialize those parts of the Electronic Journal file header that
	// are changed during operations.  Do not touch file size or other data.
	PifGetDateTime (&Date_Time);
	pFHeader->usMonth  = Date_Time.usMonth;
	pFHeader->usDay    = Date_Time.usMDay;
	pFHeader->usHour   = Date_Time.usHour;
	pFHeader->usMin    = Date_Time.usMin;

	pFHeader->usTtlLine    = 0;
	pFHeader->ulPrevPoint  = 0L;                         /* Clear last position */
	pFHeader->ulNextPoint  = sizeof(EJF_HEADER);         /* Set Start position */
	pFHeader->ulEndofPoint = sizeof(EJF_HEADER);        /* Set End position */
	pFHeader->ulBeginPoint = sizeof(EJF_HEADER);        /* Set Begin position */
}

SHORT	EJConsistencyCheck (VOID)
{
	ULONG		ulActualBytesRead;//RPH 11-10-3 SR#201
    EJF_HEADER  FHeader;
    EJT_HEADER  THeader, THeaderPrev;
	SHORT       sRetStatus = EJ_PERFORM;

	EJ_ReqSem();                           /* Get Semaphore */

    /* File not exist case, if error EJ_OpenFile() will release semaphore */
    if (EJ_OpenFile() != EJ_PERFORM) {
        return (EJ_FILE_NOT_EXIST);
    }

	EJ_ReadFileA(EJ_TOP, &FHeader, sizeof(EJF_HEADER), &ulActualBytesRead);
	{
		char xBuff[128];
		sprintf (xBuff, "==NOTE: EJ Consistency FHeader usMonth %d  usDay %d  usHour %d  usMin %d", FHeader.usMonth, FHeader.usDay, FHeader.usHour, FHeader.usMin);
		NHPOS_NONASSERT_TEXT(xBuff);
		sprintf (xBuff, "        EJ Consistency FHeader ulBeginPoint %d  ulPrevPoint %d  ulNextPoint %d", FHeader.ulBeginPoint, FHeader.ulPrevPoint, FHeader.ulNextPoint);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

	if (FHeader.ulPrevPoint > 0) {
		// these consistency checks are only good if there is something in the EJ file.
		EJ_ReadFileA(FHeader.ulPrevPoint, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
		{
			char xBuff[128];
			sprintf (xBuff, "==NOTE: EJ Consistency THeader ulOffset %d  usCVLI %d  usConsecutive %d  usGCFNo %d  usDate %d  usTime %d", FHeader.ulPrevPoint, THeader.usCVLI, THeader.usConsecutive, THeader.usGCFNo, THeader.usDate, THeader.usTime);
			NHPOS_NONASSERT_TEXT(xBuff);
		}

		if (THeader.usCVLI < sizeof(EJT_HEADER)) {
			char xBuff[128];
			// there is a problem with the record length specified in the header
			sprintf(xBuff, "**WARNING: bad usCVLI at %d  0x%x", FHeader.ulPrevPoint, FHeader.ulPrevPoint);
			NHPOS_ASSERT_TEXT((THeader.usCVLI >= sizeof(EJT_HEADER)), xBuff);
		}

		if (FHeader.ulPrevPoint + THeader.usCVLI != FHeader.ulNextPoint) {
			char xBuff[128];
			// there is a problem with the record length specified in the header
			sprintf(xBuff, "**WARNING: inconsistent usCVLI and ulPrevPoint and ulNextPoint at %d  0x%x", FHeader.ulPrevPoint, FHeader.ulPrevPoint);
			NHPOS_ASSERT_TEXT((THeader.usCVLI >= sizeof(EJT_HEADER)), xBuff);
		}

		if (FHeader.ulPrevPoint > THeader.usPVLI) {
			char xBuff[128];
			EJ_ReadFileA(FHeader.ulPrevPoint - THeader.usPVLI, &THeaderPrev, sizeof(EJT_HEADER), &ulActualBytesRead);
			sprintf (xBuff, "==NOTE: EJ Consistency THeaderPrev ulOffset %d  usCVLI %d  usConsecutive %d  usGCFNo %d  usDate %d  usTime %d", FHeader.ulPrevPoint - THeader.usPVLI, THeaderPrev.usCVLI, THeaderPrev.usConsecutive, THeaderPrev.usGCFNo, THeaderPrev.usDate, THeaderPrev.usTime);
			NHPOS_NONASSERT_TEXT(xBuff);
			if (THeader.usPVLI > 0 && THeaderPrev.usCVLI < sizeof(EJT_HEADER)) {
				// there is a problem with the record length specified in the header
				sprintf(xBuff, "**WARNING: bad usCVLI at %d  0x%x", FHeader.ulPrevPoint - THeader.usPVLI, FHeader.ulPrevPoint - THeader.usPVLI);
				NHPOS_ASSERT_TEXT((THeaderPrev.usCVLI >= sizeof(EJT_HEADER)), xBuff);
			}
		}
	}

    EJ_CloseAndRelSem();                            /* Close file and Rel. Sem */
    return (sRetStatus);
}


/*
*==========================================================================
**    Synopsis:    SHORT   EJInit(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  EJ_PERFORM
*
**  Description:
*               Create Electronic Journal Semaphore.
*==========================================================================
*/
SHORT   EJInit(VOID)
{
    husEJSem = PifCreateSem( 1 );
//	PifHighLowSem(husEJSem, PIF_SEM_FLAG_LOG_REQ_REL, 6, 0);
	fchEJLock = 0;                                /* initialize the lock flag */
    return(EJ_PERFORM);
}


/*
*==========================================================================
**    Synopsis:    SHORT   EJCreat(USHORT usNoOfBlock, UCHAR fchFlag)
*
*       Input:    USHORT  usNoOfBlock - Number of E.J. Block
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  EJ_PERFORM
*          Abnormal End:  EJ_NO_MAKE_FILE
*
**  Description:
*               Create Electronic Journal File.    If block size is 0, EJ file
*                never exist.
*==========================================================================
*/
SHORT   EJCreat(USHORT usNoOfBlock, UCHAR fchFlag)
{
    SHORT        sReturnST,sRet = EJ_PERFORM;
	SHORT		 i;

	for(i = 0; stEJFiles[i].aszFileName; i++)
	{
		ULONG        ulActSize;
		ULONG        ulActMove;

		PifDeleteFile(stEJFiles[i].aszFileName);           /* Delete Old file, if file is exist. */

		EJ_ResetDesc();                         /* Delight EJ Full Descriptor */

		if (0 == usNoOfBlock) {                 /* Size is zero, do nothing */
			continue;
		}

		/* Open EJ File (New) */
		if ((stEJFiles[i].phsHandle = PifOpenFile(stEJFiles[i].aszFileName, auchNEW_FILE_WRITE)) < 0) {
			PifLog(MODULE_EJ, FAULT_ERROR_FILE_OPEN);
			PifLog(MODULE_ERROR_NO(MODULE_EJ), (USHORT)abs(stEJFiles[i].phsHandle));
			PifLog(MODULE_LINE_NO(MODULE_EJ), (USHORT) __LINE__);
			PifAbort(MODULE_EJ, FAULT_ERROR_FILE_OPEN);
		}

		/* Check Number of Block */
		if (usNoOfBlock < FLEX_EJ_MIN) {
			usNoOfBlock = FLEX_EJ_MIN;
		}

		ulActSize = EJ_BLOCKSIZE * (ULONG)usNoOfBlock;              /* Calculate File Size */
		sReturnST = PifSeekFile(stEJFiles[i].phsHandle, ulActSize, &ulActMove);
		if ((sReturnST < 0) || (ulActMove != ulActSize)) {          /* Can not make file */
			EJ_CloseFile(stEJFiles[i].phsHandle);                   /* Close file then delete it */
			PifDeleteFile(stEJFiles[i].aszFileName);
			sRet = EJ_NO_MAKE_FILE;
		} else {
			EJF_HEADER   EJFHeader;

			hsEJFileHandle = stEJFiles[i].phsHandle;

			/* Make file header */
			EJInitFileHeader (&EJFHeader);
			EJFHeader.fchEJFlag = fchFlag;
			EJFHeader.ulEJFSize = ulActSize;                    /* File size at Creation */

			/* Write File Header */
			EJ_WriteFile(EJ_TOP, &EJFHeader, sizeof(EJF_HEADER), hsEJFileHandle);   /* Write File Header */
			EJ_CloseFile(hsEJFileHandle);                                         /* Close file then delete it */
			PifLog(MODULE_EJ, LOG_EVENT_EJ_FCREAT);
			PifLog(MODULE_LINE_NO(MODULE_EJ), (USHORT) __LINE__);
			/* fchEJFlag = fchFlag;                                    EJ File exist */
		}
	}

	hsEJFileHandle = PIF_FILE_INVALID_HANDLE;
	return sRet;
}

/*
*==========================================================================
**    Synopsis:    SHORT   EJCheckD(USHORT usNoOfBlock, UCHAR fchFlag)
*
*       Input:    USHORT  usNoOfBlock - Number of E.J. Block
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  EJ_PERFORM
*          Abnormal End:  EJ_PERFORM
*
**  Description:
*               Delete Electronic Journal File. If opened file is error case, or
*                file size and status are unmatch case.
*==========================================================================
*/
SHORT   EJCheckD(USHORT usNoOfBlock, UCHAR fchFlag)
{
	SHORT		 i;

	for(i = 0; stEJFiles[i].aszFileName; i++)
	{
		ULONG	     ulActualBytesRead; //RPH 11-10-3 SR# 201
		EJF_HEADER   EJFHeader;

		if ((stEJFiles[i].phsHandle = PifOpenFile(stEJFiles[i].aszFileName, auchOLD_FILE_READ)) < 0) {  /* Error case */
			continue;			
			//return (EJ_PERFORM);
		}

		hsEJFileHandle = stEJFiles[i].phsHandle;
		//RPH 11-10-3 SR# 201
		EJ_ReadFileA(EJ_TOP, &EJFHeader, sizeof(EJF_HEADER), &ulActualBytesRead);
		EJ_CloseFile(hsEJFileHandle);                             /* Close file */

		if ((EJFHeader.ulEJFSize != (EJ_BLOCKSIZE * (ULONG)usNoOfBlock)) || ((EJFHeader.fchEJFlag & EJFG_OVERRIDE) != fchFlag)) {
			PifDeleteFile(stEJFiles[i].aszFileName);           /* Delete Old file, if file is exist. */
			EJ_ResetDesc();                           /* Delight EJ Full Descriptor */
		}
	}

	hsEJFileHandle = PIF_FILE_INVALID_HANDLE;
	return EJ_PERFORM;
}

/*
*==========================================================================
**    Synopsis:    SHORT   EJCheckC(USHORT usNoOfBlock, UCHAR fchFlag)
*
*       Input:    USHORT  usNoOfBlock - Number of E.J. Block
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  EJ_PERFORM
*          Abnormal End:  EJ_NO_MAKE_FILE
*
**  Description:
*               Create Electronic Journal File. If block size is 0, or EJ file
*                never exist.
*==========================================================================
*/
SHORT   EJCheckC(USHORT usNoOfBlock, UCHAR fchFlag)
{
	SHORT        i;
	SHORT        sRet = EJ_PERFORM;

	for(i = 0; stEJFiles[i].aszFileName; i++)
	{
		ULONG        ulActSize;
		ULONG        ulActMove;
		SHORT        sReturnST;

		if ((stEJFiles[i].phsHandle = PifOpenFile(stEJFiles[i].aszFileName, auchOLD_FILE_READ)) >= 0) {     /* File Exist */
			EJ_CloseFile(stEJFiles[i].phsHandle);                             /* Close file */
			continue;
		}

		if (0 == usNoOfBlock) {         /* if size is zero, do nothing */
			continue;
		}

		/* Open EJ File (New) */
		if ((stEJFiles[i].phsHandle = PifOpenFile(stEJFiles[i].aszFileName, auchNEW_FILE_WRITE)) < 0) {  /* Error case */
			PifLog(MODULE_EJ, FAULT_ERROR_FILE_OPEN);
			PifLog(MODULE_ERROR_NO(MODULE_EJ), (USHORT)abs(stEJFiles[i].phsHandle));
			PifLog(MODULE_LINE_NO(MODULE_EJ), (USHORT) __LINE__);
			PifAbort(MODULE_EJ, FAULT_ERROR_FILE_OPEN);
		}

		EJ_ResetDesc();                           /* Delight EJ Full Descriptor */

		/* Check Number of Block */
		if (usNoOfBlock < FLEX_EJ_MIN) {
			usNoOfBlock = FLEX_EJ_MIN;
		}

		ulActSize = EJ_BLOCKSIZE * (ULONG)usNoOfBlock;      /* Calculate File Size */
		sReturnST = PifSeekFile(stEJFiles[i].phsHandle, ulActSize, &ulActMove);
		if ((sReturnST < 0) || (ulActMove != ulActSize)) {  /* Can not make file */
			EJ_CloseFile(stEJFiles[i].phsHandle);                             /* Close file */
			PifDeleteFile(aszEJFileName);
			sRet = EJ_NO_MAKE_FILE;
		} else {
			EJF_HEADER   EJFHeader;

			hsEJFileHandle = stEJFiles[i].phsHandle;

			/* Make file header */
			EJInitFileHeader (&EJFHeader);
			EJFHeader.fchEJFlag = fchFlag;
			EJFHeader.ulEJFSize = ulActSize;                    /* File size at Creation */

			/* Write File Header */
			EJ_WriteFile(EJ_TOP, &EJFHeader, sizeof(EJF_HEADER), hsEJFileHandle);   /* Write File Header */
			EJ_CloseFile(hsEJFileHandle);                             /* Close file */
			PifLog(MODULE_EJ, LOG_EVENT_EJ_FCHECK);
		}
	}

	hsEJFileHandle = PIF_FILE_INVALID_HANDLE;
	return sRet;
}

/*
*==========================================================================
**    Synopsis:    SHORT   EJClear(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  EJ_PERFORM
*          Abnormal End:  EJ_FILE_NOT_EXIST
*
**  Description:
*               Reset Electronic Journal File.
*==========================================================================
*/
SHORT   EJClear(VOID)
{
	ULONG	    ulActualBytesRead; //RPH 11-10-3 SR#201
    EJF_HEADER  EJFHeader;

    EJ_ReqSem();                           /* Get Semaphore */

    /* File not exist case, if error EJ_OpenFile() will release semaphore */
    if (EJ_OpenFile() != EJ_PERFORM) {
        return (EJ_FILE_NOT_EXIST);
    }

	//RPH 11-10-3 SR#201
    EJ_ReadFileA(EJ_TOP, &EJFHeader, sizeof(EJF_HEADER), &ulActualBytesRead);
	EJInitFileHeader (&EJFHeader);
    EJFHeader.fchEJFlag &= EJFG_OVERRIDE;               /* Reset flags */

    /* Write File Header */
    EJ_WriteFile(EJ_TOP, &EJFHeader, sizeof(EJF_HEADER),hsEJFileHandle);   /* Write File Header */
    EJ_ResetDesc();                                 /* Delight EJ Full Descriptor */

#if 0
	// We have introduced a new tcsActionType for the EJENTRYACTION Connection Engine Interface XML
	// to perform a clear or delete of part or all of the Connection Engine Interface Electronic Journal.
	// We no long perform the delete of the file when the standard Electronic Journal is cleared.
	// March 20, 2013 Rel 2.2.1.84 for VCS.
	// Clear and reset the new Connection Engine Electronic Journal
	{
		TCHAR      *szConnEngineElectronicJournal = _T("TOTALEJG");  // name of the Connection Engine EJ file to delete
		PifDeleteFile (szConnEngineElectronicJournal);    // Clear the Connection Engine EJ by deleting it.  ConnEngineObjectSendMessage() uses this file.
	}
#endif

    EJ_CloseAndRelSem();                            /* Close file and Rel. Sem */
    return (EJ_PERFORM);
}

/*
*==========================================================================
*   Synopsis:  SHORT    EJWrite(EJT_HEADER *pCHeader, USHORT usWSize,
*                           UCHAR *pBuff, UCHAR uchType, UCHAR uchOption)
*       Input:    EJT_HEADER *pCHeader - pointer to structure for EJT_HEADER
*            :    USHORT     usWsize - write data size in bytes
*            :    UCHAR      *pBuff - pointer to write data buffer
*            :    UCHAR      uchType - Type of EJ File
*            :    UCHAR      uchOption - (0/1: Error Message is Once)
*       Output:   Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End: EJ_PERFORM
*          Abnormal End: EJ_FILE_NOT_EXIST
*                      : EJ_DATA_TOO_LARGE
*                      : EJ_FILE_FULL
*                      : EJ_FILE_NEAR_FULL
*
**  Description:
*               Write data to EJ File.
*==========================================================================
*/
SHORT    EJWrite(EJT_HEADER *pCHeader, USHORT usWSize, UCHAR *pBuff, UCHAR uchType, UCHAR uchOption)
{
	SHORT sRetStatus;

	EJ_ReqSem();                           /* Get Semaphore */

	sRetStatus = EJ_Write (pCHeader, usWSize, pBuff, uchType, uchOption);

    EJ_RelSem();

	return sRetStatus;
 
}

/*
*==========================================================================
**    Synopsis:    SHORT   EJReverse(EJ_READ *pEJRead, UCHAR *pBuff, UCHAR uchType)
*
*       Input:    EJ_READ    *pEJRead - Pointer of structure for EJ_READ
*            :    UCHAR      *pBuff - pointer to write data buffer
*            :    UCHAR      uchType - Type of EJ File
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End: EJ_PERFORM
*          Abnormal End: EJ_FILE_NOT_EXIST
*                      : EJ_NOTHING_DATA
*
**  Description:
*               Read Last E.J. Report Data from the EJ File. R3.1
*==========================================================================
*/
SHORT   EJReverse(EJ_READ *pEJRead, WCHAR *pBuff, UCHAR uchType)
{
    EJF_HEADER  FHeader;
    EJT_HEADER  THeader;
    USHORT      usRLen;
    USHORT      usRSize;
    UCHAR       fchFlag;
    USHORT      usReadSize, usRemindLen;
    ULONG       ulPrevPoint;
	ULONG		ulActualBytesRead;//RPH 11-10-3 SR#201

    EJ_ReqSem();                           /* Get Semaphore */

    /* File not exist case, if error EJ_OpenFile() will release semaphore */
    if (EJ_OpenFile() != EJ_PERFORM) {
        return (EJ_FILE_NOT_EXIST);
    }

    //RPH 11-10-3 SR#201
	EJ_ReadFileA(EJ_TOP, &FHeader, sizeof(EJF_HEADER), &ulActualBytesRead);
    fchFlag = FHeader.fchEJFlag;
    fchFlag &= (EJFG_OVERRIDE | EJFG_FULL | EJFG_NEAR_FULL);
    if (FHeader.ulPrevPoint == 0L) {            /* No data case */
        EJ_CloseAndRelSem();                            /* Close file and Rel. Sem */
        return (EJ_NOTHING_DATA);
    }

    if (uchType == MINOR_EJ_NEW) {              /* First transaction */
        //RPH 11-10-3 SR#201
		EJ_ReadFileA(FHeader.ulPrevPoint, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
        usRLen = EJ_ReadPrtCtl(SLIP_REVJLEN_ADR) * (EJ_COLUMN * sizeof(TCHAR));
        usRemindLen = usRLen;
        usReadSize  = (THeader.usCVLI - sizeof(EJT_HEADER));
        ulPrevPoint = FHeader.ulPrevPoint;

        if ((usRLen > usReadSize) &&
            (ulPrevPoint > sizeof(EJF_HEADER))) {

            /* ---- read ej file from end to reverse line ---- */
            for (;;) {
                ulPrevPoint -= (ULONG)THeader.usPVLI;
                //RPH 11-10-3 SR#201
				EJ_ReadFileA(ulPrevPoint, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
                if ((usRLen < (usReadSize + THeader.usCVLI - sizeof(EJT_HEADER))) ||
                    (ulPrevPoint <= sizeof(EJF_HEADER)) || THeader.usPVLI < sizeof(EJT_HEADER))
				{
                    usRemindLen -= usReadSize;
                    usReadSize += (THeader.usCVLI - sizeof(EJT_HEADER));
                    break;
                }
                usReadSize += (THeader.usCVLI - sizeof(EJT_HEADER));
            }
        }
        pEJRead->ulLeftReport = ulPrevPoint;
        if (usRLen > usReadSize) {
            pEJRead->sLeftOffset = sizeof(EJT_HEADER);
            pEJRead->usLine = usReadSize;
        } else {
            pEJRead->sLeftOffset = THeader.usCVLI - usRemindLen;
            pEJRead->usLine = usRLen;
        }
    } else {
        //RPH 11-10-3 SR#201
		EJ_ReadFileA(pEJRead->ulLeftReport, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
    }

    if ((THeader.usCVLI - pEJRead->sLeftOffset) >= pEJRead->usSize) {
        usRSize = pEJRead->usSize;
    } else {
        if ((SHORT)pEJRead->usLine > 0) {
            /* ---- read next ej block ---- */
            pEJRead->ulLeftReport += THeader.usCVLI;
            pEJRead->sLeftOffset = sizeof(EJT_HEADER);
            //RPH 11-10-3 SR#201
			EJ_ReadFileA(pEJRead->ulLeftReport, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);

            if ((THeader.usCVLI - pEJRead->sLeftOffset) >= pEJRead->usSize) {
                usRSize = pEJRead->usSize;
            } else {
                usRSize = THeader.usCVLI - pEJRead->sLeftOffset;
            }
        } else {
            usRSize = THeader.usCVLI - pEJRead->sLeftOffset;
            fchFlag |= (EJFG_PAGE_END | EJFG_FINAL_PAGE);
        }
    }
    if ((SHORT)pEJRead->usLine > 0) {
        EJ_ReadFileA(pEJRead->ulLeftReport + pEJRead->sLeftOffset, pBuff, usRSize, &ulActualBytesRead);
    }
    pEJRead->usLine -= usRSize;
    pEJRead->sLeftOffset += usRSize;
    pEJRead->fchFlag = fchFlag;
    EJ_CloseAndRelSem();                            /* Close file and Rel. Sem */
    return (EJ_PERFORM);
}

/*
*==========================================================================
*   Synopsis:  SHORT    EJRead(EJ_READ *pEJRead, UCHAR *pBuff, UCHAR uchType)
*
*       Input:    EJ_READ    *pEJRead - Pointer of structure for EJ_READ
*            :    UCHAR      *pBuff - pointer to read data buffer
*            :    UCHAR      uchType - Type of EJ File
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End: sLeftLen
*          Abnormal End: EJ_FILE_NOT_EXIST
*                      : EJ_NOTHING_DATA
*
**  Description:
*               Read E.J. Report Data from the EJ File.
*==========================================================================
*/
SHORT    EJRead(EJ_READ *pEJRead, UCHAR *pBuff, UCHAR uchType)
{
    EJF_HEADER  FHeader;
    SHORT       sLeftLen = 0;
    SHORT       sRightLen = 0;
    USHORT      usLine;
	ULONG		ulActualBytesRead; //RPH 11-10-3 SR#201

    EJ_ReqSem();                           /* Get Semaphore */

    /* File not exist case, if error EJ_OpenFile() will release semaphore */
    if (EJ_OpenFile() != EJ_PERFORM) {
        return (EJ_FILE_NOT_EXIST);
    }

    //RPH 11-10-3 SR#201
	EJ_ReadFileA(EJ_TOP, &FHeader, sizeof(EJF_HEADER), &ulActualBytesRead);
    if (FHeader.ulPrevPoint == 0L) {            /* No data case */
        EJ_CloseAndRelSem();                            /* Close file and Rel. Sem */
        return (EJ_NOTHING_DATA);
    }

    fchEJFlag &= (EJFG_ADJ_LINE | EJFG_FINAL_PAGE | EJFG_RPAGE_END);
    if (uchType == MINOR_EJ_NEW) {                          /* First transaction */
        fchEJFlag &= ~(EJFG_FINAL_PAGE | EJFG_ADJ_LINE);    /* Reset final page and Adj. flag */
        pEJRead->ulLeftReport = FHeader.ulBeginPoint;
        pEJRead->sLeftOffset = sizeof(EJT_HEADER);
        pEJRead->ulLLastReport = FHeader.ulBeginPoint;
        pEJRead->sLLastOffset = sizeof(EJT_HEADER);
        pEJRead->ulRightReport = FHeader.ulBeginPoint;
        pEJRead->sRightOffset = sizeof(EJT_HEADER);
        pEJRead->ulRLastReport = FHeader.ulBeginPoint;
        pEJRead->sRLastOffset = sizeof(EJT_HEADER);
    }
    if ((pEJRead->ulRightReport == pEJRead->ulRLastReport)
        && (pEJRead->sRightOffset == pEJRead->sRLastOffset)
        && !(fchEJFlag & EJFG_RPAGE_END)) {                 /* 9/23/93 */
        usLine = EJ_Set_End(&FHeader, pEJRead);
        pEJRead->usLine = usLine;
    }
    sLeftLen = EJ_Set_Left(&FHeader, pEJRead, pBuff);

    if (!(fchEJFlag & EJFG_RPAGE_END)) {
        sRightLen = EJ_Set_Right(&FHeader, pEJRead, pBuff);
    }

    if ((fchEJFlag & EJFG_ADJ_LINE) && (fchEJFlag & EJFG_PAGE_END)) {
        if(sLeftLen != (sRightLen + 1)) {
#ifdef DEBUG
            PifAbort(MODULE_EJ, 90);
#else
            /* PifLog(MODULE_EJ, LOG_ERROR_EJ_UNMATCH1); */
            fchEJFlag |= EJFG_FINAL_PAGE;
            sLeftLen = 0;
            sRightLen = 0;
#endif
        }
    } else {
        if(sLeftLen != sRightLen) {
#ifdef DEBUG
            PifAbort(MODULE_EJ, 80);
#else
            /* PifLog(MODULE_EJ, LOG_ERROR_EJ_UNMATCH2); */
            fchEJFlag |= (EJFG_FINAL_PAGE | EJFG_PAGE_END);
            sLeftLen = 0;
            sRightLen = 0;
#endif
        }
    }

    if (fchEJFlag & EJFG_PAGE_END) {
        fchEJFlag &= ~EJFG_RPAGE_END;
    }

    EJ_CloseFile(hsEJFileHandle);                            /* Close file */
	hsEJFileHandle = PIF_FILE_INVALID_HANDLE;
    pEJRead->fchFlag = fchEJFlag;

    /* Calculate Number of Page */
    pEJRead->usPage = (FHeader.usTtlLine / (2 * EJ_ReadPrtCtl(SLIP_EJLEN_ADR)));
    if ((FHeader.usTtlLine % (2 * EJ_ReadPrtCtl(SLIP_EJLEN_ADR)))) {
        pEJRead->usPage += 1;
    }

    EJ_RelSem();
    return (sLeftLen);
}

/*
*==========================================================================
*   Synopsis:  UCHAR    EJGetFlag(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :  FHeader.fchEJFlag Data
*
**  Description:
*               Return EJ File status flag
*==========================================================================
*/
UCHAR    EJGetFlag(VOID)
{
    EJF_HEADER  FHeader;
	ULONG	ulActualBytesRead;	//RPH 11-10-3 SR#201

    EJ_ReqSem();                           /* Get Semaphore */

    /* File not exist case, if error EJ_OpenFile() will release semaphore */
    if (EJ_OpenFile() != EJ_PERFORM) {
        return (EJFG_NO_FILE);
    }

    //RPH 11-10-3 SR#201
	EJ_ReadFileA(EJ_TOP, &FHeader, sizeof(EJF_HEADER), &ulActualBytesRead);
    EJ_CloseAndRelSem();                    /* Close file and Rel. Sem */
    return(FHeader.fchEJFlag);              /* Return Status Flags */
}

/*
*===========================================================================
** Synopsis:    USHORT     EJConvertError(SHORT sError);
*
*      Input:    SHORT      sError - EJ Internal Error
*      Output:   Nothing
*       InOut:   Nothing
*
** Return:      Leadthru No
*
** Description: This function converts error code to leadthru number.
*===========================================================================
*/
USHORT   EJConvertError(SHORT sError)
{
    USHORT  usLeadthruNo = LDT_ERR_ADR;

    switch ( sError ) {
//  case EJ_CONTINUE:                   /* Success  0 for EJReadResetTrans() more data to read */
    case EJ_END:                        /* Success -1 for EJReadResetTrans() end of data */
    case EJ_EOF:                        /* Success -120 for EJReadFile() end of data */
    case EJ_PERFORM:                    /* Success 0 */
        usLeadthruNo = 0;
        break;

    case EJ_FILE_NEAR_FULL:
        usLeadthruNo = LDT_NEARFULL_ADR;    /* 25 * E.J is Near Full */
        break;

    case EJ_FILE_FULL:                  /* Error -100 */
        usLeadthruNo = LDT_FULL_ADR;    /* 26 * E.J is File Full */
        break;

    case EJ_NOTHING_DATA:               /* Error -104 */
        usLeadthruNo = LDT_NTINFL_ADR;  /* 1 * Not In File */
        break;

    case EJ_NO_MAKE_FILE:               /* Error -10 */
        usLeadthruNo = LDT_FLFUL_ADR;   /* 2 * fill full */
        break;

    case EJ_FILE_NOT_EXIST:             /* Error -101 */
    case EJ_DATA_TOO_LARGE:             /* Error -103 */
        usLeadthruNo = LDT_EJREAD_ADR;  /* 112 * E/J Read error */
        break;

    case EJ_TOO_SMALL_BUFF:             /* Error -11 */
    case EJ_PROHIBIT:                   /* Error -105 */
    case EJ_OVERRIDE_STOP:              /* Error -109 */
        usLeadthruNo = LDT_PROHBT_ADR;  /* 10 * Prohibit operation */
        break;

    default:
        usLeadthruNo = LDT_ERR_ADR;     /* 21 * Error(Global) */
        break;
    }

	if (usLeadthruNo != 0) {
		PifLog (MODULE_EJ, LOG_ERROR_EJ_CONVERTERROR);
		PifLog (MODULE_ERROR_NO(MODULE_EJ), (USHORT)(sError * (-1)));
		PifLog (MODULE_ERROR_NO(MODULE_EJ), usLeadthruNo);
	}

    return (usLeadthruNo);
}

/*
*==========================================================================
*   Synopsis:  SHORT    EJReadFile(ULONG ulOffset, VOID *pData, ULONG ulDataLen,
*                                  ULONG *pulRetLen)
*
*       Input:    ULONG      ulOffset - Read Pointer of EJ File
*            :    ULONG     ulDataLen - Read Buffer Length
*       InOut:    ULONG     *ulRetLen - Pointer of Readed Data Length
*            :    VOID       *pData - Pointer of Buffer
*
**  Return    :
*            Normal End: EJ_PERFORM
*                      : EJ_EOF
*          Abnormal End: EJ_FILE_NOT_EXIST
*                      : EJ_DATA_TOO_LARGE
*
**  Description:
*               Read E.J. File Data from the EJ File.
*==========================================================================
*/
SHORT   EJReadFile(ULONG ulOffset, VOID *pData, ULONG ulDataLen, ULONG *pulRetLen)
{
    ULONG       ulLength1 = 0;
    ULONG       ulLength2 = 0;
    ULONG       ulRelOffset;
    ULONG       ulDataSize;
    ULONG       ulActPos;
	ULONG		ulActualBytesRead;	//RPH 11-10-3 SR#201
    EJF_HEADER  FHeader;

    EJ_ReqSem();                           /* Get Semaphore */

    /* Initialize */
    *pulRetLen = 0;

    /* File not exist case, if error EJ_OpenFile() will release semaphore */
    if (EJ_OpenFile() != EJ_PERFORM) {
        return (EJ_FILE_NOT_EXIST);
    }

    /* Check buffer length */
    if (ulDataLen < sizeof(EJT_HEADER)) {
        EJ_CloseAndRelSem();                    /* Close file and Rel. Sem */
        return (EJ_PERFORM);
    }

    //RPH 11-10-3 SR#201
	EJ_ReadFileA(EJ_TOP, &FHeader, sizeof(EJF_HEADER), &ulActualBytesRead);
    if (FHeader.ulPrevPoint == 0L) {            /* No data case */
        EJ_CloseAndRelSem();                    /* Close file and Rel. Sem */
		memset (pData, 0, sizeof(EJT_HEADER));
		((EJT_HEADER *)pData)->usCVLI = sizeof(EJT_HEADER);
		((EJT_HEADER *)pData)->usEjSignature = EJT_HEADER_SIGNATURE;
        return (EJ_EOF);
    }

    /* Modify read file pointer in order to position in the Electronic Journal file
	 * an offset that is relative to where the data begins.  The beginning point
	 * of the data is at FHeader.ulBeginPoint and it continues and may wrap around
	 * since the Electronic Journal file is a circular buffer.
	 *
	 * The ulOffset specified by the caller is a relative position from the beginning
	 * of the data area which starts at FHeader.ulBeginPoint so if the last file
	 * write casued a wrap to the physical beginning of the file so that some of the
	 * data is at the physical beginning of the file, then we need to accomodate that.
	 *
	 * Some applications, PCSample for instance, may not process more than a single EJ Entry
	 * even if the returned buffer contains more than one.  So they will ignore the EJ_EOF
	 * until such time as the number of bytes read is zero so we have to handle that.
	 */
    ulRelOffset = ulOffset;
    ulRelOffset += FHeader.ulBeginPoint;

	if (ulRelOffset == FHeader.ulNextPoint) {
        EJ_CloseAndRelSem();                    /* Close file and Rel. Sem */
		memset (pData, 0, sizeof(EJT_HEADER));
		((EJT_HEADER *)pData)->usCVLI = sizeof(EJT_HEADER);
		((EJT_HEADER *)pData)->usEjSignature = EJT_HEADER_SIGNATURE;
        return (EJ_EOF);
	}

    /* Check end of file, next pointer and adjust read data */
    if (ulRelOffset >= FHeader.ulEndofPoint) {
		// Adjust the absolute offset so that it wraps around to the
		// physical beginning of the file.
        ulRelOffset -= FHeader.ulEndofPoint;
        ulRelOffset += sizeof(EJF_HEADER);

		if (ulRelOffset == FHeader.ulNextPoint) {
			// we have wrapped around and we have reached the end of the usable data.
			EJ_CloseAndRelSem();                    /* Close file and Rel. Sem */
			memset (pData, 0, sizeof(EJT_HEADER));
			((EJT_HEADER *)pData)->usCVLI = sizeof(EJT_HEADER);
			((EJT_HEADER *)pData)->usEjSignature = EJT_HEADER_SIGNATURE;
			return (EJ_EOF);
		}
    }

    /* Check read data size is overlapped between end of file and begin of file */
    if ((ulRelOffset > FHeader.ulNextPoint) && ((ulDataSize = FHeader.ulEndofPoint - ulRelOffset) < ulDataLen)) {

        /* Seek file */
        if ((PifSeekFile(hsEJFileHandle, ulRelOffset, &ulActPos)) != PIF_OK) {
            EJ_CloseAndRelSem();                            /* Close file and Rel. Sem */
			memset (pData, 0, sizeof(EJT_HEADER));
			((EJT_HEADER *)pData)->usCVLI = sizeof(EJT_HEADER);
			((EJT_HEADER *)pData)->usEjSignature = EJT_HEADER_SIGNATURE;
            return (EJ_DATA_TOO_LARGE);
        }

        /* Read EJ data */
		//RPH 11-7-3 Changes for PifReadFile
        PifReadFile(hsEJFileHandle, pData, ulDataSize, &ulLength1);
        *pulRetLen = ulLength1;

        /* Seek file to top */
        if ((PifSeekFile(hsEJFileHandle, ( ULONG)sizeof(EJF_HEADER), &ulActPos)) != PIF_OK) {
            EJ_CloseAndRelSem();                            /* Close file and Rel. Sem */
            return (EJ_DATA_TOO_LARGE);
        }

        ulRelOffset = ( ULONG)sizeof(EJF_HEADER);
        ulDataLen -= ulLength1;
    }

    if (ulRelOffset < FHeader.ulNextPoint) {
        if ((ulDataSize = FHeader.ulNextPoint - ulRelOffset) > ulDataLen) {
            ulDataSize = ulDataLen;
        }

    } else {
        if ((ulDataSize = FHeader.ulEndofPoint - ulRelOffset) > ulDataLen) {
            ulDataSize = ulDataLen;
        }
    }

    /* Seek file */
    if ((PifSeekFile(hsEJFileHandle, ulRelOffset, &ulActPos)) != PIF_OK) {
        EJ_CloseAndRelSem();                            /* Close file and Rel. Sem */
        return (EJ_DATA_TOO_LARGE);
    }

    /* Read EJ data */
	//RPH 11-7-3 Changes For PifReadFile
    PifReadFile(hsEJFileHandle, (( UCHAR*)pData + ulLength1), ulDataSize, &ulLength2);
    *pulRetLen += ulLength2;

    /* Advance file pointer */
    ulRelOffset += ulLength2;

    EJ_CloseFile(hsEJFileHandle);                            /* Close file */
	hsEJFileHandle = PIF_FILE_INVALID_HANDLE;

    if (ulRelOffset == FHeader.ulNextPoint) {   /* EJ data end */
        EJ_RelSem();
        return (EJ_EOF);
    }

    EJ_RelSem();
    return (EJ_PERFORM);
}


/*
*==========================================================================
*   Synopsis:  VOID    EJ_W_Close(EJF_HEADER *pFHeader, USHORT usWSize, UCHAR *pBuff)
*
*       Input:    EJF_HEADER *pFHrader - Pointer of structure for EJ_HEADER
*            :    USHORT     usWize - Write data size
*            :    UCHAR      *pBuff - pointer to write data buffer
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*               Write data to EJ File
*==========================================================================
*/
VOID    EJ_W_Close(EJF_HEADER *pFHeader, USHORT usWSize, UCHAR  *pBuff)
{
	NHPOS_ASSERT_TEXT((hsEJFileHandle >= 0), "**WARNING: EJ_W_Close() file handle.");

	if (pFHeader && pBuff) {
		EJ_WriteFile(pFHeader->ulNextPoint, pBuff, usWSize, hsEJFileHandle);
		pFHeader->ulPrevPoint = pFHeader->ulNextPoint;
		pFHeader->ulNextPoint += usWSize;
		if (pFHeader->ulNextPoint > pFHeader->ulEndofPoint) {
			pFHeader->ulEndofPoint = pFHeader->ulNextPoint;
		}
		EJ_WriteFile(EJ_TOP, pFHeader, sizeof(EJF_HEADER),hsEJFileHandle);
	}
    EJ_CloseFile(hsEJFileHandle);                             /* Close file */
	hsEJFileHandle = PIF_FILE_INVALID_HANDLE;
}
/*
*==========================================================================
*   Synopsis: USHORT EJ_Set_End(EJF_HEADER *pFHeader, EJ_READ *pEJRead)
*
*       Input:    EJF_HEADER   *pFHeader - Pointer to structure for EJF_HEADER
*                 EJ_READ      *pEJRead - Pointer to structure for EJ_READ
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   usR_line
*
**  Description:
*               Set one portion of EJ ASCII data from EJ file
*==========================================================================
*/
USHORT   EJ_Set_End(EJF_HEADER *pFHeader, EJ_READ *pEJRead)
{
    SHORT       sOffset;
    USHORT      usR_Line;
    USHORT      usTL_Line = 0;
    ULONG       ulPoint;
	ULONG		ulActualBytesRead;//RPH 11-10-3 SR#201
    EJT_HEADER  THeader;

    pEJRead->ulLeftReport = pEJRead->ulRLastReport;
    pEJRead->sLeftOffset = pEJRead->sRLastOffset;
    ulPoint = pEJRead->ulLeftReport;
    sOffset = pEJRead->sLeftOffset;
    usR_Line = 2 * EJ_ReadPrtCtl(SLIP_EJLEN_ADR);
    for (;;) {
        /** EJ_ReadFileA(pEJRead->ulLeftReport, &THeader, sizeof(EJT_HEADER)); **/
        //RPH 11-10-3 SR#201
		EJ_ReadFileA(ulPoint, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
        usTL_Line += (THeader.usCVLI - sOffset) / (EJ_COLUMN * sizeof(TCHAR));
        if (usTL_Line > usR_Line) {
            pEJRead->ulRLastReport = ulPoint;
            pEJRead->sRLastOffset = THeader.usCVLI
                                    - (usTL_Line - usR_Line ) * (EJ_COLUMN * sizeof(TCHAR));
            break;
        }
        sOffset = sizeof(EJT_HEADER);
        ulPoint += THeader.usCVLI;
        if (ulPoint != pFHeader->ulNextPoint) {
            if (ulPoint >= pFHeader->ulEndofPoint) {
                ulPoint = sizeof(EJF_HEADER);
            }
            if (ulPoint != pFHeader->ulNextPoint) {
                continue;
            } else {
                fchEJFlag |= EJFG_FINAL_PAGE;
            }
        } else {
            fchEJFlag |= EJFG_FINAL_PAGE;
        }
        pEJRead->ulRLastReport = ulPoint;
        pEJRead->sRLastOffset = sizeof(EJT_HEADER);
        if (usTL_Line % 2) {
            pEJRead->fchFlag |= EJFG_ADJ_LINE;
            fchEJFlag |= EJFG_ADJ_LINE;
            usR_Line = usTL_Line + 1;
        } else {
            usR_Line = usTL_Line;
        }
        break;
    }
    usR_Line /= 2;
    usTL_Line = 0;
    ulPoint = pEJRead->ulLeftReport;
    sOffset = pEJRead->sLeftOffset;
    for (;;) {
        /** EJ_ReadFileA(pEJRead->ulLeftReport, &THeader, sizeof(EJT_HEADER)); **/
        //RPH 11-10-3 SR#201
		EJ_ReadFileA(ulPoint, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
        usTL_Line += (THeader.usCVLI - sOffset) / (EJ_COLUMN * sizeof(TCHAR));
        if (usTL_Line > usR_Line) {
            pEJRead->ulLLastReport = pEJRead->ulRightReport = ulPoint;
            pEJRead->sLLastOffset = pEJRead->sRightOffset =
                    THeader.usCVLI - (usTL_Line - usR_Line ) * (EJ_COLUMN * sizeof(TCHAR));
            return (usR_Line);
        }
        sOffset = sizeof(EJT_HEADER);
        ulPoint += THeader.usCVLI;
        if (ulPoint != pFHeader->ulNextPoint) {
            if (ulPoint >= pFHeader->ulEndofPoint) {
                ulPoint = sizeof(EJF_HEADER);
            }
            if (ulPoint != pFHeader->ulNextPoint) {
                continue;
            }
        }
        pEJRead->ulLLastReport = pEJRead->ulRightReport = ulPoint;
        pEJRead->sLLastOffset = pEJRead->sRightOffset = sizeof(EJT_HEADER);
        return (usR_Line);
    }
}

/*
*==========================================================================
*   Synopsis: USHORT EJ_Set_Left(EJF_HEADER *pFHeader, EJ_READ *pEJRead, UCHAR *pBuff)
*
*       Input:    EJF_HEADER *pFHeader - Pointer to structure for EJF_HEADER
*                 EJ_READ    *pEJRead - Pointer to structure for EJ_READ
*                 UCHAR      *pBuff - Pointer to read data buffer
*      Output:    Nothing
*       InOut:    ASCII DATA into *pBuff
*
**  Return    :   Nothing
*
**  Description:
*               Set one portion of EJ ASCII data from EJ file
*==========================================================================
*/
USHORT   EJ_Set_Left(EJF_HEADER *pFHeader, EJ_READ *pEJRead, UCHAR *pBuff)
{
    USHORT  usR_Line;
    USHORT  usB_Line;
    USHORT  usT_Line;
    USHORT  usCo = 0;
    EJT_HEADER  THeader;
	ULONG	ulActualBytesRead;//RPH 11-10-3 SR#201

    usR_Line = pEJRead->usLine;
    usB_Line = pEJRead->usSize / (EJ_REPORT_WIDE*sizeof(TCHAR)); //ESMITH EJ
    for (;;) {
        //RPH 11-10-3 SR#201
		EJ_ReadFileA(pEJRead->ulLeftReport, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
        if (pEJRead->ulLeftReport == pEJRead->ulLLastReport ) {
            usT_Line = pEJRead->sLLastOffset - pEJRead->sLeftOffset;
            if (usT_Line) {
                usT_Line /= (EJ_COLUMN * sizeof(TCHAR));
            } else {
                fchEJFlag |= EJFG_PAGE_END;
                return (usCo);
            }
        } else {
            usT_Line = (THeader.usCVLI - pEJRead->sLeftOffset) / (EJ_COLUMN * sizeof(TCHAR));
        }
        if (usB_Line < (usR_Line - usCo)) {
            if (usB_Line < usT_Line) {
                EJ_Set_1Tr(usB_Line,
                           pEJRead->ulLeftReport + pEJRead->sLeftOffset,
                           pBuff + usCo * EJ_REPORT_WIDE);
                pEJRead->sLeftOffset += usB_Line * (EJ_COLUMN * sizeof(TCHAR));
                return (usCo + usB_Line);
            }
        } else {
            if ((usR_Line - usCo) < usT_Line) {
                EJ_Set_1Tr((USHORT)(usR_Line - usCo),
                           pEJRead->ulLeftReport + pEJRead->sLeftOffset,
                           pBuff + usCo * EJ_REPORT_WIDE);
                pEJRead->sLeftOffset += (usR_Line - usCo) * (EJ_COLUMN * sizeof(TCHAR));
                return (usR_Line);
            }
        }
        EJ_Set_1Tr(usT_Line,
                   pEJRead->ulLeftReport + pEJRead->sLeftOffset,
                   pBuff + usCo * EJ_REPORT_WIDE);
        usCo += usT_Line;
        usB_Line -= usT_Line;
        if (pEJRead->ulLeftReport == pEJRead->ulLLastReport ) {
            pEJRead->sLeftOffset += usT_Line * (EJ_COLUMN * sizeof(TCHAR));
            fchEJFlag |= EJFG_PAGE_END;
            return (usCo);
        }

        pEJRead->ulLeftReport += THeader.usCVLI;
        if (pEJRead->ulLeftReport == pFHeader->ulNextPoint) {
            fchEJFlag |= EJFG_PAGE_END;
            return (usCo);
        }

        if (pEJRead->ulLeftReport >= pFHeader->ulEndofPoint) {
            /* pEJRead->ulLeftReport = pFHeader->ulBeginPoint; */
            pEJRead->ulLeftReport = sizeof(EJF_HEADER);
        }
        pEJRead->sLeftOffset = sizeof(EJT_HEADER);
        if ((pEJRead->ulLeftReport == pEJRead->ulLLastReport) &&
            (pEJRead->sLeftOffset == pEJRead->sLLastOffset)) {
            fchEJFlag |= EJFG_PAGE_END;
            return (usCo);
        }
    }
}

/*
*==========================================================================
*   Synopsis: USHORT EJ_Set_Right(EJF_HEADER *pFHeader, EJ_READ *pEJRead, UCHAR *pBuff)
*
*       Input:    EJF_HEADER *pFHeader - Pointer to structure for EJF_HEADER
*                 EJ_READ    *pEJRead - Pointer to structure for EJ_READ
*                 UCHAR      *pBuff -  pointer to read data buffer
*      Output:    Nothing
*       InOut:    ASCII DATA into *pBuff
*
**  Return    :   Nothing
*
**  Description:
*               Set one portion of EJ ASCII data from EJ file
*==========================================================================
*/
USHORT   EJ_Set_Right(EJF_HEADER *pFHeader, EJ_READ *pEJRead, UCHAR *pBuff)
{
    USHORT  usR_Line;
    USHORT  usB_Line;
    USHORT  usT_Line;
    USHORT  usCo = 0;
    EJT_HEADER  THeader;
	ULONG	ulActualBytesRead;//RPH 11-10-3 SR#201

    usR_Line = pEJRead->usLine;
    if (pEJRead->fchFlag & EJFG_ADJ_LINE) {
        usR_Line--;
    }
    usB_Line = pEJRead->usSize / (EJ_REPORT_WIDE *sizeof(TCHAR)); //ESMITH EJ
    for (;;) {
        //RPH 11-10-3 SR#201
		EJ_ReadFileA(pEJRead->ulRightReport, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
        if (pEJRead->ulRightReport == pEJRead->ulRLastReport ) {
            usT_Line = pEJRead->sRLastOffset - pEJRead->sRightOffset;
            if (usT_Line) {
                usT_Line /= (EJ_COLUMN * sizeof(TCHAR));
            } else {
                fchEJFlag |= EJFG_RPAGE_END;
                return (usCo);
            }
        } else {
            usT_Line = (THeader.usCVLI - pEJRead->sRightOffset) / (EJ_COLUMN * sizeof(TCHAR));
        }
        if (usB_Line < (usR_Line - usCo)) {
            if (usB_Line < usT_Line) {
                EJ_Set_1Tr(usB_Line,
                           pEJRead->ulRightReport + pEJRead->sRightOffset,
                           pBuff + usCo * EJ_REPORT_WIDE + (EJ_COLUMN * sizeof(TCHAR)));//RPH?????(EJ_COLUMN * sizeof(TCHAR))
                pEJRead->sRightOffset += usB_Line * (EJ_COLUMN * sizeof(TCHAR));
                return (usCo + usB_Line);
            }
        } else {
            if ((usR_Line - usCo) < usT_Line) {
                EJ_Set_1Tr((USHORT)(usR_Line - usCo),
                           pEJRead->ulRightReport + pEJRead->sRightOffset,
                           pBuff + usCo * EJ_REPORT_WIDE + (EJ_COLUMN * sizeof(TCHAR)));//RPH???(EJ_COLUMN * sizeof(TCHAR))
                pEJRead->sRightOffset += (usR_Line - usCo) * (EJ_COLUMN * sizeof(TCHAR));
                return (usR_Line);
            }
        }
        EJ_Set_1Tr(usT_Line,
                   pEJRead->ulRightReport + pEJRead->sRightOffset,
                   pBuff + usCo * EJ_REPORT_WIDE + (EJ_COLUMN * sizeof(TCHAR)));//RPH ????(EJ_COLUMN * sizeof(TCHAR))
        usCo += usT_Line;
        usB_Line -= usT_Line;
        if (pEJRead->ulRightReport == pEJRead->ulRLastReport ) {
            pEJRead->sRightOffset += usT_Line * (EJ_COLUMN * sizeof(TCHAR));
            fchEJFlag |= EJFG_RPAGE_END;
            return (usCo);
        }

        pEJRead->ulRightReport += THeader.usCVLI;
        if (pEJRead->ulRightReport == pFHeader->ulNextPoint) {
            fchEJFlag |= EJFG_RPAGE_END;
            return (usCo);
        }

        if (pEJRead->ulRightReport >= pFHeader->ulEndofPoint) {
            /* pEJRead->ulRightReport = pFHeader->ulBeginPoint; */
            pEJRead->ulRightReport = sizeof(EJF_HEADER);
        }
        pEJRead->sRightOffset = sizeof(EJT_HEADER);
        if ((pEJRead->ulRightReport == pEJRead->ulRLastReport) &&
            (pEJRead->sRightOffset == pEJRead->sRLastOffset)) {
            fchEJFlag |= EJFG_RPAGE_END;
            return (usCo);
        }
    }
}

/*
*==========================================================================
*   Synopsis: VOID  EJ_Set_1Tr(USHORT usLine, ULONG ulOffset, UCHAR *pBuff)
*
*       Input:    USHORT     usLine - No. of Llines to read.
*                 ULONG      ulOffset - Offset for read point
*                 UCHAR      *pBuff - Pointer to write data buffer
*      Output:    Nothing
*       InOut:    ASCII DATA into *pBuff
*
**  Return    :   Nothing
*
**  Description:
*               Set one portion of EJ ASCII data from EJ file
*==========================================================================
*/
VOID   EJ_Set_1Tr(USHORT usLine, ULONG ulOffset, UCHAR *pBuff)
{
	ULONG	ulActualBytesRead;//RPH 11-10-3 SR#201
    for (; usLine > 0; usLine--) {
        EJ_ReadFileA(ulOffset, pBuff, (EJ_COLUMN * sizeof(TCHAR)), &ulActualBytesRead);
        //pBuff += EJ_REPORT_WIDE;
		pBuff += ulActualBytesRead; //ESMITH EJ
        ulOffset += (EJ_COLUMN * sizeof(TCHAR));
    }
}
/*
*==========================================================================
*   Synopsis: VOID EJ_DelBlk(EJF_HEADER *pFHeader, SHORT sSize)
*
*       Input:    EJF_HEADER *pFHeader - Pointer to structure for EJ_HEADER
*                 SHORT      sSize - Needed Size.
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*               Delete old data.
*==========================================================================
*/
SHORT   EJ_DelBlk(EJF_HEADER *pFHeader, SHORT sSize)
{
	EJT_HEADER  THeader;
	ULONG	    ulActualBytesRead;//RPH 11-10-3 SR#201

    if (pFHeader->ulEndofPoint == sizeof(EJF_HEADER)) {
		// file is empty since the end of the in use data area equals the
		// offset past the EJ file header.
        return EJ_PERFORM;
    }
    while(sSize > 0) {
        //RPH 11-10-3 SR#201
		EJ_ReadFileA(pFHeader->ulBeginPoint, &THeader, sizeof(THeader), &ulActualBytesRead);

		// Perform a consistency check on the EJ block header to detect corrupted EJ files
		if (THeader.usEjSignature != EJT_HEADER_SIGNATURE) {
			NHPOS_ASSERT_TEXT(0, "**WARNING EJ_DelBlk(): possible corrupt EJ file. usEjSignature != EJT_HEADER_SIGNATURE.");
		}

		if (THeader.usCVLI < sizeof(EJT_HEADER)) {
			NHPOS_ASSERT_TEXT(0, "**ERROR EJ_DelBlk(): corrupt EJ file. usCVLI range error.");
			PifLog(MODULE_EJ, FAULT_ERROR_FILE_READ);
			PifLog(MODULE_LINE_NO(MODULE_EJ), (USHORT)__LINE__);
			PifAbort(MODULE_EJ, FAULT_ERROR_FILE_READ);
			exit(1);   // force an exit to prevent zombie process
		} else if (THeader.usSeqNo >= _T(' ')) {
			NHPOS_ASSERT_TEXT((THeader.usSeqNo < _T(' ')), "**WARNING EJ_DelBlk(): Possible corrupt EJ file. usSeqNo >= _T(' ').");
			PifLog(MODULE_EJ, FAULT_ERROR_FILE_READ);
			PifLog(MODULE_LINE_NO(MODULE_EJ), (USHORT)__LINE__);
		}
        pFHeader->ulBeginPoint += THeader.usCVLI;
        sSize -= THeader.usCVLI;
        /* --- Calculate Total Line --- */
        pFHeader->usTtlLine -= ((THeader.usCVLI - sizeof(EJT_HEADER)) / (EJ_COLUMN * sizeof(TCHAR)));
        if (pFHeader->ulBeginPoint >= pFHeader->ulEndofPoint) {
			// reached end of file so lets wrap around back to the beginning.
			// the end point is now where we are going to put the next Electronic Journal entry.
			// this does assume that the file size is large enough for at least two entries.
			pFHeader->ulBeginPoint = sizeof(EJF_HEADER);
			pFHeader->ulEndofPoint = pFHeader->ulNextPoint;
        }
    }

    return EJ_PERFORM;
}

/*
*==========================================================================
*   Synopsis: VOID EJ_WriteFile(ULONG ulOffset, VOID *pData, USHORT usSize)
*
*       Input:    ULONG      ulOffset - Number of bytes from the biginning of the plu file.
*                 VOID       *pData - Pointer to write data buffer.
*                 USHORT     usSize - Number of bytes to be written.
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*               Write data to EJ file.
*==========================================================================
*/
VOID   EJ_WriteFile(ULONG ulOffset, VOID *pData, USHORT usSize, SHORT hsFileHandle)
{
    ULONG   ulActPos;

    if (usSize == 0) {      /* check writed size */
        return ;            /* if nothing, return */
    }
	if (ulOffset == EJ_TOP) {
		static  UCHAR fchEJFlag_Save = 0;
		EJF_HEADER *pEJFHeader = (EJF_HEADER *)pData;

		if (pEJFHeader->fchEJFlag != fchEJFlag_Save) {
			char xBuff[128];
			sprintf (xBuff, "==NOTE: EJ_WriteFile() pEJFHeader->fchEJFlag 0x%x -> 0x%x", fchEJFlag_Save, pEJFHeader->fchEJFlag);
			NHPOS_NONASSERT_TEXT(xBuff);
			fchEJFlag_Save = pEJFHeader->fchEJFlag;
		}
	}
    if ((PifSeekFile(hsFileHandle, ulOffset, &ulActPos)) != PIF_OK) {
        PifAbort(MODULE_EJ, FAULT_ERROR_FILE_SEEK);
    }
    PifWriteFile(hsFileHandle, pData, usSize);
}

/*
*==========================================================================
*   Synopsis: USHORT EJ_ReadFileA(ULONG ulOffset, VOID *pData, ULONG ulSize,
									ULONG *pulActualBytesRead)
*
*      Input:     ULONG      ulOffset - Start offset value of EJ file.
*                 ULONG     usSize - Number of bytes to be read.
				  ULONG		*pulActualBytesRead - Number of Bytes Read
*      Output:    Nothing
*       InOut:    VOID       *pData - Pointer to read data buffer.
*
**  Return   :    EJ_NO_READ_SIZE
					EJ_PERFORM
*
**  Description:
*               Read data from EJ file with length check.
	RPH 11-7-3 Changes for PifReadFile
	the size read is passed by pulActualBytesRead
*==========================================================================
*/
#if defined(EJ_ReadFileA)
SHORT   EJ_ReadFileA_Debug(ULONG ulOffset, VOID *pData, ULONG ulSize, ULONG *pulActualBytesRead, char *pPath, int iLineNo)
#else
SHORT    EJ_ReadFileA(ULONG ulOffset, VOID *pData, ULONG ulSize, ULONG *pulActualBytesRead)
#endif
{
    ULONG   ulActPos;
    //USHORT  usLength;
	//SHORT	sReturn;

    if (ulSize == 0) {             /* check read buffer size */
        return EJ_NO_READ_SIZE;    /* if read nothing, return */
    }

	if (hsEJFileHandle < 0) {
		PifLog (MODULE_ERROR_NO(MODULE_EJ), FAULT_ERROR_FILE_READ);
		PifLog (MODULE_LINE_NO(MODULE_EJ), (USHORT)__LINE__);
		{
#if defined(EJ_ReadFileA)
			int  iLen;
#endif
			char xBuff[128];
			sprintf (xBuff, "==NOTE: EJ_ReadFileA() Read Error handle %d  ulOffset %d  ulSize %d.", hsEJFileHandle, ulOffset, ulSize);
			NHPOS_ASSERT_TEXT((hsEJFileHandle >= 0), xBuff);
#if defined(EJ_ReadFileA)
			iLen = strlen (pPath);
			if (iLen > 32)
				iLen -= 32;
			else
				iLen = 0;
			sprintf (xBuff, "==NOTE: EJ_ReadFileA() Called from %s  line %d.", pPath+iLen, iLineNo);
			NHPOS_ASSERT_TEXT((hsEJFileHandle >= 0), xBuff);
#endif
		}
        PifAbort(MODULE_EJ, FAULT_ERROR_FILE_READ);    /* if error, then abort */
	}

    if ((PifSeekFile(hsEJFileHandle, ulOffset, &ulActPos)) != PIF_OK) {
        PifAbort(MODULE_EJ, FAULT_ERROR_FILE_SEEK);    /* if error, then abort */
    }
    PifReadFile(hsEJFileHandle, pData, ulSize, pulActualBytesRead);
    if ( *pulActualBytesRead != ulSize) {              /* if not enough length,*/
		PifLog (MODULE_ERROR_NO(MODULE_EJ), FAULT_ERROR_FILE_READ);
		PifLog (MODULE_LINE_NO(MODULE_EJ), (USHORT)__LINE__);
		{
#if defined(EJ_ReadFileA)
			int  iLen;
#endif
			char xBuff[128];
			sprintf (xBuff, "==NOTE: EJ_ReadFileA() Read Error ulOffset %d  ulSize %d  actual %d.", ulOffset, ulSize, *pulActualBytesRead);
			NHPOS_ASSERT_TEXT((*pulActualBytesRead == ulSize), xBuff);
#if defined(EJ_ReadFileA)
			iLen = strlen (pPath);
			if (iLen > 32)
				iLen -= 32;
			else
				iLen = 0;
			sprintf (xBuff, "==NOTE: EJ_ReadFileA() Called from %s  line %d.", pPath+iLen, iLineNo);
			NHPOS_ASSERT_TEXT((*pulActualBytesRead == ulSize), xBuff);
#endif
		}
        PifAbort(MODULE_EJ, FAULT_ERROR_FILE_READ);    /* then abort */
    }
    return EJ_PERFORM;
}

/*
*==========================================================================
*   Synopsis:  UCHAR    EJ_ReadPrtCtl(UCHAR    uchAddress)
*
*      Input:    UCHAR       uchAddress - Address of Prog.18
*                                           SLIP_EJLEN_ADR    (4)
*                                           SLIP_REVJLEN_ADR  (5)
*                                           SLIP_NEARFULL_ADR (6)
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :  Programmed Value (UCHAR)
*
**  Description:
*               Read EJ Control data in Prog. # 18
*==========================================================================
*/
UCHAR    EJ_ReadPrtCtl(UCHAR    uchAddress)
{
    PARASLIPFEEDCTL    EJCtl;

    EJCtl.uchMajorClass = CLASS_PARASLIPFEEDCTL;        /* Set Major Class */
    EJCtl.uchAddress = uchAddress;                      /* Set Access ADR  */
    CliParaRead(&EJCtl);                                /* Read Data */

    /* Check 0 Data */
    if ((uchAddress == SLIP_EJLEN_ADR) && (EJCtl.uchFeedCtl < EJ_EJLEN_MIN)) {    /* Length of E.J is 0 ~ 20 */
        EJCtl.uchFeedCtl = SLIP_EJLEN_DEF;
    }

    if ((uchAddress == SLIP_REVJLEN_ADR) && (EJCtl.uchFeedCtl == 0)) {  /* Length of Reverse E.J is 0 */
        EJCtl.uchFeedCtl = SLIP_REVJLEN_DEF;
    }

    return(EJCtl.uchFeedCtl);                           /* Return with data */
}

/*
*==========================================================================
*   Synopsis: VOID EJ_SetDesc( VOID )
*
*      Input:   Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*
**  Description:
*               Light EJ Full Descriptor
*==========================================================================
*/
VOID    EJ_SetDesc( VOID )
{
    return; /* not light */

    if (( PifSysConfig()->uchOperType == DISP_2X20 ) || ( PifSysConfig()->uchOperType == DISP_LCD )) {  /* R3.0 */
        UieDescriptor(UIE_OPER, EJ_2X20_PRTOFFLINE, UIE_DESC_ON);
    }
}

/*
*==========================================================================
*   Synopsis: VOID EJ_ResetDesc( VOID )
*
*      Input:   Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*
**  Description:
*               Delight EJ Full Descriptor
*==========================================================================
*/
VOID    EJ_ResetDesc( VOID )
{
    return; /* not light */

    if (( PifSysConfig()->uchOperType == DISP_2X20 ) || ( PifSysConfig()->uchOperType == DISP_LCD )) {  /* R3.0 */
        UieDescriptor(UIE_OPER, EJ_2X20_PRTOFFLINE, UIE_DESC_OFF);
    }
}

/*
*==========================================================================
*   Synopsis: SHORT EJ_OpenFile( VOID )
*
*      Input:   Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return
*            Normal End:  EJ_PERFORM
*          Abnormal End:  EJ_FILE_NOT_EXIST
*
**  Description:
*               Open EJ File
*==========================================================================
*/
SHORT   EJ_OpenFile( VOID )
{
	if (hsEJFileHandle >= 0) {
		NHPOS_ASSERT_TEXT((hsEJFileHandle < 0), "==WARNING: EJ_OpenFile() called and file already open.");
	}

	if(!uchMaintDelayBalanceFlag)
	{
		if ((hsEJFileHandle = PifOpenFile(aszEJFileName, auchOLD_FILE_READ_WRITE)) < 0) {
			char  xBuff[128];

			EJ_RelSem();
			sprintf (xBuff, "**ERROR: EJ_OpenFile() File Open error %d. EJ_FILE_NOT_EXIST no DelayBal.", hsEJFileHandle);
			NHPOS_ASSERT_TEXT((hsEJFileHandle >= 0), xBuff);
			PifLog(MODULE_EJ, FAULT_ERROR_FILE_OPEN);
			PifLog(MODULE_DATA_VALUE(MODULE_EJ), (USHORT)uchMaintDelayBalanceFlag);
			PifLog(MODULE_ERROR_NO(MODULE_EJ), (USHORT)abs(hsEJFileHandle));
			PifLog(MODULE_LINE_NO(MODULE_EJ), (USHORT)__LINE__);
			return (EJ_FILE_NOT_EXIST);
		}
	}else
	{
		if ((hsEJFileHandle = PifOpenFile(aszEJDBFileName, auchOLD_FILE_READ_WRITE)) < 0) {
			char  xBuff[128];

			EJ_RelSem();
			sprintf (xBuff, "**ERROR: EJ_OpenFile() File Open error %d. EJ_FILE_NOT_EXIST DelayBal.", hsEJFileHandle);
			NHPOS_ASSERT_TEXT((hsEJFileHandle >= 0), xBuff);
			PifLog(MODULE_EJ, FAULT_ERROR_FILE_OPEN);
			PifLog(MODULE_DATA_VALUE(MODULE_EJ), (USHORT)uchMaintDelayBalanceFlag);
			PifLog(MODULE_ERROR_NO(MODULE_EJ), (USHORT)abs(hsEJFileHandle));
			PifLog(MODULE_LINE_NO(MODULE_EJ), (USHORT)__LINE__);
			return (EJ_FILE_NOT_EXIST);
		}
	}
    return (EJ_PERFORM);
}
/*
*==========================================================================
*   Synopsis: VOID EJ_CloseFile( VOID )
*
*      Input:   Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*
**  Description:
*               Close EJ File
*==========================================================================
*/
VOID EJ_CloseFile( SHORT sFileHandle )
{
	if (sFileHandle >= 0) {
		PifCloseFile(sFileHandle);                           /* Close file then delete it */
	} else {
		char xBuff[128];
		sprintf (xBuff, "**WARNING: EJ_CloseFile() sFileHandle = %d.", sFileHandle);
		NHPOS_ASSERT_TEXT((sFileHandle >= 0), xBuff);
	}
}
/*
*==========================================================================
*   Synopsis: VOID EJ_CloseAndRelSem( VOID )
*
*      Input:   Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*
**  Description:
*               Close EJ File and Realese Semaphore
*==========================================================================
*/
VOID EJ_CloseAndRelSem( VOID )
{
	if (hsEJFileHandle >= 0) {
		EJ_CloseFile(hsEJFileHandle);           /* Close file then delete it */
	} else {
		char xBuff[128];
		sprintf (xBuff, "**WARNING: EJ_CloseAndRelSem() hsEJFileHandle = %d.", hsEJFileHandle);
		NHPOS_ASSERT_TEXT((hsEJFileHandle >= 0), xBuff);
	}
	hsEJFileHandle = PIF_ERROR_SYSTEM;          // provide way to determine if file has been closed.
    EJ_RelSem();                                /* Release Semaphore */
}

VOID EJ_CloseAndRelSemIsp( VOID )
{
	if (hsEJFileHandle >= 0) {
		EJ_CloseFile(hsEJFileHandle);           /* Close file then delete it */
	} else {
		char xBuff[128];
		sprintf (xBuff, "**WARNING: EJ_CloseAndRelSemIsp() hsEJFileHandle = %d.", hsEJFileHandle);
		NHPOS_ASSERT_TEXT((hsEJFileHandle >= 0), xBuff);
	}
	hsEJFileHandle = PIF_ERROR_SYSTEM;          // provide way to determine if file has been closed.
    EJ_RelSemIsp();                                /* Release Semaphore */
}

/*
*==========================================================================
*   Synopsis: VOID EJ_ReqSem( VOID )
*
*      Input:   Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*
**  Description:
*               Request Semaphore
*==========================================================================
*/
static struct {
	char aszFilePath[42];
	int  nLineNo;
	int  nType;
} EJ_ReqSemPoint = { 0 };

#if defined(EJ_ReqSem)
VOID  EJ_ReqSem_Debug (char *aszFilePath, int nLineNo)
{
	PifRequestSem(husEJSem);                /* Get Semaphore */

	{
		strncpy (EJ_ReqSemPoint.aszFilePath, RflPathChop(aszFilePath), 32);
		EJ_ReqSemPoint.nLineNo = nLineNo;
		EJ_ReqSemPoint.nType = 1;
	}
}
#else
VOID EJ_ReqSem( VOID )
{
    PifRequestSem(husEJSem);                /* Get Semaphore */
}
#endif

#if defined(EJ_ReqSemIsp)
VOID  EJ_ReqSemIsp_Debug (char *aszFilePath, int nLineNo)
{
	if ((usIspEJState & ISP_ST_EJ) != 0) {
		char xBuff[256];

		sprintf (xBuff, "**NOTE: EJ_ReqSemIsp_Debug() usIspEJState 0x%x EJ_ReqSemIsp_Debug called from %s %d ", usIspEJState, EJ_ReqSemPoint.aszFilePath, EJ_ReqSemPoint.nLineNo);
		NHPOS_ASSERT_TEXT(!xBuff, xBuff);
	}
	PifRequestSem(husEJSem);                /* Get Semaphore */
	{
		strncpy (EJ_ReqSemPoint.aszFilePath, RflPathChop(aszFilePath), 32);
		EJ_ReqSemPoint.nLineNo = nLineNo;
		EJ_ReqSemPoint.nType = 2;
	}
    usIspEJState |= ISP_ST_EJ;    /* EJ_ReqSemIsp_Debug() mark semaphore indicator used by ISP thread for multi-messages */
}
#else
VOID EJ_ReqSemIsp( VOID )
{
    PifRequestSem(husEJSem);                /* Get Semaphore */
    usIspEJState |= ISP_ST_EJ;    /* EJ_ReqSemIsp() mark semaphore indicator used by ISP thread for multi-messages */
}
#endif

/*
*==========================================================================
*   Synopsis: VOID EJ_RelSem( VOID )
*
*      Input:   Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*
**  Description:
*               Realese Semaphore
*==========================================================================
*/
#if defined(EJ_ReqSemIsp)
VOID  EJ_RelSem_Debug (char *aszFilePath, int nLineNo)
{
    PifReleaseSem(husEJSem);       /* Release Semaphore */
}
#else
VOID EJ_RelSem( VOID )
{
    PifReleaseSem(husEJSem);       /* Release Semaphore */
}
#endif

#if defined(EJ_ReqSemIsp)
VOID  EJ_RelSemIsp_Debug (char *aszFilePath, int nLineNo)
{
	if ((usIspEJState & ISP_ST_EJ) == 0) {
		char xBuff[256];

		sprintf (xBuff, "**NOTE: EJ_RelSemIsp_Debug() usIspEJState 0x%x EJ_ReqSem_Debug called from %s %d ", usIspEJState, EJ_ReqSemPoint.aszFilePath, EJ_ReqSemPoint.nLineNo);
		NHPOS_ASSERT_TEXT(!xBuff, xBuff);
	}

    usIspEJState &= ~ISP_ST_EJ;    /* EJ_RelSemIsp_Debug() clear semaphore indicator used by ISP thread for multi-messages */
    PifReleaseSem(husEJSem);       /* Release Semaphore */
}
#else
VOID EJ_RelSemIsp( VOID )
{
    usIspEJState &= ~ISP_ST_EJ;    /* EJ_RelSemIsp() clear semaphore indicator used by ISP thread for multi-messages */
    PifReleaseSem(husEJSem);       /* Release Semaphore */
}
#endif

/*
*==========================================================================
*   Synopsis:  SHORT    EJRead1Line(EJ_READ *pEJRead, UCHAR *pBuff, UCHAR uchType)
*
*       Input:    EJ_READ    *pEJRead - Pointer of structure for EJ_READ
*            :    UCHAR      *pBuff - pointer to read data buffer
*            :    UCHAR      uchType - Type of EJ File
*      Output:    Nothing
*       InOut:    Nothing
**  Return    :
*            Normal End: sLeftLen
*          Abnormal End: EJ_FILE_NOT_EXIST
*                      : EJ_NOTHING_DATA
**  Description:    Read E.J. 1 Line Report Data from the EJ File.
*==========================================================================
*/
SHORT    EJRead1Line(EJ_READ *pEJRead, UCHAR *pBuff, UCHAR uchType)
{
	ULONG	    ulActualBytesRead;//RPH 11-10-3 SR#201
    EJF_HEADER  FHeader;
    EJT_HEADER  THeader;

    EJ_ReqSem();

    /* File not exist case, if error EJ_OpenFile() will release semaphore */
    if (EJ_OpenFile() != EJ_PERFORM) {
        return (EJ_FILE_NOT_EXIST);
    }
    //RPH 11-10-3 SR#201
	EJ_ReadFileA(EJ_TOP, &FHeader, sizeof(EJF_HEADER), &ulActualBytesRead);
    if (FHeader.ulPrevPoint == 0L) {                    /* No data case */
        EJ_CloseAndRelSem();                            /* Close file and Rel. Sem */
        return (EJ_NOTHING_DATA);
    }
    if (uchType == MINOR_EJ_NEW) {                      /* First transaction */
        pEJRead->ulLeftReport = FHeader.ulBeginPoint;
        pEJRead->sLeftOffset = sizeof(EJT_HEADER);
        pEJRead->fchFlag = 0;
    }

    //RPH 11-10-3 SR#201
	EJ_ReadFileA(pEJRead->ulLeftReport, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
    //RPH 11-10-3 SR#201
	EJ_ReadFileA(pEJRead->ulLeftReport + pEJRead->sLeftOffset,
                 pBuff, EJ_COLUMN * sizeof(TCHAR), &ulActualBytesRead);             /* Read 1 line *///RPH TCHARS may be two bytes EJCOLUMN is not large enough
	NHPOS_ASSERT(ulActualBytesRead <= 0x7fff);//MAXSHORT);
    pEJRead->sLeftOffset += (SHORT)ulActualBytesRead/*EJ_COLUMN*/;
    if (THeader.usCVLI == (USHORT)pEJRead->sLeftOffset) {
        pEJRead->ulLeftReport += THeader.usCVLI;
        pEJRead->sLeftOffset = sizeof(EJT_HEADER);
        if (pEJRead->ulLeftReport == FHeader.ulNextPoint) {
            pEJRead->fchFlag = EJFG_FINAL_PAGE | EJFG_PAGE_END;
        } else if (pEJRead->ulLeftReport >= FHeader.ulEndofPoint) {
            pEJRead->ulLeftReport = sizeof(EJF_HEADER);
        }
    }
    EJ_CloseFile(hsEJFileHandle);
	hsEJFileHandle = PIF_FILE_INVALID_HANDLE;
    EJ_RelSem();

    return(EJ_PERFORM);
}

/*
*==========================================================================
*   Synopsis:  SHORT    EJNoOfTrans(VOID)
*
*       Input:  Nothing
*      Output:  Nothing
*       InOut:  Nothing
**  Return   :  > 0;    No of Transaction
*            :  <= 0;   Error Status
*
**  Description:    Return No of Transaction in E/J File.       R2.0GCA
*==========================================================================
*/
SHORT   EJNoOfTrans(VOID)
{
    ULONG       ulReadPoint;
	ULONG	    ulActualBytesRead;//RPH 11-10-3 SR#201
    SHORT       sCo;
    EJF_HEADER  FHeader;
    EJT_HEADER  THeader;

    EJ_ReqSem();

    /* File not exist case, if error EJ_OpenFile() will release semaphore */
    if (EJ_OpenFile() != EJ_PERFORM) {
        return(EJ_FILE_NOT_EXIST);
    }
    //RPH 11-10-3 SR#201
	EJ_ReadFileA(EJ_TOP, &FHeader, sizeof(EJF_HEADER), &ulActualBytesRead);
    if (FHeader.ulPrevPoint == 0L) {                    /* No data case */
        EJ_CloseAndRelSem();                            /* Close file and Rel. Sem */
        return(EJ_NOTHING_DATA);
    }

    for (sCo = 1, ulReadPoint = FHeader.ulBeginPoint; ; sCo++) {
		ULONG  ulReadPointLast = ulReadPoint;

		//RPH 11-10-3 SR#201
		EJ_ReadFileA(ulReadPoint, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
        ulReadPoint += THeader.usCVLI;
        if (ulReadPointLast <= FHeader.ulNextPoint && ulReadPoint >= FHeader.ulNextPoint) {
			// handle the possibility that the EJ file has a problem with bad usCVLI and check
			// that we have passed the ulNextPoint with the data from this read.
            break;
        } else if (ulReadPoint >= FHeader.ulEndofPoint) {
            ulReadPoint = sizeof(EJF_HEADER);
        }
    }
    EJ_CloseFile(hsEJFileHandle);
	hsEJFileHandle = PIF_FILE_INVALID_HANDLE;
    EJ_RelSem();

    return(sCo);
}

/*
*==========================================================================
*   Synopsis:   SHORT   EJReadResetTransOver(ULONG *pulOffset,
*                           VOID *pData, USHORT usDataLen, USHORT *pusRetLen)
*            :  ULONG       *pulOffset  - Read Pointer of EJ File
*            :  USHORT      usDataLen   - Read Buffer Length
*       InOut:  USHORT      *usRetLen   - Pointer of Readed Data Length
*            :  VOID        *pData      - Pointer of Buffer
**  Return   :  Normal End: # of Transaction
*                         : EJ_END
*             Abnormal End: EJ_FILE_NOT_EXIST
*                         : EJ_NOTHING_DATA
*                         : EJ_PROHIBIT
**  Description:    Read/Reset Trans. Data in E/J File(Override Type).  R2.0GCA
*==========================================================================
*/
SHORT   EJReadResetTransOver(ULONG *pulOffset,
                        VOID *pData, ULONG ulDataLen, ULONG *pulRetLen)
{
    ULONG       usCo, ulReadPoint;
	ULONG	    ulActualBytesRead;//RPH 11-10-3 SR#201
    EJF_HEADER  FHeader;
    EJT_HEADER  THeader;
    SHORT       sRet = 0;
    USHORT      i, usTtlLine;

    if (*pulOffset == 0L) {
        EJ_ReqSemIsp();
    }

    /* File not exist case, if error EJ_OpenFile() will release semaphore */
    if (EJ_OpenFile() != EJ_PERFORM) {
		char  xBuff[128];

		sprintf (xBuff, "**ERROR: EJReadResetTransOver() EJ_FILE_NOT_EXIST File Open error %d.", hsEJFileHandle);
		NHPOS_ASSERT_TEXT((hsEJFileHandle >= 0), xBuff);
		*pulOffset = 0L;
		*pulRetLen = 0L;
		usIspEJState &= ~ISP_ST_EJ;    /* EJReadResetTransOver() clear semaphore indicator used by ISP thread for multi-messages */
        return(EJ_FILE_NOT_EXIST);
    }

    //RPH 11-10-3 SR#201
	EJ_ReadFileA(EJ_TOP, &FHeader, sizeof(EJF_HEADER), &ulActualBytesRead);

    if( !(FHeader.fchEJFlag & EJFG_OVERRIDE) ) {
        EJ_CloseAndRelSemIsp();                    /* Close file and Rel. Sem */
		*pulOffset = 0L;
		*pulRetLen = 0L;
		NHPOS_NONASSERT_TEXT("**WARNING: EJReadResetTransOver() EJ_OVERRIDE_STOP !(FHeader.fchEJFlag & EJFG_OVERRIDE).");
        return( EJ_OVERRIDE_STOP );
    }

    if (FHeader.ulPrevPoint == 0L) {            /* No data case */
        EJ_CloseAndRelSemIsp();                    /* Close file and Rel. Sem */
		NHPOS_NONASSERT_TEXT("==NOTE: EJReadResetTransOver() EJ_END No data. EJ_CloseAndRelSem() (FHeader.ulPrevPoint == 0L).");
		*pulOffset = 0L;
		*pulRetLen = 0L;
        return( EJ_END );
    }
    /* --- Read Full/Partial Transaction --- */
    usCo = EJReadTrans(&FHeader, pulOffset, pData, ulDataLen, pulRetLen);

    /* --- Erase Already Read Trans.in E/J(Proceed Begin Pointer) --- */
    if (0 < usCo ) {
        sRet = (SHORT)usCo;
        ulReadPoint = FHeader.ulBeginPoint;

        for (i = usTtlLine = 0; i < usCo; i++) {
			ULONG  ulReadPointLast = ulReadPoint;

            //RPH 11-10-3 SR#201
			EJ_ReadFileA(ulReadPoint, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
            ulReadPoint += THeader.usCVLI;
            usTtlLine += (THeader.usCVLI - sizeof(EJT_HEADER)) / (EJ_COLUMN * sizeof(TCHAR));

            if (ulReadPointLast <= FHeader.ulNextPoint && ulReadPoint >= FHeader.ulNextPoint) {
                sRet = EJ_END;
				break;
            } else if (ulReadPoint >= FHeader.ulEndofPoint) {
                ulReadPoint = sizeof(EJF_HEADER);
            }
        }
        FHeader.ulBeginPoint = ulReadPoint;
        if (sRet == EJ_END) {
			EJInitFileHeader(&FHeader);
        } else {
            FHeader.usTtlLine -= usTtlLine;
        }
        EJ_WriteFile(EJ_TOP, &FHeader, sizeof(EJF_HEADER),hsEJFileHandle);
    }
    EJ_CloseFile(hsEJFileHandle);
	hsEJFileHandle = PIF_FILE_INVALID_HANDLE;

    /* --- Send Multi Transaction Block --- */
    if (*pulOffset == 0L) {
        EJ_RelSemIsp();
    }

    return(sRet);
}

/*
*==========================================================================
*   Synopsis:   SHORT   EJReadResetTransStart(ULONG *pulOffset,
*                           VOID *pData, USHORT usDataLen,
*                           USHORT *pusRetLen, USHORT *pusNumTrans)
*            :  ULONG       *pulOffset  - Read Pointer of EJ File
*            :  USHORT      usDataLen   - Read Buffer Length
*       InOut:  USHORT      *usRetLen   - Pointer of Readed Data Length
*            :  VOID        *pData      - Pointer of Buffer
*       Out  :  USHORT      *pusNumTrans    - Number of Transaction read in buffer
*
**  Return   :  Normal End: EJ_END
*                         : EJ_CONTINUE
*             Abnormal End: EJ_NOTHING_DATA
*                         : EJ_FILE_NOT_EXIST
*                         : EJ_PROHIBIT
**  Description:    Read/Reset Trans. Data in E/J File(Not Override).   R2.0GCA
*==========================================================================
*/
SHORT   EJReadResetTransStart(ULONG *pulOffset,
                        VOID *pData, ULONG ulDataLen, ULONG *pulRetLen, USHORT *pusNumTrans)
{
    ULONG       ulReadPoint;    /* Offset for reading data */
	ULONG	    ulActualBytesRead;//RPH 11-10-3 SR#201
    USHORT      i;             /* roop counter */
    SHORT       sError;         /* retrun value */
    EJF_HEADER  FHeader;        /* EJ File Header */
    EJT_HEADER  THeader;        /* EJ Transaction Header */

    sError = EJ_CONTINUE;

    if (*pulOffset == 0L) {
        EJ_ReqSemIsp();
    }

    /* File not exist case, if error EJ_OpenFile() will release semaphore */
    if (EJ_OpenFile() != EJ_PERFORM) {
		char  xBuff[128];
		sprintf (xBuff, "**ERROR: EJReadResetTransStart() EJ_FILE_NOT_EXIST File Open error %d.", hsEJFileHandle);
		NHPOS_ASSERT_TEXT((hsEJFileHandle >= 0), xBuff);
		*pulOffset = 0L;
		*pusNumTrans = 0;
		*pulRetLen = 0;
		usIspEJState &= ~ISP_ST_EJ;    /* EJReadResetTransStart() clear semaphore indicator used by ISP thread for multi-messages */
        return(EJ_FILE_NOT_EXIST);
    }

    //RPH 11-10-3 SR#201
	EJ_ReadFileA(EJ_TOP, &FHeader, sizeof(EJF_HEADER), &ulActualBytesRead);

    if( (FHeader.fchEJFlag & EJFG_OVERRIDE) ) {
        EJ_CloseAndRelSemIsp();                    /* Close file and Rel. Sem */
		*pulOffset = 0L;
		*pusNumTrans = 0;
		*pulRetLen = 0;
		NHPOS_NONASSERT_TEXT("**WARNING: EJReadResetTransStart() EJ_OVERRIDE_STOP (FHeader.fchEJFlag & EJFG_OVERRIDE)")
        return( EJ_OVERRIDE_STOP );
    }

    if (FHeader.ulPrevPoint == 0L) {            /* No data case */
        EJ_CloseAndRelSemIsp();                    /* Close file and Rel. Sem */
		*pulOffset = 0L;
		*pusNumTrans = 0;
		*pulRetLen = 0;
        return(EJ_NOTHING_DATA);
    }

    /* Check buffer length */
    if (ulDataLen < sizeof(EJT_HEADER)) {
        EJ_CloseAndRelSemIsp();
		*pulOffset = 0L;
		*pusNumTrans = 0;
		*pulRetLen = 0;
        return( EJ_END );
    }

    /* --- Read Full/Partial Transaction --- */
    *pusNumTrans = EJReadTrans(&FHeader, pulOffset, pData, ulDataLen, pulRetLen);

    /* --- Check End of Transaction --- */
    ulReadPoint = FHeader.ulBeginPoint;
    for (i = 0; i < *pusNumTrans; i++) {
		ULONG  ulReadPointLast = ulReadPoint;

        //RPH 11-10-3 SR#201
		EJ_ReadFileA(ulReadPoint, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
        ulReadPoint += THeader.usCVLI;

        if (ulReadPointLast <= FHeader.ulNextPoint && ulReadPoint >= FHeader.ulNextPoint) {
            sError = EJ_END;
			break;
        } else if (ulReadPoint >= FHeader.ulEndofPoint) {
            ulReadPoint = sizeof(EJF_HEADER);
        }
    }

    EJ_CloseFile(hsEJFileHandle);
	hsEJFileHandle = PIF_FILE_INVALID_HANDLE;

    /* --- Send Multi Transactin Block --- */
    if (*pulOffset == 0L) {
        EJ_RelSemIsp();
    }

    return(sError);
}

/*
*==========================================================================
*   Synopsis:   SHORT   EJReadResetTrans(ULONG *pulOffset, VOID *pData,
*                           USHORT usDataLen, USHORT *pusRetLen, USHORT *pusNumTrans)
*            :  ULONG       *pulOffset  - Read Pointer of EJ File
*            :  USHORT      usDataLen   - Read Buffer Length
*       InOut:  USHORT      *usRetLen   - Pointer of Readed Data Length
*            :  VOID        *pData      - Pointer of Buffer
**  Return   :  Normal End: EJ_END
*                         : EJ_CONTINUE
*             Abnormal End: EJ_FILE_NOT_EXIST
*                         : EJ_PROHIBIT
**  Description:    Read/Reset Trans. Data in E/J File(Not Override).   R2.0GCA
*==========================================================================
*/
SHORT   EJReadResetTrans(ULONG *pulOffset, VOID *pData,
                ULONG ulDataLen, ULONG *pulRetLen, USHORT *pusNumTrans)
{
    ULONG       ulReadPoint;    /* offset for reading data */
	ULONG	    ulActualBytesRead;//RPH 11-10-3 SR#201
    EJF_HEADER  FHeader;        /* EJ File Header */
    EJT_HEADER  THeader;        /* EJ Transaction Header */
    USHORT      i;              /* loop counter */
    SHORT       sError;         /* return code */
    USHORT      usTtlLine;      /* number of lines read */

    sError = EJ_CONTINUE;

    if (*pulOffset == 0L) {
        EJ_ReqSemIsp();
    }

    /* File not exist case, if error EJ_OpenFile() will release semaphore */
    if (EJ_OpenFile() != EJ_PERFORM) {
		usIspEJState &= ~ISP_ST_EJ;    /* EJReadResetTrans() clear semaphore indicator used by ISP thread for multi-messages */
		*pulOffset = 0L;
		*pusNumTrans = 0;
		*pulRetLen = 0;
        return(EJ_FILE_NOT_EXIST);
    }

    /* Check buffer length */
    if (ulDataLen < sizeof(EJT_HEADER)) {
        EJ_CloseAndRelSemIsp();
		*pulOffset = 0L;
		*pusNumTrans = 0;
		*pulRetLen = 0;
        return( EJ_END );
    }

    //RPH 11-10-3 SR#201
	EJ_ReadFileA(EJ_TOP, &FHeader, sizeof(EJF_HEADER), &ulActualBytesRead);

    if( (FHeader.fchEJFlag & EJFG_OVERRIDE) ) {
        EJ_CloseAndRelSemIsp();
		*pulOffset = 0L;
		*pusNumTrans = 0;
		*pulRetLen = 0;
		NHPOS_NONASSERT_TEXT("**WARNING: EJReadResetTrans() EJ_OVERRIDE_STOP (FHeader.fchEJFlag & EJFG_OVERRIDE)")
        return( EJ_OVERRIDE_STOP );
    }

    /* --- Erase Previous Read Trans.in E/J(Proceed Begin Pointer) --- */
    if ( 0 < *pusNumTrans ) {

        ulReadPoint = FHeader.ulBeginPoint;
        for (i = 0, usTtlLine = 0; i < *pusNumTrans; i++) {
			ULONG  ulReadPointLast = ulReadPoint;

            //RPH 11-10-3 SR#201
			EJ_ReadFileA(ulReadPoint, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
            ulReadPoint += THeader.usCVLI;
            usTtlLine += (THeader.usCVLI - sizeof(EJT_HEADER)) / (EJ_COLUMN * sizeof(TCHAR));

            if (ulReadPointLast <= FHeader.ulNextPoint && ulReadPoint >= FHeader.ulNextPoint) {
				// handle the possibility that the EJ file has a problem with bad usCVLI and check
				// that we have passed the ulNextPoint with the data from this read.
                sError = EJ_END;
                break;
            } else if (ulReadPoint >= FHeader.ulEndofPoint) {
                ulReadPoint = sizeof(EJF_HEADER);
            }

        }
        FHeader.ulBeginPoint = ulReadPoint;
        FHeader.usTtlLine -= usTtlLine;
        if(EJ_PERFORM == EJCheckSpace(&FHeader)) {
            FHeader.fchEJFlag  &= ~(EJFG_FULL | EJFG_NEAR_FULL);
            EJ_ResetDesc();                         /* Delight EJ Full Descriptor */
        }
        EJ_WriteFile(EJ_TOP, &FHeader, sizeof(EJF_HEADER),hsEJFileHandle);
    }

    /* --- Read Full/Partial Transaction --- */
    *pusNumTrans = EJReadTrans(&FHeader, pulOffset, pData, ulDataLen, pulRetLen);

    /* --- Check End of Transaction --- */
    ulReadPoint = FHeader.ulBeginPoint;
    for (i = 0; i < *pusNumTrans; i++) {
		ULONG  ulReadPointLast = ulReadPoint;

        //RPH 11-10-3 SR#201
		EJ_ReadFileA(ulReadPoint, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
        ulReadPoint += THeader.usCVLI;

        if (ulReadPointLast <= FHeader.ulNextPoint && ulReadPoint >= FHeader.ulNextPoint) {
			// handle the possibility that the EJ file has a problem with bad usCVLI and check
			// that we have passed the ulNextPoint with the data from this read.
            sError = EJ_END;
			break;
        } else if (ulReadPoint >= FHeader.ulEndofPoint) {
            ulReadPoint = sizeof(EJF_HEADER);
        }
    }

    EJ_CloseFile(hsEJFileHandle);
	hsEJFileHandle = PIF_FILE_INVALID_HANDLE;

    /* --- Send Multi Transactin Block --- */
    if (*pulOffset == 0L) {
        EJ_RelSemIsp();
    }

    return(sError);
}

/*
*==========================================================================
*   Synopsis:   SHORT   EJReadResetTransEnd(USHORT usNumTrans)
*       InOut:  None
**  Return   :  Normal End: EJ_PERFORM
*             Abnormal End: EJ_PROHIBIT
*
**  Description:    Erase Previous Read Trans.in E/J(Proceed Begin Pointer)
*==========================================================================
*/
SHORT   EJReadResetTransEnd(USHORT usNumTrans)
{
    USHORT      i;
    USHORT      usTtlLine;
    ULONG       ulReadPoint;
    EJF_HEADER  FHeader;
    EJT_HEADER  THeader;
	ULONG	ulActualBytesRead;//RPH 11-10-3 SR#201

    EJ_ReqSem();

    /* File not exist case, if error EJ_OpenFile() will release semaphore */
    if (EJ_OpenFile() != EJ_PERFORM) {
        return(EJ_FILE_NOT_EXIST);
    }

    /* --- Erase Previous Read Trans.in E/J(Proceed Begin Pointer) --- */
    //RPH 11-10-3 SR#201
	EJ_ReadFileA(EJ_TOP, &FHeader, sizeof(EJF_HEADER), &ulActualBytesRead);

    if( (FHeader.fchEJFlag & EJFG_OVERRIDE) ) {
        EJ_CloseAndRelSem();                    /* Close file and Rel. Sem */
		NHPOS_NONASSERT_TEXT("**WARNING: EJReadResetTransEnd() EJ_OVERRIDE_STOP (FHeader.fchEJFlag & EJFG_OVERRIDE)")
        return( EJ_OVERRIDE_STOP );
    }

    ulReadPoint = FHeader.ulBeginPoint;
    for (i = 0, usTtlLine = 0; i < usNumTrans; i++) {
		ULONG  ulReadPointLast = ulReadPoint;

        //RPH 11-10-3 SR#201
		EJ_ReadFileA(ulReadPoint, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
        ulReadPoint += THeader.usCVLI;
        usTtlLine += (THeader.usCVLI - sizeof(EJT_HEADER)) / (EJ_COLUMN * sizeof(TCHAR));

        if (ulReadPointLast <= FHeader.ulNextPoint && ulReadPoint >= FHeader.ulNextPoint) {
			// handle the possibility that the EJ file has a problem with bad usCVLI and check
			// that we have passed the ulNextPoint with the data from this read.
            break;
        } else if (ulReadPoint >= FHeader.ulEndofPoint) {
            ulReadPoint = sizeof(EJF_HEADER);
        }

    }
    FHeader.ulBeginPoint = ulReadPoint;
    FHeader.usTtlLine -= usTtlLine;

    if (ulReadPoint == FHeader.ulNextPoint) {
		EJInitFileHeader(&FHeader);
    }

    if(EJ_PERFORM == EJCheckSpace(&FHeader)) {
        FHeader.fchEJFlag  &= ~(EJFG_FULL | EJFG_NEAR_FULL);
        EJ_ResetDesc();                         /* Delight EJ Full Descriptor */
    }

    EJ_WriteFile(EJ_TOP, &FHeader, sizeof(EJF_HEADER),hsEJFileHandle);

    EJ_CloseFile(hsEJFileHandle);
	hsEJFileHandle = PIF_FILE_INVALID_HANDLE;
    EJ_RelSem();

    return(EJ_PERFORM);
}

/*
*==========================================================================
*   Synopsis:   USHORT  EJReadTrans(ULONG *pulOffset,
*                           VOID *pData, ULONG ulDataLen, ULONG *pulRetLen)
*            :  ULONG       *pulOffset  - Read Pointer of EJ File
*                                         (Offset in a Transaction)
*            :  ULONG      ulDataLen   - Read Buffer Length
*       InOut:  ULONG      *ulRetLen   - Pointer of Read Data Length
*            :  VOID        *pData      - Pointer of Buffer
**  Return   :  Normal End: # of Transaction
*                           '0' means NOT ok to Read Full Transaction
**  Description:    Read 1 Transaction Data in E/J File.        R2.0GCA
*					Should only be used as part of the reset process.
*==========================================================================
*/
USHORT  EJReadTrans(EJF_HEADER *pFHeader, ULONG *pulOffset, VOID *pData, ULONG ulDataLen, ULONG *pulRetLen)
{
    ULONG       ulReadPoint;
	ULONG	    ulActualBytesRead;
    USHORT      usCo;
    EJT_HEADER  THeader;

	// The purpose of this function is to read as many Electronic Journal entries into the provided
	// memory buffer as possible.  We will return a count of the number transactions actually in the buffer.
	// Large Electronic Journal entries may be spread across several messages.  The idea is to update the
	// Electronic Journal file header as each complete Electronic Journal entry is read.  The caller provides
	// a pointer to an offset within the current Electronic Journal entry to show how much of the current
	// entry has already been provided.
	//
	//    THeader.usCVLI contains the length in bytes of the current Electronic Journal entry
	//    pulOffset contains a pointer to the offset within the current Electronic Journal entry data
	//    pulRetLen contains a pointer to the number of bytes of Electronic Journal entry data being returned
	//    ulDataLen contains the maximum length of the data buffer into which to copy the Electronic Journal entry data
	//    usCo is the number of complete Electronic Journal entries have been processed.
    ulReadPoint = pFHeader->ulBeginPoint;
    usCo = 0;  *pulRetLen = 0;
	do {
		ULONG  ulBytesToRead = 0;

        /* --- This Is The End --- */
        if (ulReadPoint == pFHeader->ulNextPoint) {
            break;
        } else if (ulReadPoint >= pFHeader->ulEndofPoint) {
			/* --- Return to Head of E/J File --- */
            ulReadPoint = sizeof(EJF_HEADER);
        }

		EJ_ReadFileA(ulReadPoint, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
		// Perform a consistency check on the EJ block header to detect corrupted EJ files
		if (THeader.usEjSignature != EJT_HEADER_SIGNATURE) {
			NHPOS_ASSERT_TEXT((THeader.usEjSignature == EJT_HEADER_SIGNATURE), "**WARNING EJReadTrans(): possible corrupt EJ file. usEjSignature != EJT_HEADER_SIGNATURE.");
		}
		if (THeader.usCVLI < *pulOffset) {
			ulReadPoint += THeader.usCVLI;
			*pulOffset = 0;
			usCo++;
			continue;
		}

		ulBytesToRead = THeader.usCVLI - *pulOffset;
		if (ulBytesToRead + *pulRetLen >= ulDataLen) {
			// This complete Electronic Journal entry will not fit into the message buffer.
			// We must provide a set of complete lines from the Electronic Journal entry so
			// we must ensure that the number of bytes is a multiple of the Electronic Journal
			// printed line length.
			ulBytesToRead -= (ulBytesToRead + *pulRetLen) - ulDataLen;
			if (*pulOffset == 0) {
				// first of several messages so we need to include the EJT_HEADER size in our calculation.
				ulBytesToRead -= ((ulBytesToRead - sizeof(EJT_HEADER)) % (EJ_COLUMN * sizeof(TCHAR)));
			} else {
				ulBytesToRead -= (ulBytesToRead % (EJ_COLUMN * sizeof(TCHAR)));
			}
		}

		EJ_ReadFileA (ulReadPoint + *pulOffset, (UCHAR *)pData + *pulRetLen, ulBytesToRead, &ulActualBytesRead);
        ulReadPoint += ulActualBytesRead;
        *pulRetLen += ulActualBytesRead;
		*pulOffset += ulActualBytesRead;
		if (*pulOffset >= THeader.usCVLI) {
			*pulOffset = 0;
			usCo++;
			break;
		}

	} while (0); // (*pulRetLen < ulDataLen);

    return(usCo);
}

/*
*==========================================================================
*   Synopsis:   SHORT   EJCheckSpace(EJF_HEADER *pFHeader)
*
*       Input:    EJF_HEADER *pFHeader - Pointer to structure for EJ_HEADER
**  Return   :
*            Normal End: EJ_PERFORM
*                      : EJ_FILE_NEAR_FULL
*
**  Description: Check Space of EJ file(Not Override).   R2.0GCA
*==========================================================================
*/
SHORT   EJCheckSpace(EJF_HEADER *pFHeader)
{
    ULONG       ulSpace;
    ULONG       ulRem;
    SHORT       sReturn = EJ_PERFORM;

    if (pFHeader->ulBeginPoint < pFHeader->ulNextPoint) {
        ulSpace = pFHeader->ulEJFSize - pFHeader->ulNextPoint;
		ulSpace += pFHeader->ulBeginPoint - sizeof(EJF_HEADER);
    } else if ((pFHeader->ulBeginPoint != pFHeader->ulNextPoint) ||
               ((pFHeader->ulBeginPoint == pFHeader->ulNextPoint) &&
                (pFHeader->ulBeginPoint != sizeof(EJF_HEADER)))) {
        ulSpace = pFHeader->ulBeginPoint - pFHeader->ulNextPoint;
    } else {                            /* ulBeginPoint == ulNextPoint == sizeof(EJF_HEADER)  */
        ulSpace = pFHeader->ulEJFSize - sizeof(EJF_HEADER);
    }

    if (ulSpace < (EJ_FULL_LIMIT + EJ_PRT_BUFFLEN)) {
        sReturn = EJ_FILE_NEAR_FULL;
    }

	// calculate percentage of remaining space
    ulRem = (ulSpace * 100L) / pFHeader->ulEJFSize;
    if ((ulRem + EJ_ReadPrtCtl(SLIP_NEARFULL_ADR) ) < 100L) {
        sReturn = EJ_FILE_NEAR_FULL;
    }

    return (sReturn);

}

/*
*==========================================================================
*   Synopsis: VOID EJLock( VOID )
*
*      Input:   Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*
**  Description:
*               Set EJ Lock for Reset Report, R2.0 GCA
*==========================================================================
*/
static struct {
	char aszFilePath[42];
	int  nLineNo;
} EJ_ReqLockPoint = { 0 };

#if defined(EJLock)
SHORT EJLock_Special( UCHAR uchLock );
SHORT   EJLock_Debug (UCHAR uchLock, char *aszFilePath, int nLineNo)
{
	char  xBuffer[256];

    if (fchEJLock & ~uchLock) {
		sprintf (xBuffer, "EJLock_Debug(): fchEJLock = 0x%x  LOCKED from File %s  lineno = %d", fchEJLock, EJ_ReqLockPoint.aszFilePath, EJ_ReqLockPoint.nLineNo);
		NHPOS_NONASSERT_TEXT(xBuffer);
	} else {
		strncpy (EJ_ReqLockPoint.aszFilePath, RflPathChop(aszFilePath), 32);
		EJ_ReqLockPoint.nLineNo = nLineNo;

	}

	sprintf (xBuffer, "EJLock_Debug(): fchEJLock = 0x%x  File %s  lineno = %d", fchEJLock, aszFilePath + iLen, nLineNo);
	NHPOS_NONASSERT_TEXT(xBuffer);
	return EJLock_Special(uchLock);
}

SHORT EJLock_Special( UCHAR uchLock )
#else
SHORT EJLock( UCHAR uchLock )
#endif
{
    /* check exclusive lock status, between terminal and isp */
    if (fchEJLock & ~uchLock) {
		char xBuff[128];
		sprintf (xBuff, "**WARNING: EJLock(): EJ_PROHIBIT LOCKED uchLock 0x%x fchEJLock 0x%x", uchLock, fchEJLock);
		NHPOS_ASSERT_TEXT(0, xBuff);
        return(EJ_PROHIBIT);
    }
    fchEJLock |= uchLock;
    return(EJ_PERFORM);
}

/*
*==========================================================================
*   Synopsis: VOID EJUnLock( VOID )
*
*      Input:   Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*
**  Description:
*               Reset EJ Lock for Reset Report, R2.0 GCA
*==========================================================================
*/
#if defined(EJUnLock)
SHORT   EJUnLock_Special( UCHAR uchLock );
SHORT   EJUnLock_Debug (UCHAR uchLock, char *aszFilePath, int nLineNo)
{
	if (fchEJLock != 0) {
		char  xBuffer[256];

		sprintf (xBuffer, "EJUnLock_Debug(): fchEJLock = 0x%x  File %s  lineno = %d", fchEJLock, RflPathChop(aszFilePath), nLineNo);
		NHPOS_NONASSERT_TEXT(xBuffer);
	}

	return EJUnLock_Special(uchLock);
}

SHORT EJUnLock_Special( UCHAR uchLock )
#else
SHORT EJUnLock( UCHAR uchLock )
#endif
{
    fchEJLock &= ~uchLock;
    return(EJ_PERFORM);
}

/*
*==========================================================================
*   Synopsis:  USHORT   EJGetStatus(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :  EJ_THERMAL_NOT_EXIST
*             :  EJ_THERMAL_EXIST
*
**  Description:
*               Return Thermal Printer Exist or Not Status
*==========================================================================
*/

USHORT EJGetStatus(VOID)
{
    USHORT  fbPrtStatus;
    USHORT  usColumn = PMG_THERMAL_CHAR_A;  /* saratoga */
    USHORT  usError;

    usError = PmgPrtConfig(PMG_PRT_RECEIPT, &usColumn, &fbPrtStatus);     /* Get Printer Status */
    if ((usError == PMG_SUCCESS) &&                 /* Thermal Printer Exist */
        ((usColumn == PMG_THERMAL_CHAR_A) ||
         (usColumn == PMG_THERMAL_CHAR_B) ||
		 (usColumn == PMG_THERMAL_CHAR_C) || //ESMITH EJ
		 (usColumn == PMG_THERMAL_CHAR_D))) { //ESMITH EJ
         return(EJ_THERMAL_EXIST);
    }
    return(EJ_THERMAL_NOT_EXIST);
}


/*
*==========================================================================
*   Synopsis:  USHORT   EJDelayBalanceUpdateFile(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :  Nothing
* 
*
**  Description:
*               Checks and reads data from the Delayed Balance update file.
*==========================================================================
*/
USHORT EJDelayBalanceUpdateFile(VOID)
{
	EJT_HEADER   *puchEJData;
	ULONG	     ulActualBytesRead, ulReadPoint;
	EJF_HEADER   EJDB_FHeader;
	SHORT	     hsDBFileHandle;
	UCHAR	     uchType = 0, uchOption = 0;
	UCHAR        auchEJData[EJ_PRT_BUFFLEN];

	puchEJData = (EJT_HEADER *)auchEJData;	

	NHPOS_NONASSERT_NOTE("==STATE", "==STATE: EJDelayBalanceUpdateFile() start EJ update from Delayed Balance.");

	EJ_ReqSem();

	//Open the EJ Delayed Balance FILE
	hsDBFileHandle = PifOpenFile(aszEJDBFileName, auchOLD_FILE_READ_WRITE);
	if (hsDBFileHandle < 0) {
		char  xBuff[128];

		EJ_RelSem();

		sprintf (xBuff, "==WARNING: EJDelayBalanceUpdateFile() error opening Delayed Balance %d.", hsDBFileHandle);
		NHPOS_ASSERT_TEXT((hsDBFileHandle >= 0), xBuff);
		return(EJ_PERFORM);
	}
	
	//read the EJ Delay Balance file header information so that we know where to stop our while loop.
	hsEJFileHandle = hsDBFileHandle;
	EJ_ReadFileA(EJ_TOP, &EJDB_FHeader, sizeof(EJF_HEADER), &ulActualBytesRead);
	
	/* ej full is infinit or once,  V3.3 */
	uchOption = 0;
	if (CliParaMDCCheckField (MDC_EJ_ADR, MDC_PARAM_BIT_D)) {
		uchOption = 1;
	}
	ulReadPoint = EJDB_FHeader.ulBeginPoint;
	if (ulReadPoint >= sizeof(EJF_HEADER)) {
		while(EJDB_FHeader.ulEndofPoint > ulReadPoint)
		{
			// all of the EJ functions use the hsEJFileHandle for writing and reading.
			// we assign to hsEJFileHandle the Delayed Balance File handle so that we can
			// read the information from the Delayed Balance file using standard EJ functions.
			//
			// EJ_Write() will open and close the TOTALEJF file for us however EJ_ReadFileA()
			// does not do an open so we change the file handle to the EJ Delay Balance file
			// when reading and let EJ_Write() handle the normal EJ file on its own.
			hsEJFileHandle = hsDBFileHandle;

			//we read the EJ entry header data of the current EJ entry
			EJ_ReadFileA(ulReadPoint, puchEJData, sizeof(EJT_HEADER), &ulActualBytesRead);

			//Read an EJ entry out of the EJ Delayed Balance file
			EJ_ReadFileA(ulReadPoint, auchEJData, puchEJData->usCVLI, &ulActualBytesRead);

			if (puchEJData->usSeqNo == 1) 
			{
				uchType = MINOR_EJ_NEW;
			} else {
				uchType = 0;
			}

			NHPOS_ASSERT_TEXT((puchEJData->usCVLI >= sizeof(EJT_HEADER)), "**ERROR: EJDelayBalanceUpdateFile() error usCVLI value.");
			EJ_Write(puchEJData,(USHORT)(puchEJData->usCVLI - sizeof(EJT_HEADER)), puchEJData, uchType, uchOption);

			ulReadPoint += ulActualBytesRead;

			//if actual bytes read equals zero, we will break out
			//of the loop.
			if(ulActualBytesRead == 0)
			{
				NHPOS_ASSERT_TEXT(0, "ulActualBytesRead Equal Zero");
				PifLog(MODULE_EJ,LOG_EVENT_EJ_RECONCILE_ERR);
				break;
			}
		}
	}

	//since we are done with the delay balance EJ file, we will
	//clear the files header information, so that when the terminal
	//goes into delay balance again, it will start at the beginning of the file.
	PifCloseFile(hsDBFileHandle);
	PifDeleteFile(aszEJDBFileName);

	{
		PARAFLEXMEM   Flexmem;

		Flexmem.uchMajorClass = CLASS_PARAFLEXMEM ;  /* FLEXIBLE MEMORY ASSIGNMENT */  
		Flexmem.uchAddress    = FLEX_EJ_ADR;         /* Set address */
		CliParaRead(&Flexmem) ;                      /* Read Flex memory parameter */
		EJCheckC (Flexmem.ulRecordNumber, Flexmem.uchPTDFlag);
	}

	EJ_RelSem();

	NHPOS_NONASSERT_NOTE("==STATE", "==STATE: EJDelayBalanceUpdateFile() finished EJ update from Delayed Balance.");

	return(EJ_PERFORM);
}
/*
*==========================================================================
*   Synopsis:  SHORT    EJWrite(EJT_HEADER *pCHeader, USHORT usWSize,
*                           UCHAR *pBuff, UCHAR uchType, UCHAR uchOption)
*       Input:    EJT_HEADER *pCHeader - pointer to structure for EJT_HEADER
*            :    USHORT     usWsize - write data size in bytes
*            :    UCHAR      *pBuff - pointer to write data buffer
*            :    UCHAR      uchType - Type of EJ File
*            :    UCHAR      uchOption - 0/1: File (Near) Full Error Message
*								displayed every occurrence / first occurrence only.
*       Output:   Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End: EJ_PERFORM
*          Abnormal End: EJ_FILE_NOT_EXIST
*                      : EJ_DATA_TOO_LARGE
*                      : EJ_FILE_FULL
*                      : EJ_FILE_NEAR_FULL
*
**  Description:
*               Write data to EJ File, which is circular. Override files
*				will write over old data once the file is full.
*				Non-override files will check for free space, and if
*				there's not enough will display error message and continue
*				without saving data.
*==========================================================================
*/
SHORT    EJ_Write(EJT_HEADER  *pCHeader, USHORT usWSize,
                        UCHAR *pBuff, UCHAR uchType, UCHAR uchOption)
{
    ULONG       ulRem;
    ULONG       ulSpace;
	ULONG		ulActualBytesRead;	//RPH 11-10-3 SR#201
    EJF_HEADER  FHeader;
    SHORT       sReturn, sFlag = 0;
    UCHAR       fchFlag;
	UCHAR		pTempBuff[EJ_PRT_BUFFLEN];

	   /* Check write data size */
    if (usWSize == 0) {                 /* Write data is 0 */
        return(EJ_PERFORM);
    }

    /* File not exist case, if error EJ_OpenFile() will release semaphore */
    if (EJ_OpenFile() != EJ_PERFORM) {
        return (EJ_FILE_NOT_EXIST);
    }

	if (usWSize + sizeof(EJT_HEADER) > sizeof(pTempBuff)) {
		//This PifLog is for when the EJ recieves information 
		//that is out of the range of the EJ_PRT_BUFFLEN, the 
		//EJ information is still printed so it is possible 
		//to look at the time stamp in the EJ and PifLog to 
		//gain more information on  the cause of the problem.
		//RZACHARY
		PifLog (MODULE_EJ, LOG_EVENT_EJ_RESET);
		usWSize = sizeof(pTempBuff) - sizeof(EJT_HEADER);
	}
	memcpy(pTempBuff, pBuff, (sizeof(EJT_HEADER) + usWSize) );

    /* Check write data size */
	//RPH 11-10-3 SR#201
    EJ_ReadFileA(EJ_TOP, &FHeader, sizeof(EJF_HEADER), &ulActualBytesRead);
    fchFlag = FHeader.fchEJFlag;
    if (FHeader.ulEJFSize < ((ULONG)usWSize + sizeof(EJF_HEADER) + sizeof(EJT_HEADER))) {  /* EJ File size < Write data */
		EJ_CloseFile(hsEJFileHandle);
		hsEJFileHandle = PIF_FILE_INVALID_HANDLE;
		PifLog(MODULE_EJ, LOG_EVENT_EJ_FFULL);
		PifLog(MODULE_LINE_NO(MODULE_EJ), (USHORT)__LINE__);
		return (EJ_DATA_TOO_LARGE);
    }

	{
		EJT_HEADER  THeader = {0};

		/* Set EJ Entry Header Parameters linking this new one to chain of previous ones */
		//RPH 11-10-3 SR#201
		if (FHeader.ulPrevPoint > 0) {
			EJ_ReadFileA(FHeader.ulPrevPoint, &THeader, sizeof(EJT_HEADER), &ulActualBytesRead);
		}

		usWSize += sizeof(EJT_HEADER);
		pCHeader->usCVLI = usWSize;
		pCHeader->usPVLI = THeader.usCVLI;
		if (uchType == MINOR_EJ_NEW) {                  /* First transaction case */
			pCHeader->usSeqNo = 1;                      /* Intialize Sequence No. */
		} else {
			pCHeader->usSeqNo = THeader.usSeqNo + 1;    /* Advance Sequence No. */
		}
	}

	//SR 718, The problem is that pCHeader is actually using the same
	//memory address as pBuff but it was cast as a EJT_HEADER.  When work on the EJ
	//for truncation was implemented, the programmer moved all the information to a temporary
	//address.  After that movement however, more information  changing was done to the header location
	//of the original memory address. By using this memcpy, we move the new information into the correct area JHHJ
	memcpy(&pTempBuff, pCHeader, sizeof(EJT_HEADER));

	if (FHeader.ulBeginPoint == FHeader.ulNextPoint && FHeader.ulBeginPoint == sizeof(EJF_HEADER)) {
        ulSpace = FHeader.ulEJFSize - sizeof(EJF_HEADER);  // the file is empty so nothing to fix up or clean up
	} else if (FHeader.ulBeginPoint < FHeader.ulNextPoint) {
		// ulBeginPoint is less than ulNextPoint so check to see if there is enough space from ulNextPoint to the
		// end of the physical file for this new Electronic Journal entry.

		ulSpace = FHeader.ulEJFSize - FHeader.ulNextPoint;
		if (ulSpace < (ULONG)usWSize) {
			// no there is not enough space so now we look to see if we will overwrite the oldest existing
			// entries, override enabled, or not.

            FHeader.ulEndofPoint = FHeader.ulNextPoint;
			ulSpace = FHeader.ulBeginPoint - sizeof(EJF_HEADER);  // how much room at beginning of file?
            if (fchFlag & EJFG_OVERRIDE) {                      /* Override Type */
				// There's not enough space between the next point and end of file for the complete entry
				// so we need to perform a wrap around to the beginning of the file to put this new entry
				// there.  However if there is not enough room then we will need to delete some of the
				// oldest existing entries in order to make room for this new entry.
                FHeader.ulNextPoint = sizeof(EJF_HEADER);
				if (ulSpace < ( ULONG)usWSize) {
					// delete EJ Entries to reuse space.
					EJ_DelBlk(&FHeader, (SHORT)(usWSize - ulSpace));
				}
            } else {                                            /* Not Override Type */
				// If file has been reset there may be enough space at the beginning of the file 
				// between the file header and beginPoint.
				if (ulSpace < ( ULONG)usWSize) {
					if (uchOption != 0) {                       /* R2.0GCA */
						if (fchFlag & EJFG_FULL) {
							EJ_CloseAndRelSem();
							return(EJ_PERFORM);
						}
					}
					fchFlag |= EJFG_FULL;
					FHeader.fchEJFlag = fchFlag;
					EJ_WriteFile(EJ_TOP, &FHeader, sizeof(EJF_HEADER),hsEJFileHandle);
					EJ_SetDesc();                               /* Light EJ FULL Descriptor */
					EJ_CloseFile(hsEJFileHandle);
					hsEJFileHandle = PIF_FILE_INVALID_HANDLE;
					PifLog(MODULE_EJ, LOG_EVENT_EJ_FFULL);
					PifLog(MODULE_LINE_NO(MODULE_EJ), (USHORT)__LINE__);
					return (EJ_FILE_FULL);
				}
				else{
					// There's free space at beginning of file, so mark endofPoint at nextPoint,
					// and move nextPoint to beginning
					FHeader.ulNextPoint = sizeof(EJF_HEADER);
				}
            }
        }
    } else {
        ulSpace = FHeader.ulBeginPoint - FHeader.ulNextPoint;
        if (ulSpace < ( ULONG)usWSize) {
            if (fchFlag & EJFG_OVERRIDE) {                /* Override Type */
                ulSpace = FHeader.ulEJFSize - FHeader.ulNextPoint;
                if (ulSpace < ( ULONG)usWSize) {
					// if there isn't enough space between nextPoint and end of file,
					// reset pointers, and clear enough space to write data
                    FHeader.ulBeginPoint = sizeof(EJF_HEADER);
                    FHeader.ulEndofPoint = FHeader.ulNextPoint;
                    FHeader.ulNextPoint = sizeof(EJF_HEADER);
					EJ_DelBlk(&FHeader, (SHORT)usWSize);
                } else {
					// if there is enough space between nextPoint and end of file, clear
					// enough old transactions to fit new data
					EJ_DelBlk(&FHeader, (SHORT)(usWSize-(FHeader.ulBeginPoint - FHeader.ulNextPoint)));
				}

            } else {                                      /* Not Override Type */
                if (uchOption != 0) {                       /* R2.0GCA */
                    if (fchFlag & EJFG_FULL) {
                        EJ_CloseFile(hsEJFileHandle);
						hsEJFileHandle = PIF_FILE_INVALID_HANDLE;
                        return(EJ_PERFORM);
                    }
                }
                fchFlag |= EJFG_FULL;
                FHeader.fchEJFlag = fchFlag;
                EJ_WriteFile(EJ_TOP, &FHeader, sizeof(EJF_HEADER),hsEJFileHandle);
                EJ_SetDesc();                               /* Light EJ FULL Descriptor */
                EJ_CloseFile(hsEJFileHandle);
				hsEJFileHandle = PIF_FILE_INVALID_HANDLE;
				PifLog(MODULE_EJ, LOG_EVENT_EJ_FFULL);
				PifLog(MODULE_LINE_NO(MODULE_EJ), (USHORT)__LINE__);
                return (EJ_FILE_FULL);
            }
        }
    }

    if (fchFlag & EJFG_OVERRIDE) {                /* Override Type */
        //RPH bytes and character counts are mixed  sizeof(tchar) is needed
		FHeader.usTtlLine += ((usWSize - sizeof(EJT_HEADER)) / (EJ_COLUMN * sizeof(TCHAR))); /* Calculate Total Line */
        EJ_W_Close(&FHeader, usWSize, pTempBuff);       /* Write and close data */
        return(EJ_PERFORM);
    } else {                                      /* Not Override Type */
		// We need to recalculate ulSpace before checking for file near full condition. 
		// If beginPoint < nextPoint, our earlier calculation may not have
		// checked for free space between sizeof(EJF_HEADER) and beginPoint. Also if 
		// nextPoint was moved to sizeof(EJF_HEADER), our earlier calculation is now inaccurate.
		if (FHeader.ulBeginPoint < FHeader.ulNextPoint) {
			ulSpace = FHeader.ulEJFSize - FHeader.ulNextPoint;
			ulSpace += FHeader.ulBeginPoint - sizeof(EJF_HEADER);
		} else if ((FHeader.ulBeginPoint != FHeader.ulNextPoint) ||
					((FHeader.ulBeginPoint == FHeader.ulNextPoint) &&
					(FHeader.ulBeginPoint != sizeof(EJF_HEADER)))) {
			ulSpace = FHeader.ulBeginPoint - FHeader.ulNextPoint;
		} else { /* ulBeginPoint == ulNextPoint == sizeof(EJF_HEADER)  */
			ulSpace = FHeader.ulEJFSize - sizeof(EJF_HEADER);
		}

        if ((ulSpace - pCHeader->usCVLI) < (EJ_FULL_LIMIT + EJ_PRT_BUFFLEN)) {
            if (uchOption != 0) {                       /* R2.0GCA */
                if (fchFlag & EJFG_NEAR_FULL) {
                    sFlag = 1;
                }
            }
            fchFlag |= EJFG_NEAR_FULL;
            FHeader.fchEJFlag = fchFlag;
        }
        ulRem =   ((ulSpace - pCHeader->usCVLI) * 100L) / FHeader.ulEJFSize;
        if ((ulRem + EJ_ReadPrtCtl(SLIP_NEARFULL_ADR) ) < 100L) {
			if (uchOption != 0) {                       /* R2.0GCA */
                if (fchFlag & EJFG_NEAR_FULL) {
                    sFlag = 1;
                }
            }
            fchFlag |= EJFG_NEAR_FULL;
            FHeader.fchEJFlag = fchFlag;
        }

		//RPH bytes and character counts are mixed  sizeof(tchar) is needed
        FHeader.usTtlLine += ((usWSize - sizeof(EJT_HEADER)) / (EJ_COLUMN * sizeof(TCHAR))); /* Calculate Total Line */
        EJ_W_Close(&FHeader, usWSize, pTempBuff);       /* Write and close data */

        sReturn = EJ_PERFORM;
        if (sFlag != 0) {                               /* R2.0GCA */
			// Above (uchOption != 0), so only display error first time. This isn't first time, so return EJ_PERFORM.
			return(sReturn);
        }
        if (fchFlag & EJFG_NEAR_FULL) {
            EJ_SetDesc();                               /* Light EJ NEAR FULL Descriptor */
            sReturn = EJ_FILE_NEAR_FULL;
        }
        return (sReturn);
    }
}
/*
*==========================================================================
*   Synopsis:  SHORT    EJWriteDelayBalance(UCHAR uchStatus)
*       Input:    UCHAR      uchStatus - (0/1: Delay Balance is off/on)
*       Output:   Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End: EJ_PERFORM
*          Abnormal End: EJ_FILE_NOT_EXIST
*                      : EJ_DATA_TOO_LARGE
*                      : EJ_FILE_FULL
*                      : EJ_FILE_NEAR_FULL
*
**  Description:
*               Write data to EJ File.
*==========================================================================
*/
SHORT    EJWriteDelayBalance(UCHAR uchStatus)
{
	TCHAR tDelayBalanceFormatOn[] =  _T("***DELAY BALANCE ON****  ");
	TCHAR tDelayBalanceFormatOff[] = _T("***DELAY BALANCE OFF***  ");
	DATE_TIME       DT;                         /* date & time */
	USHORT			usReturnLen;
	SHORT           sReturn = 0;
	UCHAR   auchPrintBuffer[sizeof(EJT_HEADER) + (24 * sizeof(TCHAR))];//byte buffer (header data + 24 characters)

	memset(&auchPrintBuffer, 0x00, sizeof(auchPrintBuffer));

	PifGetDateTime(&DT);

	// SR 634, Fix for EJ Reporting JHHJ
	// we must set up the time and day information in order for the
	// Information entered into the EJ will allow entries after this
	//
	// We need to increment the consecutive number when we generate this log or otherwise
	// we will duplication consecutive numbers, something we do not want to do.
	((EJT_HEADER *)auchPrintBuffer)->usConsecutive = MaintIncreSpcCo(SPCO_CONSEC_ADR);
	((EJT_HEADER *)auchPrintBuffer)->usCVLI = sizeof(auchPrintBuffer);
	((EJT_HEADER *)auchPrintBuffer)->usDate = DT.usMonth*100 + DT.usMDay;
	((EJT_HEADER *)auchPrintBuffer)->usTime = DT.usHour*100 + DT.usMin;
    ((EJT_HEADER *)auchPrintBuffer)->usEjSignature  = EJT_HEADER_SIGNATURE;
	if(uchStatus)
	{
		_tcsncpy((TCHAR*)&auchPrintBuffer[sizeof(EJT_HEADER)], tDelayBalanceFormatOn, 24);
	}else
	{
		_tcsncpy((TCHAR*)&auchPrintBuffer[sizeof(EJT_HEADER)], tDelayBalanceFormatOff, 24);
	}
	//send supervisor intervention to EJ to be inserted
	usReturnLen = tcharlen((TCHAR*)&auchPrintBuffer[sizeof(EJT_HEADER)]);
	auchPrintBuffer[sizeof(EJT_HEADER) + (usReturnLen * sizeof(TCHAR))] = 0x20;
	sReturn = CliEJWrite((EJT_HEADER *)auchPrintBuffer, (24 * sizeof(TCHAR)), auchPrintBuffer, MINOR_EJ_NEW, 0);
	if (sReturn != EJ_PERFORM) {
		char xBuff[128];
		sprintf (xBuff, "**ERROR - EJWriteDelayBalance(): CliEJWrite() sRetStatus = %d", sReturn);
		NHPOS_ASSERT_TEXT((sReturn == EJ_PERFORM), xBuff);
	}

	return EJ_PERFORM;
}
/****** End of Source ******/
