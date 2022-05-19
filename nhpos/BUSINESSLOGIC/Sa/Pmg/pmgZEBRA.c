					/* NOTICE!!!
					   When working with the Zebra Printer(s) ,you may recieve an error in the windows
					   event log, concerning "failed bluetooth authentication". If this occurs you'll
					   need to use a program called "Label Vista" to configure the bluetooth settings 
					   of the Zebra printer (You may need to set a bluetooth pin/passkey). a copy of 
					   Label Vista can be found on the N:\ drive in the "ZebraPrinters" folder 
					   ( N:\Setups_SDKs_Installers\ZebraPrinters ). A 5-pin micro USB connector must be used to 
					   connect the zebra printer to the PC when using Label Vista. - TLDR
					   */



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
*$Date$ 02.19.2008
*$Author$ Philip Dinu
*$Log$ In Progress
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
#include	"BlFWif.h"
#include	"display.h"
//#include    <string.h>


#define PTR_S_JOURNAL        1L
#define PTR_S_RECEIPT        2L
#define PTR_S_SLIP           4L

PMGBLK  PmgZEBRABlk[MAX_PRINTER];

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
**Synopsis:     VOID    pmgZEBRAWinInitialize(VOID)
*
**Return:       Non
*
**Description:  Print Manager Start up
*
*********************************************************************
*/

VOID PmgZEBRAInitialize( VOID )
{
    DWORD   dwCount, dwRet, i;
    TCHAR   atchDeviceName[SCF_USER_BUFFER];
    TCHAR   atchDllName[SCF_USER_BUFFER];

    for (i = 0; i < MAX_PRINTER; i++)
    {
        PmgZEBRABlk[i].dwDllLoaded = FALSE;
    }

    dwRet = ScfGetActiveDevice(SCF_TYPENAME_PRINTER, &dwCount, atchDeviceName, atchDllName);

    if (dwRet != SCF_SUCCESS) return;

    for (i = 0; i < dwCount; i++)
    {
        PmgZEBRABlk[i].dwDllLoaded = TRUE;
    }


    return;
}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgZEBRAFinalize(VOID)
*
**Return:       Non
*
**Description:  Print Manager Finalize
*
*********************************************************************
*/

VOID PmgZEBRAFinalize( VOID )
{
	//device engine should close
#if 0
    DWORD   i;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgZEBRABlk[i].dwDllLoaded)
        {
            PmgZEBRABlk[i].Func.Close(PmgZEBRABlk[i].hHandle);
            FreeDevice();
        }
    }
