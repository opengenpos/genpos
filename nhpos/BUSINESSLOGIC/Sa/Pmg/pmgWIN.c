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
#include	"BlFWif.h"
#include	"display.h"

PMGBLK  PmgWinBlk[MAX_PRINTER];


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
**Synopsis:     VOID    PmgWinWinInitialize(VOID)
*
**Return:       Non
*
**Description:  Print Manager Start up
*
*********************************************************************
*/

VOID PmgWinInitialize( VOID )
{
    DWORD   dwCount, dwRet, i;
    TCHAR   atchDeviceName[SCF_USER_BUFFER];
    TCHAR   atchDllName[SCF_USER_BUFFER];

    for (i = 0; i < MAX_PRINTER; i++)
    {
        PmgWinBlk[i].dwDllLoaded = FALSE;
    }

    dwRet = ScfGetActiveDevice(SCF_TYPENAME_PRINTER, &dwCount, atchDeviceName, atchDllName);

    if (dwRet != SCF_SUCCESS) return;

    for (i = 0; i < dwCount; i++)
    {
        PmgWinBlk[i].dwDllLoaded = TRUE;
    }


    return;
}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgWinFinalize(VOID)
*
**Return:       Non
*
**Description:  Print Manager Finalize
*
*********************************************************************
*/

VOID PmgWinFinalize( VOID )
{
	//device engine should close
#if 0
    DWORD   i;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgWinBlk[i].dwDllLoaded)
        {
            PmgWinBlk[i].Func.Close(PmgWinBlk[i].hHandle);
            FreeDevice();
        }
    }
#endif
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgWinPrint(usPrtType, pucBuff, usLen)
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
USHORT  PmgWinPrint(USHORT usPrtType, TCHAR *pucBuff, USHORT usLen)
{
	int     i, j = 0, k = 0;
	BOOL    bS, bD, bSD;
	USHORT  fbPrtStatus = 0;                /* printer status */
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);
	TCHAR   prtBuff[MAX_PRINT_BUFFER] = {0}, prtSpareBuff[MAX_PRINT_BUFFER] = {0};

	bS = bD = bSD = FALSE;

	if(pucBuff[0]){
		for(j = 0, k = 0; j < usLen; j++){
				switch(pucBuff[j]){
				case PRT_SDOUBLE://Double High and Wide
					if(!bSD){
						bSD = TRUE;
						bD = FALSE;
						bS = FALSE;
						prtBuff[k++] = ESC;
						prtBuff[k++] = _T('|');
						prtBuff[k++] = _T('4');
						prtBuff[k++] = _T('C');
					}
					break;

				case PRT_DOUBLE://Double Wide
					if(!bD){
						bSD = FALSE;
						bD = TRUE;
						bS = FALSE;
						prtBuff[k++] = ESC;
						prtBuff[k++] = _T('|');
						prtBuff[k++] = _T('2');
						prtBuff[k++] = _T('C');
					}
					break;

				case 9://Tab, so Right Justify
					prtBuff[k++] = ESC;
					prtBuff[k++] = _T('|');
					prtBuff[k++] = _T('r');
					prtBuff[k++] = _T('A');
					break;

				case PRT_LOGO:
					prtBuff[k++] = ESC;
					prtBuff[k++] = _T('|');
					prtBuff[k++] = _T('1');
					prtBuff[k++] = _T('B');
					break;

				case PRT_CENTERED:
					prtBuff[k++] = ESC;
					prtBuff[k++] = _T('|');
					prtBuff[k++] = _T('c');
					prtBuff[k++] = _T('A');
					break;

				default:
					if(pucBuff[j]){
						if((bD || bSD) && 
							((j > 0) &&  
							 (pucBuff[j-1] != PRT_SDOUBLE) && 
							 (pucBuff[j-1] != PRT_DOUBLE))){
								bSD = FALSE;
								bD = FALSE;
								bS = TRUE;
								prtBuff[k++] = ESC;
								prtBuff[k++] = _T('|');
								prtBuff[k++] = _T('1');
								prtBuff[k++] = _T('C');
							} 
							prtBuff[k++] = pucBuff[j];
					}
					break;
				}
		}
	}

