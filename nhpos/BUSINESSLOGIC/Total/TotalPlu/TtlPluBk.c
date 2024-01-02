/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTALPLU, TotalPlu Backup Function Library  Subroutine      
* Category    : TOTAL, NCR 2172 US Saratoga Application                     
* Program Name: TtlPluBk.c                                                  
* --------------------------------------------------------------------------
* Compiler    : VisualC++ Ver. 6.00 by Microsoft Corp.                      
* Memory Model:                                                             
* Options     :                                                             
* --------------------------------------------------------------------------
* Abstract:        
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :Ver.Rev.:   Name    : Description
* 01/25/00 : 1.00.00: K.Iwata   : 2172 Rel1.0(Saratoga) initial
*
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

#include <tchar.h>
#include <memory.h>
#include <stdio.h>
#include <math.h>

#include <ecr.h>
#include <pif.h>
#include <plu.h>
#include <csttl.h>
#include <csstbstb.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <csstbpar.h>
#include <rfl.h>
#include <regstrct.h>
#include <transact.h>
#include <ttl.h>
#include <appllog.h>

#include "ttltum.h"
#include "PluTtlD.h"

static	SHORT	s_nBackupTblIDCur = 0;		/* backup table id			*/
extern	PLUTOTAL_FILE_HANDLE	g_hdDBFile;			/* database file handle		*/

SHORT	_TtlPluClass2TblID(const UCHAR uchMajorClass,const UCHAR uchMinorClass);

VOID	_InitBackUp(VOID){
	s_nBackupTblIDCur = 0;
	g_hdDBFile = PLUTOTAL_HANDLE_NULL;
}

/*
*============================================================================
**Synopsis:     SHORT  TtlReqBackEach(SHORT hsHandle, UCHAR uchType)
*
*    Input:     hsHandle    - each file handle
*               uchType     - What file would like to read
*
*   Output:     Nothing
*    InOut:
*
** Return:      TTL_SUCCES       Success 
*               TTl_FAIL         Fail
*
** Description  This function receives the Total File.
*
*============================================================================
*/
SHORT   TtlReqBackEachPlu(const SHORT nClass){
	SHORT			nTblID;
	TTLBACKUP_REQ   RequestBuff = { 0 };
    TTLBACKUP_RES   ResponseBuff = { 0 };
    SHORT           sError = 0;
	ULONG			ulSts;

	/* minor class ---> table id */
	nTblID = _TtlPluClass2TblID(CLASS_TTLPLU,(UCHAR)nClass);
	if(nTblID == TTL_NOTCLASS) {
        PifLog(MODULE_TTL_TFL_BACKUP, FAULT_ERROR_FILE_OPEN);
        PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)abs(TTL_NOTCLASS));
		return	TTL_FAIL;
	}

	/* ### ADD check PTD flag (041100) */
	switch(nTblID){
    case	PLUTOTAL_ID_PTD_CUR:
	case	PLUTOTAL_ID_PTD_SAV:
		if(RflGetPtdFlagByType(FLEX_PLU_ADR) == FALSE){
			if (PluTotalDelFile(nTblID) != PLUTOTAL_SUCCESS) {	/* delete PTD(cur & sav) file */
				NHPOS_NONASSERT_NOTE("==ACTION", "**ERROR: TtlReqBackEachPlu() delete PTD(cur & sav) file failed.");
			}
			return	TTL_SUCCESS;
		}
		break;
	default:
		break;
	}

	//This function creates a copy of the PLU database file for use with 
	//synchronization. SR 725 JHHJ
	ulSts = PluTotalOpenFile(&g_hdDBFile,nTblID,PLUTOTAL_DBFILE_CREATEWRITE);
	if(ulSts != TTL_SUCCESS)
	{
		char  xBuff[128];

		sprintf (xBuff, "==NOTE:  PluTotalOpenFile() returned ulSts %d 0x%x", ulSts, ulSts);
		NHPOS_ASSERT_TEXT((ulSts == TTL_SUCCESS), xBuff);
	}

    RequestBuff.uchFileType = TTL_BAK_PLU;  /* What file ? */
    RequestBuff.ulOffSet    = 0L;           /* start offset */
    RequestBuff.usCount     = 0;            /* request counter */
    RequestBuff.sFileSubType= nTblID;       /* FileSubType */

	do {
		LONG			lTotalPluDataLen = 0;
		USHORT          usMaxLen = sizeof(TTLBACKUP_RES);   /* buffer size maximum  REQUEST READ */

        RequestBuff.usCount ++;             /* counter increment */
        sError = SstReqBackUp(CLI_FCBAKTOTAL, &RequestBuff, sizeof(TTLBACKUP_REQ), &ResponseBuff, &usMaxLen);
        if (TTL_SUCCESS == sError) {        /* reading success ? */
                                            /* handle exist ? */
            if (RequestBuff.usCount != ResponseBuff.F.usCount) {
                if (RequestBuff.usCount == (ResponseBuff.F.usCount - 1)) {
                    RequestBuff.usCount --;
                } else {
                    RequestBuff.ulOffSet = 0L; 
                    RequestBuff.usCount = 0;
                }
                continue;
            }

			lTotalPluDataLen = (LONG)(ResponseBuff.D.usDataSize - (sizeof(TTLBACKUP_RESFIX) + 2)); /* ### CAUTION!!! "- 14" see TtlResBackEachPlu(). */
            if (0 < lTotalPluDataLen) { /* data exist ? */
				ULONG  ulRetStatus = 0;
				if((ulRetStatus = PluTotalWriteFile(g_hdDBFile,ResponseBuff.F.ulOffSet,lTotalPluDataLen,&ResponseBuff.D.auchData[0])) != PLUTOTAL_SUCCESS) {
					char  xBuff[128];
					sprintf (xBuff, "**ERROR**: TtlReqBackEachPlu() ulRetStatus = %d", ulRetStatus);
					NHPOS_ASSERT_TEXT((ulRetStatus == PLUTOTAL_SUCCESS), xBuff);
                    PifLog(MODULE_TTL_TFL_BACKUP, FAULT_ERROR_FILE_SEEK);
                    PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), (USHORT)ulRetStatus);
                    PifAbort(MODULE_TTL_TFL_BACKUP, FAULT_ERROR_FILE_SEEK);
				}
            }
        } else {                            /* reading fail ? */
            PifLog(MODULE_TTL_TFL_BACKUP, LOG_ERROR_TTL_BACKUP_MASTER);
            PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_BACKUP), abs(sError));
        }
                                            /* increment offset */
        /*RequestBuff.ulOffSet += ResponseBuff.D.usDataSize;*/
        RequestBuff.ulOffSet += lTotalPluDataLen;
    } while ((TTL_SUCCESS == sError) && (0 == ResponseBuff.F.uchEOF));

	/* close */
	//because are using a datbase file, and not the actual database, we
	//must close the file so that we can re open it for restoring the databse
	//JHHJ SR 725
	PluTotalCloseFile(g_hdDBFile);
	g_hdDBFile = PLUTOTAL_HANDLE_NULL;

	ulSts = PluTotalRestoreDB(g_hdDBFile,nTblID);
	if(ulSts != TTL_SUCCESS)
	{
		char  xBuff[128];

		sprintf (xBuff, "**WARNING:  TTL_BKUPPLUDBFAIL  PluTotalRestoreDB() returned ulSts %d 0x%x", ulSts, ulSts);
		NHPOS_ASSERT_TEXT((ulSts == TTL_SUCCESS), xBuff);
		return TTL_BKUPPLUDBFAIL;
	}

	if (sError == TTL_FILE_OPEN_ERR){
		PluTotalDelFile(nTblID);	/* delete PTD(cur & sav) file */
		sError = TTL_SUCCESS;
	}

	return	sError;
}

