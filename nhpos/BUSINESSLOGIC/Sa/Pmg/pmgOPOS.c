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
#include	<tchar.h>

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
#include	"OPOSPtr.h"
#include	"BlFWif.h"
#include	"display.h"

PMGBLK  PmgOPOSBlk[MAX_PRINTER];

PMGBLK  PmgComBlk[MAX_PRINTER];

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
**Synopsis:     VOID    PmgOPOSWinInitialize(VOID)
*
**Return:       Non
*
**Description:  Print Manager Start up
*
*********************************************************************
*/

VOID PmgOPOSInitialize( VOID )
{
    DWORD   dwCount, dwRet, i;
    TCHAR   atchDeviceName[SCF_USER_BUFFER];
    TCHAR   atchDllName[SCF_USER_BUFFER];

    for (i = 0; i < MAX_PRINTER; i++)
    {
        PmgOPOSBlk[i].dwDllLoaded = FALSE;
    }

    dwRet = ScfGetActiveDevice(SCF_TYPENAME_PRINTER, &dwCount, atchDeviceName, atchDllName);
    if (dwRet != SCF_SUCCESS) return;

    for (i = 0; i < dwCount; i++)
    {
        PmgOPOSBlk[i].dwDllLoaded = TRUE;
    }

    return;
}

/*
*********************************************************************
*
**Synopsis:     VOID    PmgOPOSFinalize(VOID)
*
**Return:       Non
*
**Description:  Print Manager Finalize
*
*********************************************************************
*/

VOID PmgOPOSFinalize( VOID )
{
	//device engine should close
#if 0
    DWORD   i;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgOPOSBlk[i].dwDllLoaded)
        {
            PmgOPOSBlk[i].Func.Close(PmgOPOSBlk[i].hHandle);
            FreeDevice();
        }
    }
#endif
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgOPOSPrint(usPrtType, pucBuff, usLen)
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
USHORT  PmgOPOSPrint(USHORT usPrtType, TCHAR *pucBuff, USHORT usLen)
{
	TCHAR   prtBuff[MAX_PRINT_BUFFER] = {0};
	TCHAR*	pStop;
	ULONG	ulTransNo;
	int     i, j, k;
	BOOL    bS, bD, bSD;
	USHORT  fbPrtStatus = 0;                /* printer status */
	SHORT	sType;
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

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
				case PRT_TRANS_BGN:
					
					sType = pucBuff[++j];
					ulTransNo = _tcstoul(&pucBuff[++j],&pStop, 10);
					BlFwIfPrintTransStart(sType, ulTransNo);

					return(PMG_SUCCESS);
				case PRT_TRANS_END:
				
					sType = pucBuff[++j];
					ulTransNo = _tcstoul(&pucBuff[++j],&pStop, 10);
					BlFwIfPrintTransEnd(sType, ulTransNo);

					return(PMG_SUCCESS);
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
	
	if(prtBuff[0] && pucBuff[0] != ESC){ //SR282
			prtBuff[k++] = 0x0D;//put return after each line
			prtBuff[k++] = 0x0A;//put line feed
	}

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgOPOSBlk[i].dwDllLoaded)
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
				 /*( usPrtType == PMG_PRT_JOURNAL) || EJ Printing, is this necessary?*/
				 ( usPrtTypeMasked == PMG_PRT_VALIDATION) ||
				 ( usPrtTypeMasked == PMG_PRT_SLIP) ) {

					PmgOPOSGetStatus(usPrtType, &fbPrtStatus);
					if ( fbPrtStatus & (PRT_STAT_POWER_OFF | PRT_STAT_HW_ERROR)) {
						if ((usPrtType & PMG_PRT_REPORT_ERROR) == 0) {
							USHORT usLeadThru = PmgCallBack(DEVICEIO_PRINTER_MSG_POWEROFF_ADR, 0);
						}
						return(PMG_ERROR_PRINTER);
					}
			}

			switch(usPrtTypeMasked){
			case PMG_PRT_RECEIPT: /* receipt station          */
			case PMG_PRT_RCT_JNL: /* receipt & journal        */
				usPrtType &= 0xf0;
				if (usPrtType & PMG_PRT_IMMEDIATE) {
					// use the printImmediate() OPOS method rather than printNormal() OPOS method.
					// needed for some kinds of label type printers such as Zebra.
					// we assume that printImmediate() method will flush any pending print lines the printer has buffered.
					usPrtType &= ~PMG_PRT_IMMEDIATE;
					if (BlFwIfPrint(PMG_MAKE_FULL_PRTTYPE(PMG_OPOS_IMMEDIATE, (PTR_S_RECEIPT | usPrtType)), prtBuff, (USHORT) wcslen(prtBuff), 0) == 0) {
						return(PMG_ERROR_PRINTER);
					}
				} else {
					if (BlFwIfPrint(PMG_MAKE_FULL_PRTTYPE(PMG_OPOS_PRINTER, (PTR_S_RECEIPT | usPrtType)), prtBuff, (USHORT) wcslen(prtBuff), 0) == 0) {
						return(PMG_ERROR_PRINTER);
					}
				}
				break;
			//We take out the PMG_PRT_JOURNAL because we now do an electronic 
			//journal, and we have no printers that print to a printers journal
			//this case was used for when printers would have the paper that 
			//goes to the customer, and the internal roll that managers used
			//for internal purposes. JHHJ
#if EJPRINT
			case PMG_PRT_JOURNAL: /* journal station          */
				if (BlFwIfPrint(PTR_S_JOURNAL, prtBuff, (USHORT) wcslen(prtBuff), 0) == 0) {
					return(PMG_ERROR_PRINTER);
				}
				break;
#endif
			case PMG_PRT_VALIDATION: /* validation               */
			case PMG_PRT_SLIP: /* slip printer             */
				usPrtType &= 0xf0;
				if (BlFwIfPrint(PMG_MAKE_FULL_PRTTYPE(PMG_OPOS_PRINTER, (PTR_S_SLIP | usPrtType)), prtBuff, (USHORT) wcslen(prtBuff), 0) == 0) {
					return(PMG_ERROR_PRINTER);
				}
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
**Synopsis:     VOID    PmgOPOSWait( VOID )
*
**Return:       non
*
**Description:  Wait for empty the spool buffer
*
**************************************************************************
*/

VOID PmgOPOSWait( VOID )
{

	return;//RPH just return for now 4-5-4
/*
    DWORD   i, dwDummy;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgOPOSBlk[i].dwDllLoaded)
        {
			BlFwPmgOPOSWait();
        }

    }
*/
}



/*
**************************************************************************
*
**Synopsis:     USHORT  PmgOPOSBeginSmallValidation(usPrtType)
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

USHORT PmgOPOSBeginSmallValidation(USHORT usPrtType)
{
    DWORD   i;
    DEVICEIO_PRINTER_ARG_VALIDATION Arg;

    /* Initialize */
    Arg.wPrintStation = usPrtType;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgOPOSBlk[i].dwDllLoaded)
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
**Synopsis:     USHORT  PmgOPOSEndSmallVlidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Reset Small Validation flag to none
*
**************************************************************************
*/

