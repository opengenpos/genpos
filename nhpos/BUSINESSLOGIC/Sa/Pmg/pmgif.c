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
#include	"display.h"
#include    "BlFwIf.h"
#include	"transact.h"

PMGBLK  PmgBlk[MAX_PRINTER];
static DWORD  dwCallBackLastErrorCode = 0;           // contains error message from last time PmgCallBack() called.
static BOOL   bCallBackLastPrinterOffline = FALSE;   // indicates last Printer Off Line


typedef struct {
	VOID    (*PmgInitialize) (VOID);
	VOID    (*PmgFinalize) (VOID);
	VOID    (*PmgStartReceipt) ( VOID );
	VOID    (*PmgEndReceipt) ( VOID );
	USHORT  (*PmgPrint) (USHORT, TCHAR *, USHORT);
	VOID    (*PmgWait) (VOID);
	USHORT  (*PmgBeginSmallValidation) (USHORT);
	USHORT  (*PmgEndSmallValidation) (USHORT);
	USHORT  (*PmgBeginValidation) (USHORT);
	USHORT  (*PmgEndValidation) (USHORT);
	USHORT  (*PmgFeed) (USHORT, USHORT);
	USHORT  (*PmgBeginImportant) (USHORT);
	USHORT  (*PmgEndImportant) (USHORT);
	USHORT  (*PmgGetStatus) (USHORT, USHORT *);
	USHORT  (*PmgGetStatusOnly) (USHORT, USHORT *);
	USHORT  (*PmgPrtConfig) (USHORT, USHORT *, USHORT *);
	USHORT  (*PmgFont) (USHORT usPrtType, USHORT);
	VOID    (*PmgSetValWaitCount) (USHORT);
	VOID	(*PmgBeginTransaction)(SHORT, ULONG);
	VOID	(*PmgEndTransaction)(SHORT, ULONG);
	LONG    (*PmgGetSetPrtCap) (USHORT usPrtType, USHORT usPrtCap, LONG lValue);
	USHORT  (*PmgPrintBarCode) (USHORT usPrtType, TCHAR *pucBuff, ULONG ulTextFlags, ULONG ulCodeFlags);
} PrinterFuncStruct;