#endif
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgZEBRAPrint(usPrtType, pucBuff, usLen)
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
USHORT  PmgZEBRAPrint(USHORT usPrtType, TCHAR *pucBuff, USHORT usLen)
{

	TCHAR  *pStop, *pPrtType, *pPrtTypeNormal;
	//TCHAR  *pPrtRight = _T("! U1 RIGHT\r\n");
	TCHAR  *pPrtRight = _T("! U1 RIGHT\r\n");
	TCHAR  *pPrtLeft = _T("! U1 LEFT\r\n");
	TCHAR  *pPrtCenter = _T("! U1 CENTER\r\n");
	TCHAR  *pPrtNormal = _T("! U1 SETLP 0 2 18\r\n! U1 SETLF 18\r\n! U1 SETSP 2\r\n");
	TCHAR  *pPrtDouble = _T("! U1 SETLP 0 3 18\r\n! U1 SETLF 18\r\n! U1 SETSP 2\r\n");
	TCHAR  *pPrtDoubleS = _T("! U1 SETLP 0 3 18\r\n! U1 SETLF 18\r\n! U1 SETSP 2\r\n");
//	TCHAR  *pPrtDoubleS = _T("! U1 SETLP 7 3 60\r\n! U1 SETSP 5\r\n");
	TCHAR  *pPrtNormal2 = _T("! U1 SETLP 0 2 18\r\n! U1 SETLF 18\r\n! U1 SETSP 2\r\n");
	TCHAR  *pPrtNormal3 = _T("! U1 SETLP 0 2 18\r\n! U1 SETLF 18\r\n! U1 SETSP 2\r\n");
	TCHAR  prtBuff[MAX_PRINT_BUFFER] = {0};
	
	ULONG	ulTransNo;
	int    i, j, k;
	BOOL   bS, bD, bSD, bDoubleWideJustify = FALSE;
	USHORT  fbPrtStatus = 0;                /* printer status */
	USHORT  usSingleCharCount = 0;     // count of single characters, double wide characters count as 2
	SHORT	sType;
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

	bS = TRUE;
	bD = bSD = FALSE;

	memset(prtBuff, 0x00, sizeof(prtBuff));

	// We don't need all of those null strings
	if ( _tcscmp(pucBuff, _T("")) == 0 )return 0; // This constant is never looked at and it is entirely arbitrary -TLDJR

		
	// Ensure that we are starting with the normal size font.
	// We may need to modify the line height if there are large
	// characters in the line of text.  So first of all, scan
	// the line checking for special characters and change the
	// line height to be that size.
	pPrtTypeNormal = pPrtNormal;
	for (j = 0; j < usLen; j++) {
		if (pucBuff[j] == PRT_SDOUBLE && pPrtTypeNormal != pPrtNormal3)
			pPrtTypeNormal = pPrtNormal3;
		if (pucBuff[j] == PRT_DOUBLE && pPrtTypeNormal == pPrtNormal)
			pPrtTypeNormal = pPrtNormal2;
	}

	pPrtType = pPrtTypeNormal;
	k = 0;
	while (*pPrtType) {
		prtBuff[k++] = *pPrtType++;
	}

	pPrtType = pPrtLeft;
	while (*pPrtType) {
		prtBuff[k++] = *pPrtType++;
	}

    if(pucBuff[0] == ESC) {
		// If we hit the "Paper Cut" escape sequence from PrtCut()
		// then send the Esc CUT sentinel value to the
		// device engine, this will send the entire
		// buffer to the printer
		if (_tcscmp(pucBuff,_T("\x1B|75P")) == 0 || _tcscmp(pucBuff,_T("\x1B|75fP")) == 0)
		{
			prtBuff[0] = _T('\x1B');
			prtBuff[1] = _T('C');
			prtBuff[2] = _T('U');
			prtBuff[3] = _T('T');
			prtBuff[4] = 0x00;
		}
		else if (_tcscmp(pucBuff,_T("\x1B|1lF")) == 0 )
		{
//			prtBuff[0] = 0x00;
		}
		else
		{
			// In some cases OPOS printer codes are embedded directly
			// into the output.  We will ignore those printer codes
			// when using the Zebra portable printer such as the RW 420.
			//prtBuff[k++] = 0x0D;//put return after each line
			//prtBuff[k++] = 0x0A;//put line feed
			return(PMG_SUCCESS);
		}		
	}
	else if(pucBuff[0]){
		for(j = 0; j < usLen; j++){
			switch(pucBuff[j]){
				case PRT_SDOUBLE://Double High and Wide
					if(!bSD){
						bSD = TRUE;
						bD = FALSE;
						bS = FALSE;
						pPrtType = pPrtDoubleS;

						//Loop inserts printer command for double high 
						//double wide before this type of char
						while (*pPrtType) {		
							prtBuff[k++] = *pPrtType++;
						}
					}
					break;

				case PRT_DOUBLE://Double Wide
					if(!bD){
						bSD = FALSE;
						bD = TRUE;
						bS = FALSE;
						pPrtType = pPrtDouble;

						//Loop inserts printer command for double wide 
						//before this type of char
						while (*pPrtType) {
							prtBuff[k++] = *pPrtType++;
						}
					}
					break;

				case PRT_SINGLE:
					if(!bS){
						bSD = FALSE;
						bD = FALSE;
						bS = TRUE;
						pPrtType = pPrtTypeNormal;

						//Loop inserts printer command for single char
						//before this type of char
						while (*pPrtType) {
							prtBuff[k++] = *pPrtType++;
						}
					}
					break;

				case 9://Tab, so Right Justify
					prtBuff[k++] = 9;
					bDoubleWideJustify = (!bS);  // indicate if we should use double wide when doing justify spaces
//					prtBuff[k++] = _T('|');
//					prtBuff[k++] = _T('r');
//					prtBuff[k++] = _T('A');
					break;

				case 10: //line feed so insert char return line feed for printer
					prtBuff[k++] = 0x0D;//put return after each line
					prtBuff[k++] = 0x0A;//put line feed
					break;

				case PRT_LOGO:
//					prtBuff[k++] = ESC;
//					prtBuff[k++] = _T('|');
//					prtBuff[k++] = _T('1');
//					prtBuff[k++] = _T('B');
					break;

				case PRT_CENTERED:
					pPrtType = pPrtCenter;

					//Loop inserts printer command for printing characters
					//centered before this type of char
					while (*pPrtType) {
						prtBuff[k++] = *pPrtType++;
					}
					break;

				case PRT_TRANS_BGN:
					sType = pucBuff[++j];
					ulTransNo = _tcstoul(&pucBuff[++j],&pStop, 10);
					BlFwIfPrintTransStart(PMG_MAKE_FULL_PRTTYPE(PMG_ZEBRA_PRINTER,sType), ulTransNo);
					return(PMG_SUCCESS);

				case PRT_TRANS_END:
					sType = pucBuff[++j];
					ulTransNo = _tcstoul(&pucBuff[++j],&pStop, 10);
					BlFwIfPrintTransEnd(PMG_MAKE_FULL_PRTTYPE(PMG_ZEBRA_PRINTER,sType), ulTransNo);
					return(PMG_SUCCESS);
					break;

				case PRT_SEPARATOR: // Don't print the separator character for condiments (ASCII 0xFF) - TLDJR
					prtBuff[k++] = PRT_SPACE;
					break;

				default:
					// We are counting characters in the output line so that if the line
					// contains a tab then we can do a right justify by inserting spaces.
					if (bS)
						usSingleCharCount += 1;
					else if (bD)
						usSingleCharCount += 2;
					else
						usSingleCharCount += 2;
					prtBuff[k++] = pucBuff[j];
					break;
				}
		}
	}
	
    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgZEBRABlk[i].dwDllLoaded)
        {
			//PrintTwoNormal(not implemented) can be used to print to two stations
			//But right now only print to receipt if both selected
			/*
				NOTE: Here we are using wcslen of the prtBuff instead of the
				passed in string length 'uslen' because we did formatting in
				this function that may have added or removed characters from 
				the original string. This value is casted to a ushort to be 
				compatible with the usage of the function, although the length
				of the string will not be greater than 512 (the size of the buffer).
			*/

			if ( ( usPrtTypeMasked == PMG_PRT_RECEIPT) ||
				 ( usPrtTypeMasked == PMG_PRT_RCT_JNL) ||
				 ( usPrtTypeMasked == PMG_PRT_VALIDATION) ||
				 ( usPrtTypeMasked == PMG_PRT_SLIP) ) {

					PmgZEBRAGetStatus(usPrtType, &fbPrtStatus);

					if ( fbPrtStatus & PRT_STAT_POWER_OFF) {
						return(PMG_ERROR_PRINTER);
					}
			}
			
			/*Here is where the right justify takes place.  PMG_THERMAL_CHAR_D is the max char length
			of the Zebra RW 220 printer which is 40 char.  pTab finds the first occurence of the tab 
			character, 9.  endBuffer points to the end of the line buffer and endString points to the 
			end of the string to be printed.
			Basically going backward up the string each char is moved to the end of the line until the 
			to create the tab char is reached then blank spaces are filled in the remaining spaces.*/
			
			if(usSingleCharCount + 1 < PMG_THERMAL_CHAR_D){
				TCHAR  *endBuffer, *endString, *pTab, endCharsToInsert;
				pTab = _tcschr(prtBuff, 9);
				if(pTab){
					endCharsToInsert = (PMG_THERMAL_CHAR_D - 1 - usSingleCharCount);

					if(bDoubleWideJustify){
						endCharsToInsert /= 2;
						endCharsToInsert += 2;
					}

					endBuffer = prtBuff + _tcslen(prtBuff) + endCharsToInsert;
					
					endString = prtBuff + _tcslen(prtBuff);
					for( ; endString > pTab ; ){
						*endBuffer-- = *endString--;	
					}

					while(endBuffer >= pTab){
						*endBuffer-- = ' ';
					}
				}
			}

			switch(usPrtTypeMasked){ 
			case PMG_PRT_RECEIPT: /* receipt station          */
			case PMG_PRT_RCT_JNL: /* receipt & journal        */
				BlFwIfPrint(PMG_MAKE_FULL_PRTTYPE(PMG_ZEBRA_PRINTER,PTR_S_RECEIPT), prtBuff, (USHORT) wcslen(prtBuff), 0);
				break;
			//We take out the PMG_PRT_JOURNAL because we now do an electronic 
			//journal, and we have no printers that print to a printers journal
			//this case was used for when printers would have the paper that 
			//goes to the customer, and the internal roll that managers used
			//for internal purposes. JHHJ
#if EJPRINT
			case PMG_PRT_JOURNAL: /* journal station          */
				BlFwIfPrint(PMG_MAKE_FULL_PRTTYPE(PMG_ZEBRA_PRINTER,PTR_S_JOURNAL), prtBuff, (USHORT) wcslen(prtBuff), 0);
				break;
#endif
			case PMG_PRT_VALIDATION: /* validation               */
			case PMG_PRT_SLIP: /* slip printer             */
				BlFwIfPrint(PMG_MAKE_FULL_PRTTYPE(PMG_ZEBRA_PRINTER,PTR_S_SLIP), prtBuff, (USHORT) wcslen(prtBuff), 0);
				break;
			 /* local file printing				*/
			case PMG_PRT_FILE_PRG:
				BlFwIfPrintFileWrite( prtBuff, (USHORT) wcslen(prtBuff), BL_PRTFILE_PROG);
				break;
			case PMG_PRT_FILE_SUP:
				BlFwIfPrintFileWrite( prtBuff, (USHORT) wcslen(prtBuff), BL_PRTFILE_SUPER);
				break;
			case PMG_PRT_FILE_REG:
				BlFwIfPrintFileWrite( prtBuff, (USHORT) wcslen(prtBuff), BL_PRTFILE_REG);
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
**Synopsis:     VOID    PmgZEBRAWait( VOID )
*
**Return:       non
*
**Description:  Wait for empty the spool buffer
*
**************************************************************************
*/

VOID PmgZEBRAWait( VOID )
{

	return;//RPH just return for now 4-5-4
}



/*
**************************************************************************
*
**Synopsis:     USHORT  PmgZEBRABeginSmallValidation(usPrtType)
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

USHORT PmgZEBRABeginSmallValidation(USHORT usPrtType)
{
#if 0
    DWORD   i;
    DEVICEIO_PRINTER_ARG_VALIDATION Arg;

    /* Initialize */
    Arg.wPrintStation = usPrtType;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgZEBRABlk[i].dwDllLoaded)
        {
			BlFwBeginInsertion(2000);
			BlFwEndInsertion();
        }
    }