USHORT PmgOPOSEndSmallValidation(USHORT usPrtType)
{
    DWORD   i;
    DEVICEIO_PRINTER_ARG_END_VALIDATION Arg;

    /* Initialize */
    Arg.wPrintStation = usPrtType;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgOPOSBlk[i].dwDllLoaded)
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
**Synopsis:     USHORT  PmgOPOSBeginValidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert begin of validation into spool buffer
*
**************************************************************************
*/

USHORT PmgOPOSBeginValidation(USHORT usPrtType)
{
    DWORD   i;
    DEVICEIO_PRINTER_ARG_VALIDATION Arg;

    /* Initialize */
    Arg.wPrintStation = usPrtType;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgOPOSBlk[i].dwDllLoaded)
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
**Synopsis:     USHORT  PmgOPOSEndVlidation(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert end of validation into spool buffer
*
**************************************************************************
*/

USHORT PmgOPOSEndValidation(USHORT usPrtType)
{
    DWORD   i;
    DEVICEIO_PRINTER_ARG_END_VALIDATION Arg;

    /* Initialize */
    Arg.wPrintStation = usPrtType;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgOPOSBlk[i].dwDllLoaded)
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
**Synopsis:     USHORT  PmgOPOSFeed(usPrtType, usLine)
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

USHORT PmgOPOSFeed(USHORT usPrtType, USHORT usLine)
{
    USHORT   i = 0;
	TCHAR	tchBuff[8];

	tchBuff[i++] = ESC;
	tchBuff[i++] = _T('|');
	_ultot(usLine, &tchBuff[i++], 10);
	if(usLine > 9)i++;
	if(usLine > 99)i++;
	tchBuff[i++] = _T('l');
	tchBuff[i++] = _T('F');
	tchBuff[i] = 0x00;

	PmgOPOSPrint(usPrtType, tchBuff, i);
    return(PMG_SUCCESS);
}

     
/*
**************************************************************************
*
**Synopsis:     USHORT  PmgOPOSBeginImportant(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert begin of important lines into spool buffer
*
**************************************************************************
*/

USHORT PmgOPOSBeginImportant(USHORT usPrtType)
{
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgOPOSEndImportant(usPrtType)
*               input   USHORT  usPrtType;  : type of printer
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*
**Description:  Insert end of important lines into spool buffer
*
**************************************************************************
*/

USHORT PmgOPOSEndImportant(USHORT usPrtType)
{
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgOPOSGetStatus(usPrtType, pfbStatus)
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

USHORT PmgOPOSGetStatus(USHORT usPrtType, USHORT *pfbStatus)
{
    DWORD   i;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgOPOSBlk[i].dwDllLoaded)
        {
			BlFwGetPrinterStatus(PMG_MAKE_FULL_PRTTYPE(PMG_OPOS_PRINTER, usPrtType), pfbStatus);	
			if(*pfbStatus & (PRT_STAT_POWER_OFF | PRT_STAT_HW_ERROR))
			{
				if ((usPrtType & PMG_PRT_REPORT_ERROR) == 0) {
					USHORT usLeadThru = PmgCallBack(DEVICEIO_PRINTER_MSG_POWEROFF_ADR, 0);
				}
			}
		
		}
	}
    return(PMG_SUCCESS);
}

USHORT PmgOPOSGetStatusOnly(USHORT usPrtType, USHORT *pfbStatus)
{
    DWORD   i;

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgOPOSBlk[i].dwDllLoaded)
        {
			BlFwGetPrinterStatus(PMG_MAKE_FULL_PRTTYPE(PMG_OPOS_PRINTER, usPrtType), pfbStatus);
			break;
		}
	}
    return(PMG_SUCCESS);
}

