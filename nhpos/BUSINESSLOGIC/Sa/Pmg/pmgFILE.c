/*
**************************************************************************
**
**  Title:      Print Manager Interface Routine
**
**  Categoly:   Print Manager
**
**  Abstruct:   Interface Routine of Application and Print Manager
**
*;========================================================================
*
*   PVCS ENTRY
*
*;========================================================================
*
*$Revision$
*$Date$
*$Author$
*$Log$
*
**************************************************************************
*/

/**
;=============================================================================
;     Rev.   |  Date      |  Comment
;-----------------------------------------------------------------------------
;   00.00.00 |            | Initial
:            |            | 
;=============================================================================
**/

/**
;=============================================================================
;+                                                                           +
;+                  I N C L U D E     F I L E s                              +
;+                                                                           +
;=============================================================================
**/

#include    <windows.h>
#include    <stdio.h>
#ifndef _WIN32_WCE
#include	<tchar.h>
#endif
#include    <DeviceIOPrinter.h>
#include    <Scf.h>
#include    "ecr.h"
#include    "pif.h"
#include    "rfl.h"
#include    "uie.h"
#include    "paraequ.h"
#include    "pmg.h"
#include    "LoadDev.h"
#include    "pmglib.h"
#include	"BlFWif.h"
#include	"display.h"

#define MAX_PRINTER_FILES    20

static SHORT  PmgFileBlkIndex = -1;

static PMGBLK  PmgFileBlk[MAX_PRINTER_FILES];

static TCHAR tcFileNames[MAX_PRINTER_FILES][36];


/**
;========================================================================
;+                                                                      +
;+            S T A T I C    R A M    A S S I G N M E N T               +
;+                                                                      +
;========================================================================
**/

USHORT PmgFileGetNextPrtType (VOID)
{
	USHORT usIndex = 0;
	USHORT usPrtType;
	TCHAR  tcFilePath [256];

	if (PmgFileBlkIndex < 0) {
		PmgFileInitialize();
	}

	usIndex = ++PmgFileBlkIndex;

	if (PmgFileBlkIndex >= MAX_PRINTER_FILES) {
		usIndex = PmgFileBlkIndex = 1;
	}

	usPrtType = PMG_PRT_WEB_FILE | (usIndex << 8);

	_tcscpy (tcFilePath, BL_PRTFILE_WEB);

	_tcscat (tcFilePath, PmgFileBlk[usIndex].pmgFileInfo.ptcFileName);

	PmgFileBlk[usIndex].pmgFileInfo.pFileHandle = _tfopen ( tcFilePath, _T("w"));

	return usPrtType;
}

VOID *PmgFileGetThisFileInfo (USHORT usPrtType)
{
	USHORT usIndex = ((usPrtType >> 8) & 0xff);

	if ( (usPrtType & PMG_PRT_WEB_MASK) == PMG_PRT_WEB_FILE) {
		return &(PmgFileBlk[usIndex].pmgFileInfo);
	}
	else
	{
		return NULL;
	}
}

VOID *PmgFileCloseThisFile (USHORT usPrtType)
{
	USHORT usIndex = ((usPrtType >> 8) & 0xff);

	if ( (usPrtType & PMG_PRT_WEB_MASK) == PMG_PRT_WEB_FILE) {
		if (PmgFileBlk[usIndex].pmgFileInfo.pFileHandle) {
			fclose (PmgFileBlk[usIndex].pmgFileInfo.pFileHandle);
			PmgFileBlk[usIndex].pmgFileInfo.pFileHandle = 0;
		}
		return &(PmgFileBlk[usIndex].pmgFileInfo);
	}
	else {
		return NULL;
	}
}


USHORT PmgFileGetThisPrtType (VOID *pvOutput)
{
	USHORT usPrtType = 0;

	if (pvOutput) {
		PMGBLK_FILE *pvFile = (PMGBLK_FILE *)pvOutput;
		usPrtType = PMG_PRT_WEB_FILE | (pvFile->usIndex << 8);
	}
	return usPrtType;
}

TCHAR * PmgFileGetThisFileName (VOID *pvOutput)
{
	TCHAR * ptchFileName = 0;

	if (pvOutput) {
		PMGBLK_FILE *pvFile = (PMGBLK_FILE *)pvOutput;
		ptchFileName = pvFile->ptcFileName;
	}
	return ptchFileName;
}