#endif
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgZEBRAEndSmallVlidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Reset Small Validation flag to none
*
**************************************************************************
*/

USHORT PmgZEBRAEndSmallValidation(USHORT usPrtType)
{
#if 0
    DWORD   i;
    DEVICEIO_PRINTER_ARG_END_VALIDATION Arg;

    /* Initialize */
    Arg.wPrintStation = usPrtType;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgZEBRABlk[i].dwDllLoaded)
        {
			BlFwBeginRemoval(2000);
			BlFwEndRemoval();
        }

    }
#endif
    return(PMG_SUCCESS);
}


/*
**************************************************************************
*
**Synopsis:     USHORT  PmgZEBRABeginValidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert begin of validation into spool buffer
*
**************************************************************************
*/

USHORT PmgZEBRABeginValidation(USHORT usPrtType)
{
#if 0
    DWORD   i;
    DEVICEIO_PRINTER_ARG_VALIDATION Arg;

    /* Initialize */
    Arg.wPrintStation = usPrtType;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgZEBRABlk[i].dwDllLoaded)
        {
			BlFwBeginInsertion(2000);
			BlFwEndInsertion();
        }
    }
#endif
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgZEBRAEndVlidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert end of validation into spool buffer
*
**************************************************************************
*/

USHORT PmgZEBRAEndValidation(USHORT usPrtType)
{
#if 0
    DWORD   i;
    DEVICEIO_PRINTER_ARG_END_VALIDATION Arg;

    /* Initialize */
    Arg.wPrintStation = usPrtType;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgZEBRABlk[i].dwDllLoaded)
        {
			BlFwBeginRemoval(2000);
			BlFwEndRemoval();
        }

    }