//	if(prtBuff[0] && pucBuff[0] != ESC){ //SR282
//			prtBuff[k++] = 0x0D;//put return after each line
//			prtBuff[k++] = 0x0A;//put line feed
//	}

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgWinBlk[i].dwDllLoaded)
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
				 ( usPrtTypeMasked == PMG_PRT_JOURNAL) ||
				 ( usPrtTypeMasked == PMG_PRT_VALIDATION) ||
				 ( usPrtTypeMasked == PMG_PRT_SLIP) ) {
					USHORT  usPrtTypeTemp = PMG_MAKE_FULL_PRTTYPE(PMG_WIN_PRINTER, usPrtType);

					PmgWinGetStatus(usPrtTypeTemp, &fbPrtStatus);

					if ( fbPrtStatus & PRT_STAT_POWER_OFF) {
						return(PMG_ERROR_PRINTER);
					}
			}

			BlFwIfPrintWin(prtBuff, (USHORT) wcslen(prtBuff));
	
/*
			switch(usPrtType){
			case PMG_PRT_RECEIPT: /* receipt station          *
			case PMG_PRT_RCT_JNL: /* receipt & journal        
				BlFwIfPrint(PTR_S_RECEIPT, prtBuff, (USHORT) wcslen(prtBuff), 0);
				break;
			case PMG_PRT_JOURNAL: /* journal station          *
				BlFwIfPrint(PTR_S_JOURNAL, prtBuff, (USHORT) wcslen(prtBuff), 0);
				break;
			case PMG_PRT_VALIDATION: /* validation               *
			case PMG_PRT_SLIP: /* slip printer             *
				BlFwIfPrint(PTR_S_SLIP, prtBuff, (USHORT) wcslen(prtBuff), 0);
				break;
			 /* local file printing				*
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
			}*/
        }

    }
	
    return(PMG_SUCCESS);
}


/*
**************************************************************************
*
**Synopsis:     VOID    PmgWinWait( VOID )
*
**Return:       non
*
**Description:  Wait for empty the spool buffer
*
**************************************************************************
*/

VOID PmgWinWait( VOID )
{

	return;//RPH just return for now 4-5-4
/*
    DWORD   i, dwDummy;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgWinBlk[i].dwDllLoaded)
        {
			BlFwPmgWinWait();
        }

    }
*/
}



/*
**************************************************************************
*
**Synopsis:     USHORT  PmgWinBeginSmallValidation(usPrtType)
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

USHORT PmgWinBeginSmallValidation(USHORT usPrtType)
{
    DWORD   i;
    DEVICEIO_PRINTER_ARG_VALIDATION Arg;

    /* Initialize */
    Arg.wPrintStation = usPrtType;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgWinBlk[i].dwDllLoaded)
        {
			BlFwBeginInsertion(2000);
			BlFwEndInsertion();
        }
    }
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgWinEndSmallVlidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Reset Small Validation flag to none
*
**************************************************************************
*/

USHORT PmgWinEndSmallValidation(USHORT usPrtType)
{
    DWORD   i;
    DEVICEIO_PRINTER_ARG_END_VALIDATION Arg;

    /* Initialize */
    Arg.wPrintStation = usPrtType;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgWinBlk[i].dwDllLoaded)
        {
			BlFwBeginRemoval(2000);
			BlFwEndRemoval();
        }

    }
    return(PMG_SUCCESS);
}


/*
**************************************************************************
*
**Synopsis:     USHORT  PmgWinBeginValidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert begin of validation into spool buffer
*
**************************************************************************
*/

USHORT PmgWinBeginValidation(USHORT usPrtType)
{
    DWORD   i;
    DEVICEIO_PRINTER_ARG_VALIDATION Arg;

    /* Initialize */
    Arg.wPrintStation = usPrtType;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgWinBlk[i].dwDllLoaded)
        {
			BlFwBeginInsertion(2000);
			BlFwEndInsertion();
        }
    }
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgWinEndVlidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert end of validation into spool buffer
*
**************************************************************************
*/