TCHAR * PmgFileGetThisFileNamePrt (USHORT usPrtType)
{
	TCHAR * ptchFileName = 0;
	USHORT usIndex = ((usPrtType >> 8) & 0xff);

	if ( (usPrtType & PMG_PRT_WEB_MASK) == PMG_PRT_WEB_FILE) {
		PMGBLK_FILE *pvFile = (PMGBLK_FILE *)PmgFileGetThisFileInfo(usPrtType);
		if (pvFile)
			ptchFileName = pvFile->ptcFileName;
	}
	return ptchFileName;
}

FILE * PmgFileGetThisFileHandlePrt (USHORT usPrtType)
{
	FILE * pFileHandle = 0;
	USHORT usIndex = ((usPrtType >> 8) & 0xff);

	if ( (usPrtType & PMG_PRT_WEB_MASK) == PMG_PRT_WEB_FILE) {
		PMGBLK_FILE *pvFile = (PMGBLK_FILE *)PmgFileGetThisFileInfo(usPrtType);
		if (pvFile)
			pFileHandle = pvFile->pFileHandle;
	}
	return pFileHandle;
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgFileWinInitialize(VOID)
*
**Return:       Non
*
**Description:  Print Manager Start up
*
*********************************************************************
*/

VOID PmgFileInitialize( VOID )
{
    USHORT   i;

	memset (PmgFileBlk, 0, sizeof(PmgFileBlk));

    for (i = 0; i < MAX_PRINTER_FILES; i++)
    {
        PmgFileBlk[i].dwDllLoaded = TRUE;
		PmgFileBlk[i].pmgFileInfo.ptcFileName = tcFileNames[i];
		_stprintf (&tcFileNames[i][0], _T("web%4.4d"), i+1);
		PmgFileBlk[i].pmgFileInfo.usIndex = i;
	}

	PmgFileBlkIndex = 1;
    return;
}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgFileFinalize(VOID)
*
**Return:       Non
*
**Description:  Print Manager Finalize
*
*********************************************************************
*/

VOID PmgFileFinalize( VOID )
{
	//device engine should close
#if 0
    DWORD   i;

    for (i = 0; i < MAX_PRINTER_FILES; i++)
    {
        if (PmgFileBlk[i].dwDllLoaded)
        {
            PmgFileBlk[i].Func.Close(PmgFileBlk[i].hHandle);
            FreeDevice();
        }
    }
#endif
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgFilePrint(usPrtType, pucBuff, usLen)
*               input   USHORT  usPrtType;  : type of printer
*               input   UCHAR   *pucBuff;   : pointer to print buffer
*               input   USHORT  usLen;      : print data length
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert print data into spool buffer
*				
*				This functions third parameter, usLen, should specify 
*				the number of characters within the string, pucBuff.
*
*					----- NOT THE NUMBER OF BYTES!!! -----
*
*			  ** NOTE **
*			     Print to file in Supervisor Mode uses function BlFwIfPrintFileWrite()
*			     rather than this functionality.  See function PmgOPOSPrint()
*			     in the OPOS print functionality.  Richard Chambers, Nov-20-2014
**************************************************************************
*/
USHORT  PmgFilePrint(USHORT usPrtType, TCHAR *pucBuff, USHORT usLen)
{
	int     j = 0, k = 0;
	FILE    *pFileHandle = 0;
	TCHAR   prtBuff[MAX_PRINT_BUFFER] = {0};
	BOOL    bS, bD, bSD;

	bS = bD = bSD = FALSE;

	if(pucBuff[0]){
		for(j = 0, k = 0; j < usLen; j++){
				switch(pucBuff[j]){
				case PRT_SDOUBLE://Double High and Wide
					break;

				case PRT_DOUBLE://Double Wide
					break;

				case 9://Tab, so Right Justify
					// default tab positions are every 8 characters so tab positions are
					// at character positions 9, 17, 25, ...
					for ( ; k % 8 != 1; k++) {
						prtBuff[k] = _T(' ');
					}
					break;

				case PRT_LOGO:
					break;

				case PRT_CENTERED:
					break;

				default:
					if(pucBuff[j]){
						prtBuff[k++] = pucBuff[j];
					}
					break;
				}
		}
	}

	pFileHandle = PmgFileGetThisFileHandlePrt (usPrtType);
	if (pFileHandle) {
		_ftprintf(pFileHandle, _T("%-52.52s"), prtBuff);
    }
	
    return(PMG_SUCCESS);
}


/*
**************************************************************************
*
**Synopsis:     VOID    PmgFileWait( VOID )
*
**Return:       non
*
**Description:  Wait for empty the spool buffer
*
**************************************************************************
*/

VOID PmgFileWait( VOID )
{

	return;//RPH just return for now 4-5-4
}



/*
**************************************************************************
*
**Synopsis:     USHORT  PmgFileBeginSmallValidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Set Small Validation flag which sends Station Selection and
*				Ignore paper sensor parameters.
*
*               Added to provide for short slip printing for US Customs.
*               Problem was trailing edge of short slip would pass trailing
*               edge paper sensor thereby causing printing to stop in the
*               middle of validation printing.  The changes to this file
*               provide for the use of a flag in the NCR 7158 printer DLL
*               to not send the printer station selection string.
*               When the NCR 7158 printer receives a station select string,
*               the printer automatically checks for paper under the paper
*				out sensors.  If no paper is under both sensors the printer stops.
*
*               This code complements the code added to pmgif.cpp in the Device7158Printer
*               files project.  This function uses the IoControl function to
*               talk with the printer DLL requesting that the station selction
*               string not be sent before each line.
*
*               For those printer DLLs that do not handle the DEVICEIO_CTL_PRINTER_SMALL_VALIDATION
*               message, this request should be safely ignored and will do nothing.
*
*************************************************************************
*/

USHORT PmgFileBeginSmallValidation(USHORT usPrtType)
{
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgFileEndSmallVlidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Reset Small Validation flag to none
*
**************************************************************************
*/

USHORT PmgFileEndSmallValidation(USHORT usPrtType)
{
    return(PMG_SUCCESS);
}


/*
**************************************************************************
*
**Synopsis:     USHORT  PmgFileBeginValidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert begin of validation into spool buffer
*
**************************************************************************
*/

USHORT PmgFileBeginValidation(USHORT usPrtType)
{
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgFileEndVlidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert end of validation into spool buffer
*
**************************************************************************
*/

USHORT PmgFileEndValidation(USHORT usPrtType)
{
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgFileFeed(usPrtType, usLine)
*               input   USHORT  usPrtType;  : type of printer
*               input   USHORT  usLine;     : number of lines
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*               PMG_ERROR_COMM      : communication error
*               PMG_ERROR_PRNTER    : printer error
*
**Description:  Feed the desired pinter
*
**************************************************************************
*/

USHORT PmgFileFeed(USHORT usPrtType, USHORT usLine)
{
    return(PMG_SUCCESS);
}

     
/*
**************************************************************************
*
**Synopsis:     USHORT  PmgFileBeginImportant(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert begin of important lines into spool buffer
*
**************************************************************************
*/

USHORT PmgFileBeginImportant(USHORT usPrtType)
{
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgFileEndImportant(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert end of important lines into spool buffer
*
**************************************************************************
*/

USHORT PmgFileEndImportant(USHORT usPrtType)
{
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgFileGetStatus(usPrtType, pfbStatus)
*               input   USHORT  usPrtType;  : type of printer
*               inout   PUSHORT pfbStatus;  : printer status
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*               PMG_ERROR_COMM      : communication error
*               PMG_ERROR_PRNTER    : printer error
*
**Description:  Get printer status. This function execute immediately.
*
**************************************************************************
*/

USHORT PmgFileGetStatus(USHORT usPrtType, USHORT *pfbStatus)
{
    DWORD   i;

    for (i = 0; i < MAX_PRINTER_FILES; i++)
    {
        if (PmgFileBlk[i].dwDllLoaded)
        {
			BlFwGetPrinterStatus(usPrtType, pfbStatus);	
			if(*pfbStatus & PRT_STAT_POWER_OFF)
			{
				PmgCallBack(DEVICEIO_PRINTER_MSG_POWEROFF_ADR, 0);
			}
		
		}
	}
    return(PMG_SUCCESS);
}

USHORT PmgFileGetStatusOnly(USHORT usPrtType, USHORT *pfbStatus)
{
    DWORD   i;

    for (i = 0; i < MAX_PRINTER_FILES; i++)
    {
        if (PmgFileBlk[i].dwDllLoaded)
        {
			BlFwGetPrinterStatus(usPrtType, pfbStatus);	
		}
	}
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgFilePrtConfig(usPrtType, pusColumn, pfbStatus)
*               input   USHORT  usPrtType;  : type of printer
*               inout   USHORT  *pusColumn; : number of clumn
*               inout   USHORT  *pfbStatus; : printer status
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*               PMG_ERROR_COMM      : communication error
*               PMG_ERROR_PRNTER    : printer error
*
**Description:  Get printer status & number of column.
*
**************************************************************************
*/

USHORT PmgFilePrtConfig(USHORT usPrtType, USHORT *pusColumn, USHORT *pfbStatus)
{
    DWORD   i;
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

    for (i = 0; i < MAX_PRINTER_FILES; i++)
    {
        if (PmgFileBlk[i].dwDllLoaded)
        {
			switch(usPrtTypeMasked){
			case PMG_PRT_RECEIPT: /* receipt station          */
			case PMG_PRT_RCT_JNL: /* receipt & journal        */
			//	BlFwGetPrinterColumnsStatus(PTR_S_RECEIPT, pusColumn, pfbStatus);
				break;
			case PMG_PRT_JOURNAL: /* journal station          */
			//	BlFwGetPrinterColumnsStatus(PTR_S_JOURNAL, pusColumn, pfbStatus);
				break;
			case PMG_PRT_VALIDATION: /* validation               */
			case PMG_PRT_SLIP: /* slip printer             */
			//	BlFwGetPrinterColumnsStatus(PTR_S_SLIP, pusColumn, pfbStatus);
				break;
			default:
				break;
			}
        }

    }
    return(PMG_SUCCESS);
}


/*
**************************************************************************
*
**Synopsis:     USHORT  PmgFileFont(usPrtType, usFont)
*               input   USHORT  usPrtType;  : type of printer
*               inout   USHORT  usFont;     : type of font
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*               PMG_ERROR_COMM      : communication error
*
**Description:  Set to font type
*
**************************************************************************
*/

USHORT  PmgFileFont( USHORT usPrtType, USHORT usFont)
{
    DWORD   i;
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

    for (i = 0; i < MAX_PRINTER_FILES; i++)
    {
        if (PmgFileBlk[i].dwDllLoaded)
        {
			switch(usPrtTypeMasked){
			case PMG_PRT_RECEIPT: /* receipt station          */
			case PMG_PRT_RCT_JNL: /* receipt & journal        */
//				BlFwIfPrintSetFont(PTR_S_RECEIPT, usFont);
				break;
			case PMG_PRT_JOURNAL: /* journal station          */
			//	BlFwIfPrintSetFont(PTR_S_JOURNAL, usFont);
				break;
			case PMG_PRT_VALIDATION: /* validation               */
			case PMG_PRT_SLIP: /* slip printer             */
			//	BlFwIfPrintSetFont(PTR_S_SLIP, usFont);
				break;
			default:
				break;
			}

//            PmgFileBlk[i].Func.IoControl(
//                PmgFileBlk[i].hHandle, 
//                DEVICEIO_CTL_PRINTER_FONT, 
//                NULL, 
//                0, 
//                &Arg, 
//                sizeof(Arg), 
//                &dwDummy);
        }

    }
    return(PMG_SUCCESS);
}


/*
**************************************************************************
*
**Synopsis:     VOID    PmgFileSetValWaitCount(USHORT usRetryCount)
*               input   USHORT  usRetryCount;   : retry counter
*                                   = 0:    wait etertally
*                                   = X:    retry count 
**Return:       none
*
**Description:  This function set retry counter for checking validation sheet.
*               If retry over this counter then pmg send messege to printer
*               whether sheet exist or not. 
*
**************************************************************************
*/

VOID PmgFileSetValWaitCount(USHORT usRetryCount)
{
    DWORD   i;

    /* Initialize */

    for (i = 0; i < MAX_PRINTER_FILES; i++)
    {
        if (PmgFileBlk[i].dwDllLoaded)
        {
        }

    }
    return;
}   


VOID PmgFileStartReceipt(VOID)
{
}

VOID PmgFileEndReceipt(VOID)
{
}

/***    End of PMGIF.C    ***/