PrinterFuncStruct printerfuncs [] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{                             // PMG_OPOS_PRINTER and PMG_OPOS_IMMEDIATE
		//  ** NOTE **
		//     Print to file in Supervisor Mode uses function BlFwIfPrintFileWrite()
		//     called through the OPOS print.  Richard Chambers, Nov-20-2014

		PmgOPOSInitialize,
		PmgOPOSFinalize,
		0,                        // PmgStartReceipt () is not defined for an OPOS printer
		0,                        // PmgEndReceipt () is not defined for an OPOS printer
		PmgOPOSPrint,
		PmgOPOSWait,
		PmgOPOSBeginSmallValidation,
		PmgOPOSEndSmallValidation,
		PmgOPOSBeginValidation,
		PmgOPOSEndValidation,
		PmgOPOSFeed,
		PmgOPOSBeginImportant,
		PmgOPOSEndImportant,
		PmgOPOSGetStatus,
		PmgOPOSGetStatusOnly,
		PmgOPOSPrtConfig,
		PmgOPOSFont,
		PmgOPOSSetValWaitCount,
		PmgOPOSBeginTransaction,
		PmgOPOSEndTransaction,
		PmgOPOSGetSetPrtCap,
		PmgOPOSPrintBarCode
	},
	{             // PMG_WIN_PRINTER
		PmgWinInitialize,
		PmgWinFinalize,
		PmgWinStartReceipt,
		PmgWinEndReceipt,
		PmgWinPrint,
		PmgWinWait,
		PmgWinBeginSmallValidation,
		PmgWinEndSmallValidation,
		PmgWinBeginValidation,
		PmgWinEndValidation,
		PmgWinFeed,
		PmgWinBeginImportant,
		PmgWinEndImportant,
		PmgWinGetStatus,
		PmgWinGetStatusOnly,
		PmgWinPrtConfig,
		PmgWinFont,
		PmgWinSetValWaitCount,
		PmgWinBeginTransaction,
		PmgWinEndTransaction,
		0,
		0
	},
	{             // PMG_WEB_FILE
		//  ** NOTE **
		//     Print to file in Supervisor Mode uses function BlFwIfPrintFileWrite()
		//     rather than this functionality.  See function PmgOPOSPrint()
		//     in the OPOS print functionality.  Richard Chambers, Nov-20-2014
		PmgFileInitialize,
		PmgFileFinalize,
		PmgFileStartReceipt,
		PmgFileEndReceipt,
		PmgFilePrint,
		PmgFileWait,
		PmgFileBeginSmallValidation,
		PmgFileEndSmallValidation,
		PmgFileBeginValidation,
		PmgFileEndValidation,
		PmgFileFeed,
		PmgFileBeginImportant,
		PmgFileEndImportant,
		PmgFileGetStatus,
		PmgFileGetStatusOnly,
		PmgFilePrtConfig,
		PmgFileFont,
		PmgFileSetValWaitCount,
		0,
		0
	},
	{             // PMG_ZEBRA_PRINTER
		PmgZEBRAInitialize,
		PmgZEBRAFinalize,
		0,                        // PmgStartReceipt () is not defined for an OPOS printer
		0,                        // PmgEndReceipt () is not defined for an OPOS printer
		PmgZEBRAPrint,
		PmgZEBRAWait,
		PmgZEBRABeginSmallValidation,
		PmgZEBRAEndSmallValidation,
		PmgZEBRABeginValidation,
		PmgZEBRAEndValidation,
		PmgZEBRAFeed,
		PmgZEBRABeginImportant,
		PmgZEBRAEndImportant,
		PmgZEBRAGetStatus,
		PmgZEBRAGetStatusOnly,
		PmgZEBRAPrtConfig,
		PmgZEBRAFont,
		PmgZEBRASetValWaitCount,
		PmgZEBRABeginTransaction,
		PmgZEBRAEndTransaction,
		0,
		0
	},
	{             // PMG_CONNECTION_ENGINE
		PmgConnEngineInitialize,
		PmgConnEngineFinalize,
		0,                        // PmgStartReceipt () is not defined for an OPOS printer
		0,                        // PmgEndReceipt () is not defined for an OPOS printer
		PmgConnEnginePrint,
		PmgConnEngineWait,
		PmgConnEngineBeginSmallValidation,
		PmgConnEngineEndSmallValidation,
		PmgConnEngineBeginValidation,
		PmgConnEngineEndValidation,
		PmgConnEngineFeed,
		PmgConnEngineBeginImportant,
		PmgConnEngineEndImportant,
		PmgConnEngineGetStatus,
		PmgConnEngineGetStatusOnly,
		PmgConnEnginePrtConfig,
		PmgConnEngineFont,
		PmgConnEngineSetValWaitCount,
		PmgConnEngineBeginTransaction,
		PmgConnEngineEndTransaction,
		0,
		0
	},
	{                             // PMG_COM_PRINTER
		PmgCOMInitialize,
		PmgCOMFinalize,
		0,                        // PmgStartReceipt () is not defined for an OPOS printer
		0,                        // PmgEndReceipt () is not defined for an OPOS printer
		PmgCOMPrint,
		PmgCOMWait,
		0,                        // PmgBeginSmallValidation () is not defined for a direct to COM printer
		0,                        // PmgEndSmallValidation () is not defined for a direct to COM printer
		0,                        // PmgBeginValidation () is not defined for a direct to COM printer
		0,                        // PmgEndValidation () is not defined for a direct to COM printer
		PmgCOMFeed,
		0,                        // PmgBeginImportant () is not defined for a direct to COM printer
		0,                        // PmgEndImportant () is not defined for a direct to COM printer
		PmgCOMGetStatus,
		PmgCOMGetStatusOnly,
		PmgCOMPrtConfig,
		PmgCOMFont,
		PmgCOMSetValWaitCount,
		0,                        // PmgBeginTransaction () is not defined for a direct to COM printer
		0,                        // PmgEndTransaction () is not defined for a direct to COM printer
		PmgCOMGetSetPrtCap,
		PmgCOMPrintBarCode
	}
};