/*
*============================================================================
**Synopsis:     SHORT  TtlResBackEach(TTLBACKUP_RES *pSend,
*                                      USHORT usMaxLen, SHORT hsHandle);
*
*    Input:     usMaxLen    - max buffer length can be read
*    Input:     hsHandle    - each file handle
*   Output:     pSend       - send buffer address
*
** Return:      size of send data
*
** Description  This function reads data from base total file.
*
*============================================================================
*/
USHORT  TtlResBackEachPlu(TTLBACKUP_RES *pSend,const USHORT usMaxLen){
	LONG			lReadLen = 0; 
	ULONG			ulSts;

	pSend->F.sError = TTL_SUCCESS;
	pSend->F.uchEOF = 0;
	pSend->D.usDataSize = 0;

	/* only PLU */
	if(pSend->F.uchFileType != TTL_BAK_PLU){
		pSend->F.sError = TTL_FAIL;
		return	0;
	}

	/*  */
	if(s_nBackupTblIDCur != pSend->F.sFileSubType){
		if(g_hdDBFile != PLUTOTAL_HANDLE_NULL){
			//Memory Leak 7.11.05
			//We do not need to close the File, because we only close it now
			//once at the closing of the application
//			PluTotalCloseFile(g_hdDBFile);
			//g_hdDBFile = PLUTOTAL_HANDLE_NULL;
		}else
		{
#ifdef XP_PORT
			PluTotalOpenFile(&g_hdDBFile,pSend->F.sFileSubType,PLUTOTAL_DBFILE_CREATEREAD);
#else
			PluTotalOpenFile(&g_hdDBFile,pSend->F.sFileSubType,PLUTOTAL_DBFILE_CREATEREAD);
#endif

		}
	}

	/* read data */
	ulSts = PluTotalReadFile(g_hdDBFile, pSend->F.ulOffSet, usMaxLen, pSend->D.auchData, &lReadLen);
	if(lReadLen < TTL_BAK_BUFFER)
	{
		pSend->F.sError = TTL_SUCCESS;
	}

	//pSend->D.usDataSize = lReadLen; /*size of read length */
	pSend->D.usDataSize = (USHORT)(lReadLen + sizeof(TTLBACKUP_RESFIX) + 2); /* ### CAUTION "+14" */

	/* EOF? */
	if(ulSts == PLUTOTAL_EOF || (ulSts == PLUTOTAL_SUCCESS && lReadLen == 0L)){
		//Memory Leak 7.11.05
		//We do not need to close the File, because we only close it now
		//once at the closing of the application
		PluTotalCloseFile(g_hdDBFile);
		g_hdDBFile = PLUTOTAL_HANDLE_NULL;
		s_nBackupTblIDCur  = 0;
		pSend->F.uchEOF = 1;            /* EOF */
	}
	else{
		pSend->F.uchEOF = 0;            /* still exist data */
	}

	return (pSend->D.usDataSize);
}


/*=========================================================================*/