#endif
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgZEBRAFeed(usPrtType, usLine)
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

USHORT PmgZEBRAFeed(USHORT usPrtType, USHORT usLine)
{
    USHORT   i = 0, j;
	TCHAR	tchBuff[8];

	tchBuff[i++] = _T('\r');
	tchBuff[i++] = _T('\n');
	tchBuff[i] = 0x00;

    for (j = 0; j < MAX_PRINTER; j++)
    {
		PmgZEBRAPrint(usPrtType, tchBuff, i);
    }
    return(PMG_SUCCESS);
}

     
/*
**************************************************************************
*
**Synopsis:     USHORT  PmgZEBRABeginImportant(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert begin of important lines into spool buffer
*
**************************************************************************
*/

USHORT PmgZEBRABeginImportant(USHORT usPrtType)
{
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgZEBRAEndImportant(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert end of important lines into spool buffer
*
**************************************************************************
*/

USHORT PmgZEBRAEndImportant(USHORT usPrtType)
{
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgZEBRAGetStatus(usPrtType, pfbStatus)
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

USHORT PmgZEBRAGetStatus(USHORT usPrtType, USHORT *pfbStatus)
{
    DWORD   i;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgZEBRABlk[i].dwDllLoaded)
        {
			BlFwGetPrinterStatus(PMG_MAKE_FULL_PRTTYPE(PMG_ZEBRA_PRINTER,usPrtType), pfbStatus);	
			if(*pfbStatus & PRT_STAT_POWER_OFF)
			{
				PmgCallBack(DEVICEIO_PRINTER_MSG_POWEROFF_ADR, 0);
			}
		
		}
	}
    return(PMG_SUCCESS);
}

USHORT PmgZEBRAGetStatusOnly(USHORT usPrtType, USHORT *pfbStatus)
{
    DWORD   i;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgZEBRABlk[i].dwDllLoaded)
        {
			BlFwGetPrinterStatus(PMG_MAKE_FULL_PRTTYPE(PMG_ZEBRA_PRINTER,usPrtType), pfbStatus);	
		}
	}
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgZEBRAPrtConfig(usPrtType, pusColumn, pfbStatus)
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