// This define will automatically calculate the number of printers defined
// in the table so that if someone defines a new printer type but does not
// update the table above the new printer type will not be recognized but
// the code will still perform correctly for the existing printer types.
// Do not modify this define but instead add new printer types to the
// printerfuncs [] table above.
#define  MAX_PRINTER_TYPE  (sizeof(printerfuncs)/sizeof(printerfuncs[0]))


/**
;========================================================================
;+                                                                      +
;+            S T A T I C    R A M    A S S I G N M E N T               +
;+                                                                      +
;========================================================================
**/

/*
*********************************************************************
*
**Synopsis:     VOID    PmgInitialize(VOID)
*
**Return:       Non
*
**Description:  Print Manager Start up
*
*********************************************************************
*/

USHORT PmgPrintType(VOID)
{
    return BlFwIfGetPrintType();
}
/*
*********************************************************************
*
**Synopsis:     VOID    PmgInitialize(VOID)
*
**Return:       Non
*
**Description:  Print Manager Start up
*
*********************************************************************
*/

VOID PmgInitialize( VOID )
{
	USHORT usFunc = 0;

	usFunc = BlFwIfGetPrintType();
	if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgInitialize) {
		printerfuncs[usFunc].PmgInitialize ();
	}
}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgFinalize(VOID)
*
**Return:       Non
*
**Description:  Print Manager Finalize
*
*********************************************************************
*/

VOID PmgFinalize( VOID )
{
	USHORT usFunc = 0;

	usFunc = BlFwIfGetPrintType();
	if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgFinalize) {
		printerfuncs[usFunc].PmgFinalize ();
	}
	return;
}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgStartReceipt(VOID)
*
**Return:       Non
*
**Description:  Print Manager Start of Receipt processing
*
*********************************************************************
*/

VOID PmgStartReceipt( VOID )
{
	USHORT usFunc = 0;

	usFunc = BlFwIfGetPrintType();
	if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgStartReceipt) {
		if (printerfuncs[usFunc].PmgStartReceipt)
		{
			printerfuncs[usFunc].PmgStartReceipt ();
		}
	}
	return;
}


/*
*********************************************************************
*
**Synopsis:     VOID    PmgEndReceipt(VOID)
*
**Return:       Non
*
**Description:  Print Manager End Receipt processing
*
*********************************************************************
*/

VOID PmgEndReceipt( VOID )
{
	USHORT usFunc = 0;

	usFunc = BlFwIfGetPrintType();
	if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgEndReceipt) {
		if (printerfuncs[usFunc].PmgEndReceipt)
		{
			printerfuncs[usFunc].PmgEndReceipt ();
		}
	}
	return;
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgPrint(usPrtType, pucBuff, usLen)
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
**************************************************************************
*/
#if defined(PmgPrint)
USHORT  PmgPrint_Special(USHORT usPrtType, TCHAR *pucBuff, USHORT usLen);
USHORT  PmgPrint_Debug (USHORT usPrtType, TCHAR *pucBuff, USHORT usLen, char *aszFilePath, int nLineNo)
{
#undef PmgPrint
	char xBuff[128];
	int  iLen = strlen(aszFilePath);

	if (iLen > 30) iLen -= 30; else iLen = 0;
	sprintf (xBuff, "PmgPrint_Debug(): usPrtType 0x%x, File %s, Line %d", usPrtType, aszFilePath + iLen, nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff);
	return PmgPrint_Special(usPrtType, pucBuff, usLen);
}
USHORT  PmgPrint_Special(USHORT usPrtType, TCHAR *pucBuff, USHORT usLen)
#else
USHORT  PmgPrint(USHORT usPrtType, TCHAR *pucBuff, USHORT usLen)
#endif
{
	USHORT usFunc = 0;

	if ((usPrtType & PMG_PRT_WEB_MASK) != PMG_PRT_WEB_FILE)
	{
		usFunc = BlFwIfGetPrintType();
		if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgPrint) {
			return printerfuncs[usFunc].PmgPrint (usPrtType,pucBuff, usLen);
		}
	}
	else
	{
		return printerfuncs[PMG_WEB_FILE].PmgPrint (usPrtType,pucBuff, usLen);
	}
	return PMG_ERROR_PROVIDE;
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgDblShrdPrint(usPrtType, pucBuff, usLen)
*               input   USHORT  usPrtType;  : type of printer
*               input   UCHAR   *pucBuff;   : pointer to print buffer
*               input   USHORT  usLen;      : print data length
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*+