USHORT PmgWinEndValidation(USHORT usPrtType)
{
    DWORD   i;
    DEVICEIO_PRINTER_ARG_END_VALIDATION Arg;

    /* Initialize */
    Arg.wPrintStation = usPrtType;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgWinBlk[i].dwDllLoaded)
        {
			BlFwBeginRemoval(2000);
			BlFwEndRemoval();
        }

    }
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgWinFeed(usPrtType, usLine)
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

USHORT PmgWinFeed(USHORT usPrtType, USHORT usLine)
{
    USHORT   i = 0, j;
	TCHAR	tchBuff[8] = {0};

	tchBuff[i++] = ESC;
	tchBuff[i++] = _T('|');
	_ultot(usLine, &tchBuff[i++], 10);
	if(usLine > 9)i++;
	if(usLine > 99)i++;
	tchBuff[i++] = _T('l');
	tchBuff[i++] = _T('F');
	tchBuff[i] = 0x00;

    for (j = 0; j < MAX_PRINTER; j++)
    {
		PmgWinPrint(usPrtType, tchBuff, i);
    }
    return(PMG_SUCCESS);
}

     
/*
**************************************************************************
*
**Synopsis:     USHORT  PmgWinBeginImportant(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert begin of important lines into spool buffer
*
**************************************************************************
*/

USHORT PmgWinBeginImportant(USHORT usPrtType)
{
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgWinEndImportant(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert end of important lines into spool buffer
*
**************************************************************************
*/

USHORT PmgWinEndImportant(USHORT usPrtType)
{
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgWinGetStatus(usPrtType, pfbStatus)
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

USHORT PmgWinGetStatus(USHORT usPrtType, USHORT *pfbStatus)
{
    DWORD   i;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgWinBlk[i].dwDllLoaded)
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

USHORT PmgWinGetStatusOnly(USHORT usPrtType, USHORT *pfbStatus)
{
    DWORD   i;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgWinBlk[i].dwDllLoaded)
        {
			BlFwGetPrinterStatus(usPrtType, pfbStatus);	
		}
	}
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgWinPrtConfig(usPrtType, pusColumn, pfbStatus)
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

USHORT PmgWinPrtConfig(USHORT usPrtType, USHORT *pusColumn, USHORT *pfbStatus)
{
    DWORD   i;
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgWinBlk[i].dwDllLoaded)
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
**Synopsis:     USHORT  PmgWinFont(usPrtType, usFont)
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

USHORT  PmgWinFont( USHORT usPrtType, USHORT usFont)
{
    DWORD   i;
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgWinBlk[i].dwDllLoaded)
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

//            PmgWinBlk[i].Func.IoControl(
//                PmgWinBlk[i].hHandle, 
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
**Synopsis:     VOID    PmgWinSetValWaitCount(USHORT usRetryCount)
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

VOID PmgWinSetValWaitCount(USHORT usRetryCount)
{
    DWORD   i;

    /* Initialize */

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgWinBlk[i].dwDllLoaded)
        {
        }

    }
    return;
}   


VOID PmgWinStartReceipt(VOID)
{
	BlFwIfPrintWinStart();
}

VOID PmgWinEndReceipt(VOID)
{
	BlFwIfPrintWinEnd();
}
/*
**************************************************************************
*
**Synopsis:     USHORT  PmgOPOSFont(usPrtType, usFont)
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

VOID  PmgWinBeginTransaction(SHORT sType, ULONG ulTransNo)
{
    BlFwIfPrintTransStart(sType, ulTransNo);
}
/*
**************************************************************************
*
**Synopsis:     USHORT  PmgOPOSFont(usPrtType, usFont)
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

VOID  PmgWinEndTransaction(SHORT sType, ULONG ulTransNo)
{
    BlFwIfPrintTransEnd(sType, ulTransNo);
}
/***    End of PMGIF.C    ***/