/*
**************************************************************************
*
**Synopsis:     USHORT  PmgOPOSPrtConfig(usPrtType, pusColumn, pfbStatus)
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

USHORT PmgOPOSPrtConfig(USHORT usPrtType, USHORT *pusColumn, USHORT *pfbStatus)
{
    DWORD   i;
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgOPOSBlk[i].dwDllLoaded)
        {
			switch(usPrtTypeMasked){
			case PMG_PRT_RECEIPT: /* receipt station          */
			case PMG_PRT_RCT_JNL: /* receipt & journal        */
				BlFwGetPrinterColumnsStatus(PTR_S_RECEIPT, pusColumn, pfbStatus);
				break;
			case PMG_PRT_JOURNAL: /* journal station          */
				BlFwGetPrinterColumnsStatus(PTR_S_JOURNAL, pusColumn, pfbStatus);
				break;
			case PMG_PRT_VALIDATION: /* validation               */
			case PMG_PRT_SLIP: /* slip printer             */
				BlFwGetPrinterColumnsStatus(PTR_S_SLIP, pusColumn, pfbStatus);
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
USHORT  PmgOPOSFont( USHORT usPrtType, USHORT usFont)
{
    DWORD   i;
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgOPOSBlk[i].dwDllLoaded)
        {
			switch(usPrtTypeMasked){
			case PMG_PRT_RECEIPT: /* receipt station          */
			case PMG_PRT_RCT_JNL: /* receipt & journal        */
				usPrtType &= 0xf0;
				BlFwIfPrintSetFont(PMG_MAKE_FULL_PRTTYPE(PMG_OPOS_PRINTER, (PTR_S_RECEIPT | usPrtType)), usFont);
				break;
			case PMG_PRT_JOURNAL: /* journal station          */
				BlFwIfPrintSetFont(PTR_S_JOURNAL, usFont);
				break;
			case PMG_PRT_VALIDATION: /* validation               */
			case PMG_PRT_SLIP: /* slip printer             */
				usPrtType &= 0xf0;
				BlFwIfPrintSetFont(PMG_MAKE_FULL_PRTTYPE(PMG_OPOS_PRINTER, (PTR_S_SLIP | usPrtType)), usFont);
				break;
			default:
				break;
			}

//            PmgOPOSBlk[i].Func.IoControl(
//                PmgOPOSBlk[i].hHandle, 
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
LONG PmgOPOSGetSetPrtCap (USHORT usPrtType, USHORT usPrtCap, LONG lValue)
{
	LONG    lRetValue = 0;
    DWORD   i;
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgOPOSBlk[i].dwDllLoaded)
        {
			switch(usPrtTypeMasked){
			case PMG_PRT_RECEIPT: /* receipt station          */
			case PMG_PRT_RCT_JNL: /* receipt & journal        */
				usPrtType &= 0xf0;
				lRetValue = BlFwIfPrintGetSetPrtCap(PMG_MAKE_FULL_PRTTYPE(PMG_OPOS_PRINTER, (PTR_S_RECEIPT | usPrtType)), usPrtCap, lValue);
				break;
			case PMG_PRT_JOURNAL: /* journal station          */
				lRetValue = BlFwIfPrintGetSetPrtCap(PTR_S_JOURNAL, usPrtCap, lValue);
				break;
			case PMG_PRT_VALIDATION: /* validation               */
			case PMG_PRT_SLIP: /* slip printer             */
				usPrtType &= 0xf0;
				lRetValue = BlFwIfPrintGetSetPrtCap(PMG_MAKE_FULL_PRTTYPE(PMG_OPOS_PRINTER, (PTR_S_SLIP | usPrtType)), usPrtCap, lValue);
				break;
			default:
				break;
			}

//            PmgOPOSBlk[i].Func.IoControl(
//                PmgOPOSBlk[i].hHandle, 
//                DEVICEIO_CTL_PRINTER_FONT, 
//                NULL, 
//                0, 
//                &Arg, 
//                sizeof(Arg), 
//                &dwDummy);
        }

    }
    return(lRetValue);
}