**Description:  Insert print data into spool buffer
*				
*				This functions third parameter, usLen, should specify 
*				the number of characters within the string, pucBuff.
*
*					----- NOT THE NUMBER OF BYTES!!! -----
*
**************************************************************************
*/
USHORT  PmgDblShrdPrint(USHORT usPrtType, TCHAR *pucBuff, USHORT usLen)
{
	USHORT usFunc = 0;
	USHORT retCode;
	TRANITEMIZERS       *WorkTI;

	usFunc = BlFwIfGetPrintType();
	if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgPrint) {

		TrnGetTI(&WorkTI);
		WorkTI->blDblShrdPrinting = TRUE;
		TrnPutTI(WorkTI);

		retCode = printerfuncs[usFunc].PmgPrint (usPrtType, pucBuff, usLen);

//		TrnGetTI(&WorkTI);
//		WorkTI->blDblShrdPrinting = FALSE;
//		TrnPutTI(WorkTI);

		return retCode;
	}

	return PMG_ERROR_PROVIDE;
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgPrintf(usPrtType, pszFormat, ...)
*               input   USHORT  usPrtType;  : type of printer
*               input   UCHAR   *pszFormat; : pointer to print buffer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert formated print data into spool buffer
*
**************************************************************************
*/
USHORT PmgPrintf(USHORT usPrtType, const TCHAR FARCONST *pszFormat, ...)
{
	USHORT  usLen = 0;
	USHORT usFunc = 0;
    TCHAR   pszTmpBuff[MAX_PRINT_CHARACTER*2];

	if ((usPrtType & PMG_PRT_WEB_MASK) != PMG_PRT_WEB_FILE)
	{
		usFunc = BlFwIfGetPrintType();
		if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgPrint) {
			usLen = _RflFormStr(pszFormat, (SHORT *)(&pszFormat + 1), pszTmpBuff, 255);   // PmgPrintf() function to print format
			return printerfuncs[usFunc].PmgPrint (usPrtType, pszTmpBuff, usLen);
		}
	}
	else
	{
		return printerfuncs[PMG_WEB_FILE].PmgPrint (usPrtType, pszTmpBuff, usLen);
	}
	return PMG_ERROR_PROVIDE;
}

/*
**************************************************************************
*
**Synopsis:     VOID    PmgWait( VOID )
*
**Return:       non
*
**Description:  Wait for empty the spool buffer
*
**************************************************************************
*/

VOID PmgWait( VOID )
{
	USHORT usFunc = 0;

	usFunc = BlFwIfGetPrintType();
	if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgWait) {
		printerfuncs[usFunc].PmgWait ();
	}
}



/*
**************************************************************************
*
**Synopsis:     USHORT  PmgBeginSmallValidation(usPrtType)
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

USHORT PmgBeginSmallValidation(USHORT usPrtType)
{
	USHORT usFunc = 0;

	if ((usPrtType & PMG_PRT_WEB_MASK) != PMG_PRT_WEB_FILE)
	{
		usFunc = BlFwIfGetPrintType();
		if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgBeginSmallValidation) {
			return printerfuncs[usFunc].PmgBeginSmallValidation (usPrtType);
		}
	}
	else
	{
	}
	return PMG_ERROR_PROVIDE;
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgEndSmallVlidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Reset Small Validation flag to none
*
**************************************************************************
*/

USHORT PmgEndSmallValidation(USHORT usPrtType)
{
	USHORT usFunc = 0;

	if ((usPrtType & PMG_PRT_WEB_MASK) != PMG_PRT_WEB_FILE)
	{
		usFunc = BlFwIfGetPrintType();
		if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgEndSmallValidation) {
			return printerfuncs[usFunc].PmgEndSmallValidation (usPrtType);
		}
	}
	else
	{
	}
	return PMG_ERROR_PROVIDE;
}