USHORT PmgZEBRAPrtConfig(USHORT usPrtType, USHORT *pusColumn, USHORT *pfbStatus)
{
    DWORD   i;
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgZEBRABlk[i].dwDllLoaded)
        {
			switch(usPrtTypeMasked){
			case PMG_PRT_RECEIPT: /* receipt station          */
			case PMG_PRT_RCT_JNL: /* receipt & journal        */
				BlFwGetPrinterColumnsStatus(PMG_MAKE_FULL_PRTTYPE(PMG_ZEBRA_PRINTER,PTR_S_RECEIPT), pusColumn, pfbStatus);
				break;
			case PMG_PRT_JOURNAL: /* journal station          */
				BlFwGetPrinterColumnsStatus(PMG_MAKE_FULL_PRTTYPE(PMG_ZEBRA_PRINTER,PTR_S_JOURNAL), pusColumn, pfbStatus);
				break;
			case PMG_PRT_VALIDATION: /* validation               */
			case PMG_PRT_SLIP: /* slip printer             */
				BlFwGetPrinterColumnsStatus(PMG_MAKE_FULL_PRTTYPE(PMG_ZEBRA_PRINTER,PTR_S_SLIP), pusColumn, pfbStatus);
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
**Synopsis:     USHORT  PmgZEBRAFont(usPrtType, usFont)
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

USHORT  PmgZEBRAFont( USHORT usPrtType, USHORT usFont)
{
    DWORD   i;
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgZEBRABlk[i].dwDllLoaded)
        {
			switch(usPrtTypeMasked){
			case PMG_PRT_RECEIPT: /* receipt station          */
			case PMG_PRT_RCT_JNL: /* receipt & journal        */
				BlFwIfPrintSetFont(PMG_MAKE_FULL_PRTTYPE(PMG_ZEBRA_PRINTER,PTR_S_RECEIPT), usFont);
				break;
			case PMG_PRT_JOURNAL: /* journal station          */
				BlFwIfPrintSetFont(PMG_MAKE_FULL_PRTTYPE(PMG_ZEBRA_PRINTER,PTR_S_JOURNAL), usFont);
				break;
			case PMG_PRT_VALIDATION: /* validation               */
			case PMG_PRT_SLIP: /* slip printer             */
				BlFwIfPrintSetFont(PMG_MAKE_FULL_PRTTYPE(PMG_ZEBRA_PRINTER,PTR_S_SLIP), usFont);
				break;
			default:
				break;
			}

//            PmgZEBRABlk[i].Func.IoControl(
//                PmgZEBRABlk[i].hHandle, 
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
**Synopsis:     VOID    PmgZEBRASetValWaitCount(USHORT usRetryCount)
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

VOID PmgZEBRASetValWaitCount(USHORT usRetryCount)
{
    DWORD   i;

    /* Initialize */

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgZEBRABlk[i].dwDllLoaded)
        {
        }

    }
    return;
}   

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgZEBRAFont(usPrtType, usFont)
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

VOID  PmgZEBRABeginTransaction(SHORT sType, ULONG ulTransNo)
{
    
	BlFwIfPrintTransStart(PMG_MAKE_FULL_PRTTYPE(PMG_ZEBRA_PRINTER,sType), ulTransNo);

}
/*
**************************************************************************
*
**Synopsis:     USHORT  PmgZEBRAFont(usPrtType, usFont)
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

VOID  PmgZEBRAEndTransaction(SHORT sType, ULONG ulTransNo)
{

	BlFwIfPrintTransEnd(PMG_MAKE_FULL_PRTTYPE(PMG_ZEBRA_PRINTER,sType),ulTransNo);
    
}

/***    End of PMGIF.C    ***/


#if 0
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
/*
*********************************************************************
*
**Synopsis:     VOID    PmgZebraVersion(VOID)
*
**Input:		None
*
**Return:       None
*
**Description:  Displays the version number of the ZebraPrint.dll
*
*********************************************************************
*/

VOID PmgZebraVersion( VOID )
{
	DWORD i;

	for (i = 0; i < MAX_PRINTER; i++)
    {
        PmgOLEBlk[i].dwDllLoaded = FALSE;
    }

	for (i = 0; i < MAX_PRINTER; i++)
    {
        PmgOLEBlk[i].dwDllLoaded = TRUE;
    }
    
}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgZebraSerialConnection(LPCTSTR szPort, long lBaud)
*
**Input:		- szPort is the serial port (e.g. "COM1:", "COM2:", etc.)
*				- lBaud is the baud rate (e.g. 9600, 19200, 38400, 115200)
*
*
**Return:       None
*
**Description:  Declares you are connecting to the printer via a serial 
*				comm port or via a BlueTooth serial/modem connection.
*
*********************************************************************
*/

VOID PmgZebraSerialConnection( LPCTSTR szPort, long lBaud )
{

}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgZebraTcpConnection(LPCTSTR szHostname, long lPort)
*
**Input:		- szHostname is the ip address or hostname of the printer
*				- lPort is the TCP port number the printer is listening on
*
*
**Return:       None
*
**Description:  Declares you are connecting to the printer via TCP/IP
*
*********************************************************************
*/
VOID  PmgZebraTcpConnection(LPCTSTR szHostname, long iPort)
{
	
}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgZebraIrDAConnection()
*
**Input:		NONE
*
**Return:       None
*
**Description:  Declares you are connecting to the printer via the infrared (IrDA) port.
*
*********************************************************************
*/
VOID  PmgZebraIrDAConnection()
{
	
}

/*
*********************************************************************
*
**Synopsis:     LONG    PmgZebraPrint(LPCTSTR szData)
*
**Input:		- szData is the data to be sent to the printer
*
**Return:       This function returns PMG_SUCCESS on success and 
*				PMG_ERROR_PRINTER if the Print() command failed.  
*				Call ZebraPrint's GetLastError() function to get 
*				the error code.
*
**Description:  Print() will do an implicit Open() and Close() of the 
*				printer port, so it can be called immediately after 
*				declaring the printer connection.  It will also do error 
*				checking and will prompt for retry attempts (via dialog 
*				boxes) when an error condition is encountered.  Because 
*				it handles the Open() and Close() of the printer 
*				automatically and does error handling, it is recommended 
*				that Print() be used to send data to the printer. This 
*				function returns 1 on success and  0 if the Print() 
*				command failed.  Call ZebraPrint's GetLastError() 
*				function to get the error code.
*
*				NOTE: if you prefer this command does not do error 
*				checking or perform a status check before the print 
*				is performed, use the EnableStatusCheck(), 
*				SetStatusQueryTimeout(), and ShowPrompts() to 
*				modify the default behavior of this command. These 
*				commands are described below.
*
*********************************************************************
*/
ULONG  PmgZebraPrint(LPCTSTR szData)
{
	
// returns 1 on suceess, 0 on error  ***Different from NHPOS

}



/*********************************************************************
**********************************************************************
*																	 *
** If you require a lower level of interaction and/or				**
** control with the printer, the following methods					**
** are also supplied:												**
*																	 *
**********************************************************************
*********************************************************************/



/*
*********************************************************************
*
**Synopsis:     LONG    PmgZebraOpen()
*
**Input:		NONE
*
**Return:       This function returns PMG_SUCCESS on success and 
*				PMG_ERROR_PRINTER if PmgZebraOpen() failed. 
*
**Description:  Opens a connection to the printer. On failure, 
*				the error code encountered can be retrieved via 
*				a call to ZebraPrint's GetLastError() (see below).
*
*********************************************************************
*/
ULONG  PmgZebraOpen()
{
	
	// returns 1 on suceess, 0 on error  ***Different from NHPOS

}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgZebraClose()
*
**Input:		NONE
*
**Return:       NONE
*
**Description:  Closes the connection to the printer.
*
*********************************************************************
*/
VOID  PmgZebraClose()
{

}

/*
*********************************************************************
*
**Synopsis:     LONG    PmgZebraGetLastError()
*
**Input:		NONE
*
**Return:       LONG - Error Code
*
**Description:  Returns the numeric error code of the last error encountered by the ActiveX dll.
*
*********************************************************************
*/
ULONG  PmgZebraGetLastError()
{

}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgZebraShowLastError()
*
**Input:		NONE
*
**Return:       NONE
*
**Description:  This method displays a window with the last error code 
*				encountered, along with a text description. 
*
*********************************************************************
*/
VOID  PmgZebraShowLastError()
{

}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgZebraShowPrompts(long bEnablePrompts)
*
**Input:		- bEnablePrompts is either a 1 or 0 to specify whether or 
*				  not error messages and "retry" prompts should be displayed.
*
**Return:       NONE
*
**Description:  This method displays a window with the last error code 
*				encountered, along with a text description. 
*
*********************************************************************
*/
VOID  PmgZebraShowPrompts(long bEnablePrompts)
{

}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgZebraEnableStatusCheck(long bEnableCheck)
*
**Input:		- bEnableCheck specifies whether to enable (1) or 
*				  disable (0) the status checking
*
**Return:       NONE
*
**Description:  If status checking is enabled, the ZebraPrint ActiveX 
*				control will first verify the printer is available and 
*				in a "ready" state before it attempts to send data to 
*				the printer via the Print() command. Disabling status 
*				checking will allow printing of data without first 
*				querying the printer's status. 
*
*********************************************************************
*/
VOID  PmgZebraEnableStatusCheck(long bEnableCheck)
{

}

/*
*********************************************************************
*
**Synopsis:     LONG    PmgZebraSend(LPCTSTR data)
*
**Input:		- data is the null terminated string of data to be 
*				  sent to the printer
*
**Return:       LONG - Returns the number 
*					   of bytes successfully sent.
*
**Description:  Sends the data to the printer. Returns the number 
*				of bytes successfully sent. If the return value is 
*				less than the number of bytes in data, call 
*				GetLastError() to determine the error condition. 
*				This function requires the printer connection was 
*				previously opened via a call to Open().
*
*********************************************************************
*/
ULONG  PmgZebraSend(LPCTSTR data)
{

}

/*
*********************************************************************
*
**Synopsis:     LONG    PmgZebraGetPrinterStatus()
*
**Input:		NONE
*
**Return:       LONG - Status bits
*
**Description:  This command will query the printer for its current 
*				status and will return it as a bit field. A response 
*				of 0xFFFF0000 indicates no response from the printer.  
*				The following mask values indicate different status 
*				attributes of the printer:
*
*				LOWBATTERY_MASK	0x0008			/* printer status, low-battery bit 
*				HEADUP_MASK		0x0004			/* printer status, head-up bit   
*				PAPEROUT_MASK	0x0002			/* printer status, paper-out bit 
*				BUSY_MASK		0x0001			/* printer status, busy bit      
*
*
*********************************************************************
*/
ULONG  PmgZebraGetPrinterStatus()
{

}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgZebraSetStatusQueryTimeout(long lTimeout)
*
**Input:		- lTimeout specifies how long (in seconds) to wait for 
*				  a response from the printer
*
**Return:       NONE
*
**Description:  If status checking is enabled, this command will specify 
*				how long to wait until reporting an error that there is 
*				no response from the printer.  The default timeout is 
*				10 seconds.  The minimum allowed value is a timeout of 1 second.
*
*********************************************************************
*/
VOID  PmgZebraSetStatusQueryTimeout(long lTimeout)
{

}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgZebraEnablePrintVerify(long bVerify)
*
**Input:		- bVerify is a flag to specificy whether a confirmation 
*				  "reprint" prompt will be displayed after each Print()
*
**Return:       NONE
*
**Description:  This option is useful if you want to give the user a 
*				chance to reprint something when the printer 
*				encounters an error while it was printing  (such as 
*				running out of paper).   Pass a 1 to enable this 
*				feature.  By default this feature is off.
*
*********************************************************************
*/
VOID  PmgZebraEnablePrintVerify(long bVerify)
{

}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgZebraSerialCloseWait(long lWaitTime)
*
**Input:		- lWaitTime is the time in milliseconds to wait
*
**Return:       NONE
*
**Description:  This command will alter the default behavior of 
*				the Print() command when used with a serial port.  
*				It specifies the number of milliseconds to wait 
*				before closing the serial port after a Print().  
*				This is useful if you have configured the printer 
*				to use the PRESENT-AT command in a printer config.sys 
*				file.  You should ensure the wait time chosen exceeds 
*				the time delay specified with the PRESENT-AT command.  
*				For information on PRESENT-AT, please refer to the CPL 
*				programmer's guide.  NOTE: This command only affects 
*				serial printing - it does not apply to IR or TCP printing. 
*
*********************************************************************
*/
VOID  PmgZebraSerialCloseWait(long lWaitTime)
{

}

/*
*********************************************************************
*
**Synopsis:     CSTRING    PmgZebraRecv(long lTimeout)
*
**Input:		-  lTimeout is the maximum time (in milliseconds) to  
				   block waiting for a response from the printer
*
**Return:       CSTRING - data from the printer
*
**Description:  This command will receive data from the printer.  
*				If the timeout period is exceeded, an empty string "" 
*				is returned. This command will block a maximum of 
*				lTimeout milliseconds. If data is received before 
*				the the timeout is exceeded, the command will return 
*				as soon as the input buffer has been exhausted for 100ms.
*
*				The maximum number of bytes returned by a single call 
*				to Recv() is 999 bytes. If 999 bytes of data is received 
*				from a single call to Recv(), another call should be made
*				with a timeout of 0 to check for any remaining data.
*
*********************************************************************
*/
UCHAR  PmgZebraRecv(long lTimeout)
{

}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgZebraSetFlowCtl(long lFlowCtlMask)
*
**Input:		- lFlowCtlMask specifies the flow control settings mask
*
**Return:       NONE
*
**Description:  This command enables/disables serial hardware and 
				software flow control.  To choose the flow control 
				settings, build a mask of values where 0x01 specifies 
				hardware flow control and 0x02 specifies software 
				flow control.  For example, to enable both hardware 
				and software flow control, you would use a mask value 
				of 0x03 (i.e. 0x1 + 0x02).  The default mask is 0x0 
				(i.e. no flow control).  
*
*********************************************************************
*/
VOID  PmgZebraSetFlowCtl(long lFlowCtlMask)
{

}
#endif