/*
**************************************************************************
*
**Synopsis:     VOID PmgOPOSPrintBarCode (USHORT usPrtType, TCHAR *pucBuff, ULONG ulTextFlags, ULONG ulCodeFlags)
*               input   USHORT  usPrtType;     : type of printer
*               inout   ULONG   ulTextFlags;   : operations on text such as alignment
*               inout   ULONG   ulCodeFlags;   : indicates which symbology to use
*
**Return:       PMG_ERROR_PROVIDE   : printer not provide
*               PMG_ERROR_COMM      : communication error
*
**Description:  Set to font type
*
**************************************************************************
*/
USHORT PmgOPOSPrintBarCode (USHORT usPrtType, TCHAR *pucBuff, ULONG ulTextFlags, ULONG ulCodeFlags)
{
    DWORD   i;
	USHORT  fbPrtStatus = 0;                /* printer status */
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgOPOSBlk[i].dwDllLoaded)
        {
			if ( ( usPrtTypeMasked == PMG_PRT_RECEIPT) ||
				 ( usPrtTypeMasked == PMG_PRT_RCT_JNL) ||
				 /*( usPrtType == PMG_PRT_JOURNAL) || EJ Printing, is this necessary?*/
				 ( usPrtTypeMasked == PMG_PRT_VALIDATION) ||
				 ( usPrtTypeMasked == PMG_PRT_SLIP) ) {

					PmgOPOSGetStatus(usPrtType, &fbPrtStatus);
					if ( fbPrtStatus & (PRT_STAT_POWER_OFF | PRT_STAT_HW_ERROR)) {
						if ((usPrtType & PMG_PRT_REPORT_ERROR) == 0) {
							USHORT usLeadThru = PmgCallBack(DEVICEIO_PRINTER_MSG_POWEROFF_ADR, 0);
						}
						return(PMG_ERROR_PRINTER);
					}
			}
			switch(usPrtTypeMasked){
			case PMG_PRT_RECEIPT: /* receipt station          */
			case PMG_PRT_RCT_JNL: /* receipt & journal        */
				usPrtType &= 0xf0;
				BlFwIfPrintPrintBarCode(PMG_MAKE_FULL_PRTTYPE(PMG_OPOS_PRINTER, (PTR_S_RECEIPT | usPrtType)), pucBuff, ulTextFlags, ulCodeFlags, 0);
				break;
			case PMG_PRT_JOURNAL: /* journal station          */
				BlFwIfPrintPrintBarCode(PTR_S_JOURNAL, pucBuff, ulTextFlags, ulCodeFlags, 0);
				break;
			case PMG_PRT_VALIDATION: /* validation               */
			case PMG_PRT_SLIP: /* slip printer             */
				usPrtType &= 0xf0;
				BlFwIfPrintPrintBarCode(PMG_MAKE_FULL_PRTTYPE(PMG_OPOS_PRINTER, (PTR_S_SLIP | usPrtType)), pucBuff, ulTextFlags, ulCodeFlags, 0);
				break;
			default:
				break;
			}

//            PmgOPOSBlk[i].Func.IoControl(
//                PmgOPOSBlk[i].hHandle, 
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
**Synopsis:     VOID    PmgOPOSSetValWaitCount(USHORT usRetryCount)
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

VOID PmgOPOSSetValWaitCount(USHORT usRetryCount)
{
    DWORD   i;

    /* Initialize */

    for (i = 0; i < MAX_PRINTER; i++)
    {
        if (PmgOPOSBlk[i].dwDllLoaded)
        {
        }

    }
    return;
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

VOID  PmgOPOSBeginTransaction(SHORT sType, ULONG ulTransNo)
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

VOID  PmgOPOSEndTransaction(SHORT sType, ULONG ulTransNo)
{

	BlFwIfPrintTransEnd(sType,ulTransNo);
    
}

//=============================================================================
//=============================================================================

// PMG_COM_PRINTER functions begin here.


VOID PmgCOMInitialize(VOID)
{
	DWORD   dwCount, dwRet, i;
	TCHAR   atchDeviceName[SCF_USER_BUFFER];
	TCHAR   atchDllName[SCF_USER_BUFFER];

	for (i = 0; i < MAX_PRINTER; i++)
	{
		PmgComBlk[i].dwDllLoaded = FALSE;
	}

	dwRet = ScfGetActiveDevice(SCF_TYPENAME_PRINTER, &dwCount, atchDeviceName, atchDllName);
	if (dwRet != SCF_SUCCESS) return;

	for (i = 0; i < dwCount; i++)
	{
		PmgComBlk[i].dwDllLoaded = TRUE;
	}

	return;
}

VOID PmgCOMFinalize(VOID)
{
	//device engine should close
#if 0
	DWORD   i;

	for (i = 0; i < MAX_PRINTER; i++)
	{
		if (PmgComBlk[i].dwDllLoaded)
		{
			PmgComBlk[i].Func.Close(PmgComBlk[i].hHandle);
			FreeDevice();
		}
	}
#endif
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//

#define ALIGN_RIGHT   0x09      // Tab, so Right Justify
#define ALIGN_CENTER  0x16      // PRT_CENTERED, so center text
#define ALIGN_LEFT    0x17

TCHAR EscSeqOne[8] = { L"\x1b|lA" };    // left align following text in print line
TCHAR EscSeqTwo[8] = { L"\x1b|rA" };    // right align following text in print line
TCHAR EscSeqThree[8] = { L"\x1b|cA" };  // center following text in print line

#if 0
// OPOS receipt printer escape codes for standard
// printer output formatting actions.

typedef struct {
	const CHAR* pEsc;
	const USHORT usLen;
} PrinterEscCode;

static const PrinterEscCode EscAlignLeft = { "\x1b|lA", 4 };      // left align following text in print line
static const PrinterEscCode EscAlignRight = { "\x1b|rA", 4 };     // right align following text in print line
static const PrinterEscCode EscAlignCenter = { "\x1b|cA", 4 };    // center following text in print line
#endif

TCHAR* EscSeqList[] = { EscSeqOne, EscSeqTwo, EscSeqThree, NULL };

static int CheckEscSeq(const TCHAR*inBuff, const TCHAR*escSeq) {
for (; *escSeq; ) {
	if (*(inBuff++) != *escSeq++) return 0;
}
return 1;
}

static int EscCpy(TCHAR* outBuff, int* out, const  TCHAR* inBuff, int* in, const TCHAR* escSeq) {
	int outSave = *out;

	for (; *escSeq; escSeq++) {
		outBuff[((*out)++)] = inBuff[((*in)++)];
	}
	return *out - outSave;;
}

static int CountChars(const TCHAR* inBuff) {
	int iCount = 0;
	int bRun = 1;

	for (TCHAR ucAlignType = *inBuff++; bRun; ) {
		switch (*inBuff) {
		case 0:
			bRun = 0;
			break;
		case ALIGN_RIGHT:
			if (*inBuff != ucAlignType) {
				bRun = 0;
			}
			ucAlignType = *inBuff++;
			break;
		case ALIGN_CENTER:
			if (*inBuff != ucAlignType) {
				bRun = 0;
			}
			ucAlignType = *inBuff++;
			break;
		case ALIGN_LEFT:
			if (*inBuff != ucAlignType) {
				bRun = 0;
			}
			ucAlignType = *inBuff++;
			break;
		case 0x1b:   // escape character
			// break;
		default:
			iCount++;
			inBuff++;
			break;
		}
	}
	return iCount;
}

static int createPrintBuffer(TCHAR* outBuff, const TCHAR* inBuff, int lineLength)
{
	int in = 0, out = 0;
	int nChars = 0;
	int nCharsEsc = 0;
	TCHAR ucAlignType = 0;

	for (; inBuff[in]; ) {
		switch (inBuff[in]) {
		case 0x1b:   // escape character
#if 1
			// count the number of escaped characters we have processed as these are
			// invisible characters that will not be printed. 
			if (CheckEscSeq(inBuff + in, EscSeqOne)) {
				nCharsEsc += EscCpy(outBuff, &out, inBuff, &in, EscSeqOne);
			}
			else {
				nCharsEsc += 2;      // assume at least 2 chars in unknown escape sequence
				outBuff[out++] = inBuff[in++];
				nChars++;
			}
#else
			// just copy the escape sequence taking a simple approach
			outBuff[out++] = inBuff[in++];
			nChars++;
#endif
			break;
		case ALIGN_RIGHT:
			if (ucAlignType != inBuff[in]) {
				int i = lineLength - CountChars(inBuff + in);
				for (; out < i; out++) {
					outBuff[out] = ' ';
				}
			}
			ucAlignType = inBuff[in];
			in++;
			break;
		case ALIGN_CENTER:
			if (ucAlignType != inBuff[in]) {
				int i = lineLength / 2 + nCharsEsc - CountChars(inBuff + in);
				for (; out < i; out++) {
					outBuff[out] = ' ';
				}
			}
			ucAlignType = inBuff[in];
			in++;
			break;
		case ALIGN_LEFT:
			if (ucAlignType != inBuff[in]) out = 0;
			ucAlignType = inBuff[in];
			in++;
			break;
		default:
			outBuff[out++] = inBuff[in++];
			nChars++;
			break;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

USHORT  PmgCOMPrint(USHORT usPrtType, TCHAR *pucBuff, USHORT usLen)
{
	TCHAR   prtBuff[MAX_PRINT_BUFFER] = { 0 };
	TCHAR   tmpBuff[128] = { 0 };
	TCHAR*	pStop;
	ULONG	ulTransNo;
	int     i, j, k;
	BOOL    bS, bD, bSD;
	USHORT  fbPrtStatus = 0;                /* printer status */
	SHORT	sType;
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

	createPrintBuffer(tmpBuff, pucBuff, 44);

	bS = bD = bSD = FALSE;

	if (pucBuff[0]) {
		TCHAR   * ptmpBuff = tmpBuff;
		usLen = wcslen(tmpBuff);
		for (j = 0, k = 0; j < usLen; j++) {
			switch (ptmpBuff[j]) {
			case PRT_SDOUBLE://Double High and Wide
				if (!bSD) {
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
				if (!bD) {
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
			case PRT_TRANS_BGN:

				sType = ptmpBuff[++j];
				ulTransNo = _tcstoul(&ptmpBuff[++j], &pStop, 10);
				BlFwIfPrintTransStart(sType, ulTransNo);

				return(PMG_SUCCESS);
			case PRT_TRANS_END:

				sType = ptmpBuff[++j];
				ulTransNo = _tcstoul(&ptmpBuff[++j], &pStop, 10);
				BlFwIfPrintTransEnd(sType, ulTransNo);

				return(PMG_SUCCESS);
				break;
			default:
				if (ptmpBuff[j]) {
					if ((bD || bSD) &&
						((j > 0) &&
						(ptmpBuff[j - 1] != PRT_SDOUBLE) &&
							(ptmpBuff[j - 1] != PRT_DOUBLE))) {
						bSD = FALSE;
						bD = FALSE;
						bS = TRUE;
						prtBuff[k++] = ESC;
						prtBuff[k++] = _T('|');
						prtBuff[k++] = _T('1');
						prtBuff[k++] = _T('C');
					}
					prtBuff[k++] = ptmpBuff[j];
				}
				break;
			}
		}
	}

	if (prtBuff[0] && pucBuff[0] != ESC) { //SR282
		prtBuff[k++] = 0x0D;//put return after each line
		prtBuff[k++] = 0x0A;//put line feed
	}

	for (i = 0; i < MAX_PRINTER; i++)
	{
		if (PmgComBlk[i].dwDllLoaded)
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

			if ((usPrtTypeMasked == PMG_PRT_RECEIPT) ||
				(usPrtTypeMasked == PMG_PRT_RCT_JNL) ||
				/*( usPrtType == PMG_PRT_JOURNAL) || EJ Printing, is this necessary?*/
				(usPrtTypeMasked == PMG_PRT_VALIDATION) ||
				(usPrtTypeMasked == PMG_PRT_SLIP)) {

				PmgOPOSGetStatus(usPrtType, &fbPrtStatus);
				if (fbPrtStatus & (PRT_STAT_POWER_OFF | PRT_STAT_HW_ERROR)) {
					if ((usPrtType & PMG_PRT_REPORT_ERROR) == 0) {
						USHORT usLeadThru = PmgCallBack(DEVICEIO_PRINTER_MSG_POWEROFF_ADR, 0);
					}
					return(PMG_ERROR_PRINTER);
				}
			}

			switch (usPrtTypeMasked) {
			case PMG_PRT_RECEIPT: /* receipt station          */
			case PMG_PRT_RCT_JNL: /* receipt & journal        */
				usPrtType &= 0xf0;
				if (BlFwIfPrint(PMG_MAKE_FULL_PRTTYPE(PMG_COM_PRINTER, (PTR_S_RECEIPT | usPrtType)), prtBuff, (USHORT)wcslen(prtBuff), 0) == 0) {
					return(PMG_ERROR_PRINTER);
				}
				break;
				//We take out the PMG_PRT_JOURNAL because we now do an electronic 
				//journal, and we have no printers that print to a printers journal
				//this case was used for when printers would have the paper that 
				//goes to the customer, and the internal roll that managers used
				//for internal purposes. JHHJ
#if EJPRINT
			case PMG_PRT_JOURNAL: /* journal station          */
				if (BlFwIfPrint(PTR_S_JOURNAL, prtBuff, (USHORT)wcslen(prtBuff), 0) == 0) {
					return(PMG_ERROR_PRINTER);
				}
				break;
#endif
			case PMG_PRT_VALIDATION: /* validation               */
			case PMG_PRT_SLIP: /* slip printer             */
				usPrtType &= 0xf0;
				if (BlFwIfPrint(PMG_MAKE_FULL_PRTTYPE(PMG_COM_PRINTER, (PTR_S_SLIP | usPrtType)), prtBuff, (USHORT)wcslen(prtBuff), 0) == 0) {
					return(PMG_ERROR_PRINTER);
				}
				break;
				/* local file printing				*/
			case PMG_PRT_FILE_PRG:
				BlFwIfPrintFileWrite(prtBuff, (USHORT)wcslen(prtBuff), BL_PRTFILE_PROG);
				break;
			case PMG_PRT_FILE_SUP:
				BlFwIfPrintFileWrite(prtBuff, (USHORT)wcslen(prtBuff), BL_PRTFILE_SUPER);
				break;
			case PMG_PRT_FILE_REG:
				BlFwIfPrintFileWrite(prtBuff, (USHORT)wcslen(prtBuff), BL_PRTFILE_REG);
				break;
			default:
				break;
			}
		}
	}
	return(PMG_SUCCESS);
}


VOID PmgCOMWait(VOID)
{
	return;//RPH just return for now 4-5-4
}


USHORT PmgCOMBeginSmallValidation(USHORT usPrtType)
{
	DWORD   i;
	DEVICEIO_PRINTER_ARG_VALIDATION Arg;

	/* Initialize */
	Arg.wPrintStation = usPrtType;

	for (i = 0; i < MAX_PRINTER; i++)
	{
		if (PmgComBlk[i].dwDllLoaded)
		{
			BlFwBeginInsertion(2000);
			BlFwEndInsertion();
		}
	}
	return(PMG_SUCCESS);
}

USHORT PmgCOMEndSmallValidation(USHORT usPrtType)
{
	DWORD   i;
	DEVICEIO_PRINTER_ARG_END_VALIDATION Arg;

	/* Initialize */
	Arg.wPrintStation = usPrtType;

	for (i = 0; i < MAX_PRINTER; i++)
	{
		if (PmgComBlk[i].dwDllLoaded)
		{
			BlFwBeginRemoval(2000);
			BlFwEndRemoval();
		}
	}
	return(PMG_SUCCESS);
}


USHORT PmgCOMBeginValidation(USHORT usPrtType)
{
	DWORD   i;
	DEVICEIO_PRINTER_ARG_VALIDATION Arg;

	/* Initialize */
	Arg.wPrintStation = usPrtType;

	for (i = 0; i < MAX_PRINTER; i++)
	{
		if (PmgComBlk[i].dwDllLoaded)
		{
			BlFwBeginInsertion(2000);
			BlFwEndInsertion();
		}
	}
	return(PMG_SUCCESS);
}

USHORT PmgCOMEndValidation(USHORT usPrtType)
{
	DWORD   i;
	DEVICEIO_PRINTER_ARG_END_VALIDATION Arg;

	/* Initialize */
	Arg.wPrintStation = usPrtType;

	for (i = 0; i < MAX_PRINTER; i++)
	{
		if (PmgComBlk[i].dwDllLoaded)
		{
			BlFwBeginRemoval(2000);
			BlFwEndRemoval();
		}

	}
	return(PMG_SUCCESS);
}

USHORT PmgCOMFeed(USHORT usPrtType, USHORT usLine)
{
	USHORT   i = 0;
	TCHAR	tchBuff[8];

	tchBuff[i++] = ESC;
	tchBuff[i++] = _T('|');
	_ultot(usLine, &tchBuff[i++], 10);
	if (usLine > 9)i++;
	if (usLine > 99)i++;
	tchBuff[i++] = _T('l');
	tchBuff[i++] = _T('F');
	tchBuff[i] = 0x00;

	PmgCOMPrint(usPrtType, tchBuff, i);
	return(PMG_SUCCESS);
}


USHORT PmgCOMBeginImportant(USHORT usPrtType)
{
	return(PMG_SUCCESS);
}

USHORT PmgCOMEndImportant(USHORT usPrtType)
{
	return(PMG_SUCCESS);
}

USHORT PmgCOMGetStatus(USHORT usPrtType, USHORT *pfbStatus)
{
	DWORD   i;

	for (i = 0; i < MAX_PRINTER; i++)
	{
		if (PmgComBlk[i].dwDllLoaded)
		{
			BlFwGetPrinterStatus(PMG_MAKE_FULL_PRTTYPE(PMG_COM_PRINTER, usPrtType), pfbStatus);
			if (*pfbStatus & (PRT_STAT_POWER_OFF | PRT_STAT_HW_ERROR))
			{
				if ((usPrtType & PMG_PRT_REPORT_ERROR) == 0) {
					USHORT usLeadThru = PmgCallBack(DEVICEIO_PRINTER_MSG_POWEROFF_ADR, 0);
				}
			}

		}
	}
	return(PMG_SUCCESS);
}

USHORT PmgCOMGetStatusOnly(USHORT usPrtType, USHORT *pfbStatus)
{
	DWORD   i;

	for (i = 0; i < MAX_PRINTER; i++)
	{
		if (PmgComBlk[i].dwDllLoaded)
		{
			BlFwGetPrinterStatus(PMG_MAKE_FULL_PRTTYPE(PMG_COM_PRINTER, usPrtType), pfbStatus);
			break;
		}
	}
	return(PMG_SUCCESS);
}

USHORT PmgCOMPrtConfig(USHORT usPrtType, USHORT *pusColumn, USHORT *pfbStatus)
{
	DWORD   i;
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

	for (i = 0; i < MAX_PRINTER; i++)
	{
		if (PmgComBlk[i].dwDllLoaded)
		{
			switch (usPrtTypeMasked) {
			case PMG_PRT_RECEIPT: /* receipt station          */
			case PMG_PRT_RCT_JNL: /* receipt & journal        */
				BlFwGetPrinterColumnsStatus(PMG_MAKE_FULL_PRTTYPE(PMG_COM_PRINTER, PTR_S_RECEIPT), pusColumn, pfbStatus);
				break;
			case PMG_PRT_JOURNAL: /* journal station          */
				BlFwGetPrinterColumnsStatus(PMG_MAKE_FULL_PRTTYPE(PMG_COM_PRINTER, PTR_S_JOURNAL), pusColumn, pfbStatus);
				break;
			case PMG_PRT_VALIDATION: /* validation               */
			case PMG_PRT_SLIP: /* slip printer             */
				BlFwGetPrinterColumnsStatus(PMG_MAKE_FULL_PRTTYPE(PMG_COM_PRINTER, PTR_S_SLIP), pusColumn, pfbStatus);
				break;
			default:
				break;
			}
		}

	}
	return(PMG_SUCCESS);
}

USHORT  PmgCOMFont(USHORT usPrtType, USHORT usFont)
{
	DWORD   i;
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

	for (i = 0; i < MAX_PRINTER; i++)
	{
		if (PmgComBlk[i].dwDllLoaded)
		{
			switch (usPrtTypeMasked) {
			case PMG_PRT_RECEIPT: /* receipt station          */
			case PMG_PRT_RCT_JNL: /* receipt & journal        */
				usPrtType &= 0xf0;
				BlFwIfPrintSetFont(PMG_MAKE_FULL_PRTTYPE(PMG_COM_PRINTER, (PTR_S_RECEIPT | usPrtType)), usFont);
				break;
			case PMG_PRT_JOURNAL: /* journal station          */
				BlFwIfPrintSetFont(PTR_S_JOURNAL, usFont);
				break;
			case PMG_PRT_VALIDATION: /* validation               */
			case PMG_PRT_SLIP: /* slip printer             */
				usPrtType &= 0xf0;
				BlFwIfPrintSetFont(PMG_MAKE_FULL_PRTTYPE(PMG_COM_PRINTER, (PTR_S_SLIP | usPrtType)), usFont);
				break;
			default:
				break;
			}
		}
	}
	return(PMG_SUCCESS);
}

LONG PmgCOMGetSetPrtCap(USHORT usPrtType, USHORT usPrtCap, LONG lValue)
{
	LONG    lRetValue = 0;
	DWORD   i;
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

	for (i = 0; i < MAX_PRINTER; i++)
	{
		if (PmgComBlk[i].dwDllLoaded)
		{
			switch (usPrtTypeMasked) {
			case PMG_PRT_RECEIPT: /* receipt station          */
			case PMG_PRT_RCT_JNL: /* receipt & journal        */
				usPrtType &= 0xf0;
				lRetValue = BlFwIfPrintGetSetPrtCap(PMG_MAKE_FULL_PRTTYPE(PMG_COM_PRINTER, (PTR_S_RECEIPT | usPrtType)), usPrtCap, lValue);
				break;
			case PMG_PRT_JOURNAL: /* journal station          */
				lRetValue = BlFwIfPrintGetSetPrtCap(PTR_S_JOURNAL, usPrtCap, lValue);
				break;
			case PMG_PRT_VALIDATION: /* validation               */
			case PMG_PRT_SLIP: /* slip printer             */
				usPrtType &= 0xf0;
				lRetValue = BlFwIfPrintGetSetPrtCap(PMG_MAKE_FULL_PRTTYPE(PMG_COM_PRINTER, (PTR_S_SLIP | usPrtType)), usPrtCap, lValue);
				break;
			default:
				break;
			}
		}
	}
	return(lRetValue);
}


USHORT PmgCOMPrintBarCode(USHORT usPrtType, TCHAR *pucBuff, ULONG ulTextFlags, ULONG ulCodeFlags)
{
	DWORD   i;
	USHORT  fbPrtStatus = 0;                /* printer status */
	USHORT  usPrtTypeMasked = PMG_GET_PRT_STATION(usPrtType);

	for (i = 0; i < MAX_PRINTER; i++)
	{
		if (PmgComBlk[i].dwDllLoaded)
		{
			if ((usPrtTypeMasked == PMG_PRT_RECEIPT) ||
				(usPrtTypeMasked == PMG_PRT_RCT_JNL) ||
				/*( usPrtType == PMG_PRT_JOURNAL) || EJ Printing, is this necessary?*/
				(usPrtTypeMasked == PMG_PRT_VALIDATION) ||
				(usPrtTypeMasked == PMG_PRT_SLIP)) {

				PmgOPOSGetStatus(usPrtType, &fbPrtStatus);
				if (fbPrtStatus & (PRT_STAT_POWER_OFF | PRT_STAT_HW_ERROR)) {
					if ((usPrtType & PMG_PRT_REPORT_ERROR) == 0) {
						USHORT usLeadThru = PmgCallBack(DEVICEIO_PRINTER_MSG_POWEROFF_ADR, 0);
					}
					return(PMG_ERROR_PRINTER);
				}
			}
			switch (usPrtTypeMasked) {
			case PMG_PRT_RECEIPT: /* receipt station          */
			case PMG_PRT_RCT_JNL: /* receipt & journal        */
				usPrtType &= 0xf0;
				BlFwIfPrintPrintBarCode(PMG_MAKE_FULL_PRTTYPE(PMG_COM_PRINTER, (PTR_S_RECEIPT | usPrtType)), pucBuff, ulTextFlags, ulCodeFlags, 0);
				break;
			case PMG_PRT_JOURNAL: /* journal station          */
				BlFwIfPrintPrintBarCode(PTR_S_JOURNAL, pucBuff, ulTextFlags, ulCodeFlags, 0);
				break;
			case PMG_PRT_VALIDATION: /* validation               */
			case PMG_PRT_SLIP: /* slip printer             */
				usPrtType &= 0xf0;
				BlFwIfPrintPrintBarCode(PMG_MAKE_FULL_PRTTYPE(PMG_COM_PRINTER, (PTR_S_SLIP | usPrtType)), pucBuff, ulTextFlags, ulCodeFlags, 0);
				break;
			default:
				break;
			}
		}
	}

	return(PMG_SUCCESS);
}

VOID PmgCOMSetValWaitCount(USHORT usRetryCount)
{
}

VOID  PmgCOMBeginTransaction(SHORT sType, ULONG ulTransNo)
{
}

VOID  PmgCOMEndTransaction(SHORT sType, ULONG ulTransNo)
{
}

/***    End of PMGIF.C    ***/