/*
**************************************************************************
*
**Synopsis:     USHORT  PmgBeginValidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert begin of validation into spool buffer
*
**************************************************************************
*/

USHORT PmgBeginValidation(USHORT usPrtType)
{
	USHORT usFunc = 0;

	if ((usPrtType & PMG_PRT_WEB_MASK) != PMG_PRT_WEB_FILE)
	{
		usFunc = BlFwIfGetPrintType();
		if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgBeginValidation) {
			return printerfuncs[usFunc].PmgBeginValidation (usPrtType);
		}
	}
	else
	{
	}
	return PMG_ERROR_PROVIDE;
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgEndVlidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert end of validation into spool buffer
*
**************************************************************************
*/

USHORT PmgEndValidation(USHORT usPrtType)
{
	USHORT usFunc = 0;

	if ((usPrtType & PMG_PRT_WEB_MASK) != PMG_PRT_WEB_FILE)
	{
		usFunc = BlFwIfGetPrintType();
		if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgEndValidation) {
			return printerfuncs[usFunc].PmgEndValidation (usPrtType);
		}
	}
	else
	{
	}
	return PMG_ERROR_PROVIDE;
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgFeed(usPrtType, usLine)
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

#if defined(PmgFeed)
USHORT  PmgFeed_Special(USHORT usPrtType, USHORT usLine);
USHORT  PmgFeed_Debug (USHORT usPrtType, USHORT usLine, char *aszFilePath, int nLineNo)
{
#undef PmgFeed
	char xBuff[128];
	int  iLen = strlen(aszFilePath);

	if (iLen > 30) iLen -= 30; else iLen = 0;
	sprintf (xBuff, "PmgFeed_Debug(): usPrtType 0x%x, File %s, Line %d", usPrtType, aszFilePath + iLen, nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff);
	return PmgFeed_Special(usPrtType, usLine);
}
USHORT  PmgFeed_Special(USHORT usPrtType, USHORT usLine)
#else
USHORT PmgFeed(USHORT usPrtType, USHORT usLine)
#endif
{
	USHORT usFunc = 0;
	if ((usPrtType & PMG_PRT_WEB_MASK) != PMG_PRT_WEB_FILE)
	{
		usFunc = BlFwIfGetPrintType();
		if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgFeed) {
			return printerfuncs[usFunc].PmgFeed (usPrtType,usLine);
		}
	}
	else
	{
	}
	return PMG_ERROR_PROVIDE;
}

     
/*
**************************************************************************
*
**Synopsis:     USHORT  PmgBeginImportant(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert begin of important lines into spool buffer
*
**************************************************************************
*/

USHORT PmgBeginImportant(USHORT usPrtType)
{
	USHORT usFunc = 0;

	if ((usPrtType & PMG_PRT_WEB_MASK) != PMG_PRT_WEB_FILE)
	{
		usFunc = BlFwIfGetPrintType();
		if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgBeginImportant) {
			return printerfuncs[usFunc].PmgBeginImportant (usPrtType);
		}
	}
	else
	{
	}
	return PMG_ERROR_PROVIDE;
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgEndImportant(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert end of important lines into spool buffer
*
**************************************************************************
*/

USHORT PmgEndImportant(USHORT usPrtType)
{
	USHORT usFunc = 0;

	if ((usPrtType & PMG_PRT_WEB_MASK) != PMG_PRT_WEB_FILE)
	{
		usFunc = BlFwIfGetPrintType();
		if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgEndImportant) {
			return printerfuncs[usFunc].PmgEndImportant (usPrtType);
		}
	}
	else
	{
	}
	return PMG_ERROR_PROVIDE;
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgGetStatus(usPrtType, pfbStatus)
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
#if defined(PmgGetStatus)
USHORT PmgGetStatus_Special(USHORT usPrtType, USHORT *pfbStatus);
USHORT  PmgGetStatus_Debug( USHORT usPrtType, USHORT *pfbStatus, char *aszFilePath, int nLineNo )
{
#undef PmgGetStatus
	char xBuff[128] = {0};
	int  iLen = strlen(aszFilePath);

	if (iLen > 30) iLen -= 30; else iLen = 0;
	sprintf (xBuff, "PmgGetStatus_Debug(): usPrtType 0x%x, File %s, Line %d", usPrtType, aszFilePath + iLen, nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff);
	return PmgGetStatus_Special(usPrtType, pfbStatus);
}

USHORT PmgGetStatus_Special(USHORT usPrtType, USHORT *pfbStatus)
#else
USHORT PmgGetStatus(USHORT usPrtType, USHORT *pfbStatus)
#endif
{
	USHORT usFunc = 0;

	if ((usPrtType & PMG_PRT_WEB_MASK) != PMG_PRT_WEB_FILE)
	{
		usFunc = BlFwIfGetPrintType();
		if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgGetStatus) {
			return printerfuncs[usFunc].PmgGetStatus (usPrtType, pfbStatus);
		}
	}
	else
	{
	}
	return PMG_ERROR_PROVIDE;
}

#if defined(PmgGetStatusOnly)
USHORT PmgGetStatusOnly_Special(USHORT usPrtType, USHORT *pfbStatus);
USHORT  PmgGetStatusOnly_Debug(USHORT usPrtType, USHORT *pfbStatus, char *aszFilePath, int nLineNo)
{
#undef PmgGetStatusOnly
	char xBuff[128] = {0};
	int  iLen = strlen(aszFilePath);

	if (iLen > 30) iLen -= 30; else iLen = 0;
	sprintf (xBuff, "PmgGetStatusOnly_Debug(): usPrtType 0x%x, File %s, Line %d", usPrtType, aszFilePath + iLen, nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff);
	return PmgGetStatusOnly_Special(usPrtType, pfbStatus);
}

USHORT PmgGetStatusOnly_Special(USHORT usPrtType, USHORT *pfbStatus)
#else
USHORT PmgGetStatusOnly(USHORT usPrtType, USHORT *pfbStatus)
#endif
{
	USHORT usFunc = 0;

	if ((usPrtType & PMG_PRT_WEB_MASK) != PMG_PRT_WEB_FILE)
	{
		usFunc = BlFwIfGetPrintType();
		if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgGetStatusOnly) {
			return printerfuncs[usFunc].PmgGetStatusOnly (usPrtType, pfbStatus);
		}
	}
	else
	{
	}
	return PMG_ERROR_PROVIDE;
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgPrtConfig(usPrtType, pusColumn, pfbStatus)
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

USHORT PmgPrtConfig(USHORT usPrtType, USHORT *pusColumn, USHORT *pfbStatus)
{
	USHORT usFunc = 0;

	if ((usPrtType & PMG_PRT_WEB_MASK) != PMG_PRT_WEB_FILE)
	{
		usFunc = BlFwIfGetPrintType();
		if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgPrtConfig) {
			return printerfuncs[usFunc].PmgPrtConfig (usPrtType, pusColumn, pfbStatus);
		}
	}
	else
	{
	}
	return PMG_ERROR_PROVIDE;
}

/*
**************************************************************************
*
**Synopsis:     USHORT PmgCallBack(DWORD dwErrorCode)
*               input   DWORD dwErrorCode;  : Error Code
*
**Return:       None
*
**Description:  Error Notify.
*
**************************************************************************
*/
USHORT PmgCallBackDisplayStatus (DWORD dwErrorCode)
{
	USHORT  usErrorMsg = 0;

	if (dwErrorCode != DEVICEIO_PRINTER_MSG_SUCCESS_ADR) {
		USHORT  usKeyCode;
		USHORT  usPrevious;

		switch (dwErrorCode) {
		case DEVICEIO_PRINTER_MSG_VSLPINS_ADR:
			usErrorMsg = LDT_VSLPINS_ADR;
			break;
		case DEVICEIO_PRINTER_MSG_VSLPREMV_ADR:
			usErrorMsg = LDT_VSLPREMV_ADR;
			break;
		case DEVICEIO_PRINTER_MSG_PRTTBL_ADR:
			usErrorMsg = LDT_PRTTBL_ADR;
			break;
		case DEVICEIO_PRINTER_MSG_COVEROPEN_ADR:
			usErrorMsg = LDT_PRT_COVEROPEN_ADR;
			break;
		case DEVICEIO_PRINTER_MSG_PAPEROUT_ADR:
			usErrorMsg = LDT_PRT_PAPERCHNG_ADR;
			break;
		case DEVICEIO_PRINTER_MSG_PAPERCHG_ADR:
			usErrorMsg = LDT_PAPERCHG_ADR;
			break;
		case DEVICEIO_PRINTER_MSG_JNLLOW_ADR:
			usErrorMsg = LDT_JNLLOW_ADR;
			break;
		default:
			usErrorMsg = LDT_PRTTBL_ADR;
			break;
		}

		/* allow power switch at error display */
		usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
		usKeyCode = UieErrorMsg(usErrorMsg, UIE_WITHOUT);
		PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
	}

	return usErrorMsg;
}

#if defined(PmgCallBack)
USHORT  PmgCallBack_Special(DWORD dwErrorCode, SHORT  usSilent);
USHORT  PmgCallBack_Debug (DWORD dwErrorCode, SHORT  usSilent, char *aszFilePath, int nLineNo)
{
	char xBuff[128];
	int  iLen = strlen(aszFilePath);

	if (iLen > 30) iLen -= 30; else iLen = 0;
	sprintf (xBuff, "PmgCallBack_Debug(): dwErrorCode %d, File %s, Line %d", dwErrorCode, aszFilePath + iLen, nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff);
	return PmgCallBack_Special(dwErrorCode, usSilent);
}
USHORT  PmgCallBack_Special(DWORD dwErrorCode, SHORT  usSilent)
#else
USHORT  PmgCallBack(DWORD dwErrorCode, SHORT  usSilent)
#endif
{
	BOOL       bCallBackPrinterOffline = ((flDispRegDescrControl & PRT_OFFLINE_CNTRL) != 0);
	USHORT     usErrorMsg = 0;
	USHORT     usRetCode = 0;

	if(dwErrorCode == DEVICEIO_PRINTER_MSG_SUCCESS_ADR){
		flDispRegDescrControl &= ~PRT_OFFLINE_CNTRL;
		flDispRegKeepControl &= ~PRT_OFFLINE_CNTRL;
		RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_1);
		bCallBackPrinterOffline = ((flDispRegDescrControl & PRT_OFFLINE_CNTRL) != 0);
	}else{
		//TRUE = online, FALSE = offline or error

		if(! bCallBackPrinterOffline && ! usSilent) {
			usErrorMsg = PmgCallBackDisplayStatus (dwErrorCode);
		}

		if((dwErrorCode == DEVICEIO_PRINTER_MSG_PRTTBL_ADR) ||
		   (dwErrorCode == DEVICEIO_PRINTER_MSG_COVEROPEN_ADR) ||
		   (dwErrorCode == DEVICEIO_PRINTER_MSG_PAPEROUT_ADR) ||
		   (dwErrorCode == DEVICEIO_PRINTER_MSG_POWEROFF_ADR))
		{
			flDispRegDescrControl |= PRT_OFFLINE_CNTRL;
			flDispRegKeepControl |= PRT_OFFLINE_CNTRL;
			RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_1);
		}
	}

	dwCallBackLastErrorCode = dwErrorCode;   // save the last error code received
	bCallBackLastPrinterOffline = bCallBackPrinterOffline;
    return usErrorMsg;
}


BOOL PmgCallBackGetLastStatus (DWORD *pdwLastErrorCode)
{
	if (pdwLastErrorCode) {
		*pdwLastErrorCode = dwCallBackLastErrorCode;
	}

	return ! bCallBackLastPrinterOffline;
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgFont(usPrtType, usFont)
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
USHORT  PmgFont( USHORT usPrtType, USHORT usFont)
{
	USHORT usFunc = 0;

	if ((usPrtType & PMG_PRT_WEB_MASK) != PMG_PRT_WEB_FILE)
	{
		usFunc = BlFwIfGetPrintType();
		if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgFont) {
			return printerfuncs[usFunc].PmgFont (usPrtType, usFont);
		}
	}
	else
	{
	}
	return PMG_ERROR_PROVIDE;
}

/*
**************************************************************************
*
**Synopsis:     LONG  PmgCapSet (usPrtType, usPrtCap, lValue)
*               input   USHORT  usPrtType;  : type of printer
*               inout   USHORT  usFont;     : type of font
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*               PMG_ERROR_COMM      : communication error
*               previous value of the capability specified
*
**Description:  Set printer capability such as line spacing returning previouis value.
*
*
**************************************************************************
*/
LONG  PmgGetSetCap( USHORT usPrtType, USHORT usPrtCap, LONG lValue)
{
	USHORT usFunc = 0;

	if ((usPrtType & PMG_PRT_WEB_MASK) != PMG_PRT_WEB_FILE)
	{
		usFunc = BlFwIfGetPrintType();
		if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgGetSetPrtCap) {
			return printerfuncs[usFunc].PmgGetSetPrtCap (usPrtType, usPrtCap, lValue);
		}
	}
	else
	{
	}
	return PMG_ERROR_PROVIDE;
}

/*
**************************************************************************
*
**Synopsis:     VOID  PmgPrintBarCode(USHORT usPrtType, TCHAR *pucBuff, ULONG ulTextFlags, ULONG ulCodeFlags)
*               input   USHORT  usPrtType;  : type of printer
*               inout   ULONG ulTextFlags;  : type of text manipulation such as alignment
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*               PMG_ERROR_COMM      : communication error
*               previous value of the capability specified
*
**Description:  Print out a text string as a bar code.
*
*
**************************************************************************
*/
USHORT  PmgPrintBarCode(USHORT usPrtType, TCHAR *pucBuff, ULONG ulTextFlags, ULONG ulCodeFlags)
{
	USHORT usFunc = 0;

	if ((usPrtType & PMG_PRT_WEB_MASK) != PMG_PRT_WEB_FILE)
	{
		usFunc = BlFwIfGetPrintType();
		if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgPrintBarCode) {
			return printerfuncs[usFunc].PmgPrintBarCode (usPrtType, pucBuff, ulTextFlags, ulCodeFlags);
		}
	}
	else
	{
	}
	return PMG_ERROR_PROVIDE;
}


/*
**************************************************************************
*
**Synopsis:     VOID    PmgSetValWaitCount(USHORT usRetryCount)
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

VOID PmgSetValWaitCount(USHORT usRetryCount)
{
	USHORT usFunc = 0;

	usFunc = BlFwIfGetPrintType();
	if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgSetValWaitCount) {
		 printerfuncs[usFunc].PmgSetValWaitCount (usRetryCount);
	}
}   
/*
*********************************************************************
*
**Synopsis:     VOID    PmgFinalize(VOID)
*
**Return:       Non
*
**Description:  Print Manager Finalize
*
*********************************************************************
*/

VOID PmgBeginTransaction( SHORT sType, ULONG ulTransNo )
{
	USHORT usFunc = 0;

	usFunc = BlFwIfGetPrintType();
	if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgBeginTransaction) {
		printerfuncs[usFunc].PmgBeginTransaction (sType,ulTransNo);
	}
	return;
}
/*
*********************************************************************
*
**Synopsis:     VOID    PmgFinalize(VOID)
*
**Return:       Non
*
**Description:  Print Manager Finalize
*
*********************************************************************
*/

VOID PmgEndTransaction( SHORT sType, ULONG ulTransNo )
{
	USHORT usFunc = 0;

	usFunc = BlFwIfGetPrintType();
	if (usFunc && usFunc <= MAX_PRINTER_TYPE && printerfuncs[usFunc].PmgEndTransaction) {
		printerfuncs[usFunc].PmgEndTransaction (sType,ulTransNo);
	}
	return;
}

/***    End of PMGIF.C    ***/
