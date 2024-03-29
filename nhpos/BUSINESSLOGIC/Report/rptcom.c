/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Common Report Module                         
* Category    : Report For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: RPTCOM.C
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*       2015  -> GenPOS Rel 2.3.0 (EMV for Electronic Payment)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               RptPrtStatusSet() :  Set print status
*               RptElement()      :  Print each element on J and R
*               RptFeed()         :  Feed for Receipt   
*               RptSvscalc()      :  Calculate Service Total
*               RptPrtError()     :  Print Error Status 
*               RptRound()        :
*               RptEODChkFileExist() : Check File Exist or Not
*               RptPTDChkFileExist() : Check PTD File Exist or Not
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Dec-07-92: 01.00.00 : K.you     : Del <rfl.h>                                   
* Jul-07-93: 01.00.12 : J.IKEDA   : Adds RptRound() for Hourly Activity Rpt.
* Jul-22-93: 01.00.12 : M.Suzuki  : Move RptEODChkFileExist() form rpteod.c.
*                                   Move RptPTDChkFileExist() from rptptd.c.
* Dec-11-99:01.00.00  : hrkato    : Saratoga
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
/*
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/
#include <tchar.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <ecr.h>
#include <uie.h>
#include <pif.h>
#include <rfl.h>
#include <paraequ.h>
#include <para.h>
#include <fsc.h>
#include <csstbpar.h>
#include <maint.h>
#include <cscas.h>
#include <csstbcas.h>
#include <cswai.h>
#include <csstbwai.h>
#include <csop.h>
#include <csstbopr.h>
#include <csttl.h>
#include <csstbttl.h>
#include <csstbstb.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <report.h>
#include <mld.h>
#include <mldsup.h>
#include <item.h>
#include <plu.h>
#include "rptcom.h"
#include <appllog.h>


UCHAR   uchRptOpeCount;
UCHAR   uchRptWgtFlag;
USHORT  usRptPrintStatus;
UCHAR   uchRptMldAbortStatus;  /* abort status by LCD R3.0 */
/*
*=============================================================================
**  Synopsis: VOID RptInitialize( VOID )
* 
*               
*       Input:  nothing
*      Output:  nothing
*
**  Return:                 : nothing  
*
**  Description: Clear report own work area. 
*===============================================================================
*/
VOID RptInitialize( VOID )
{
    uchRptOpeCount = 0;         /* Store the Flag Bit for Header */
    uchRptWgtFlag = 0;          /* Weight Flag for Total */
    uchRptMldAbortStatus = 0;   /* abort status by MLD */
}


/*
*=============================================================================
**  Synopsis: VOID RptPrtStatusSet(UCHAR uchMinorClass)
* 
*               
*       Input:  
*      Output:  nothing
*
**  Return:                 : nothing  
*
**  Description:  
*       Check Read or Reset report, and check also MDC bit.
*       Set status of those condition.  
*===============================================================================
*/
VOID RptPrtStatusSet(UCHAR uchMinorClass)
{
    /* Set print status in static area */
                                                                     
    usRptPrintStatus = 0;                                                        /* Clear print status  */
    if ((uchMinorClass == CLASS_TTLCURDAY) ||                                 /* if Read report?     */ 
         (uchMinorClass == CLASS_TTLCURPTD)) {
        if (!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT0))) {                /* J print ? MDC check */
            usRptPrintStatus |= PRT_JOURNAL;                               /* Set J print status  */
        }
        if (!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT1))) {                /* R print ? MDC check */ 
            usRptPrintStatus |= PRT_RECEIPT;                               /* Set R print status  */
        }
    } else {                                                                  /* if Reset report ?   */
        if (!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT2))) {                /* J print ? MDC check */
            usRptPrintStatus |= PRT_JOURNAL;                               /* Set J print status  */
        }
        if (!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT3))) {                /* R print ? MDC check */ 
            usRptPrintStatus |= PRT_RECEIPT;                               /* Set R print status  */
        }
    }
}
/*
*===========================================================================
**  Synopsis:  VOID  RptPrtTime( UCHAR uchTransAddr, DATE *pDate)
*               
*       Input:  UCHAR   uchTransAddr    :
*               DATE    * pDate         :
*
*      Output:  nothing
*
**  Return: nothing
*
**  Description:  
*       Print out TIME FROM and TO.
*===========================================================================
*/
VOID RptPrtTime(USHORT uchTransAddr, N_DATE *pDate)
{
	RPTREGFIN RptRegFn = {0};

    if (uchRptMldAbortStatus) return;                       /* aborted by MLD */

	if (RptCheckReportOnStream()) return;

    RflGetTranMnem (RptRegFn.aszTransMnemo, uchTransAddr);
    RptRegFn.uchMajorClass = CLASS_RPTREGFIN;               /* Set Major for print out      */
    RptRegFn.uchMinorClass = CLASS_RPTREGFIN_PRTTIME;       /* Set Minor for print out      */
    RptRegFn.Date = *pDate;                                 /* Set Date  for print out      */
    RptRegFn.usPrintControl = usRptPrintStatus;             /* Set J & R print status */
    if (RptCheckReportOnMld()) {
        MldDispItem(&RptRegFn, 0);                          /* |--------------------------| */
        RptRegFn.usPrintControl &= PRT_JOURNAL;       /* Reset Receipt print status so only goes to Electronic Journal if set */
    }
    PrtPrintItem(NULL, &RptRegFn);                          /* |--------------------------| */
}                                                           /* | XXXXXXXXXX   Z9:99AM9/99 | */
                                                            /* |--------------------------| */
/*
*=============================================================================
**  Synopsis: VOID RptElement(UCHAR uchTransAddr, TOTAL *pTtlData,
*                             D13DIGITS Amount13, LONG lAmount,
*                             UCHAR uchMinor, UCHAR uchBonusRate)
* 
*               
*       Input:  uchTransAddr, *pDate, *pTtlData, Amount13, lAmount, uchMinor,
*               uchBonusRate 
*      Output:  nothing
*
**  Return:                 : nothing  
*
**  Description:  
*       Prepare to set the data for print format function. 
*===============================================================================
*/

VOID RptElement(USHORT uchTransAddr, TOTAL *pTtlData,
                D13DIGITS Amount13, DCURRENCY lAmount, 
                UCHAR uchMinor, UCHAR uchBonusRate)
{
	RPTREGFIN FinData = {0};

    if (uchRptMldAbortStatus) return;                       /* aborted by MLD */
    
    if (uchTransAddr != 0) {
        RflGetTranMnem (FinData.aszTransMnemo, uchTransAddr);
    }       
    switch (uchMinor) {                                      
    case CLASS_RPTREGFIN_PRTTTLCNT:                         /* |--------------------------| */
        FinData.Total = *pTtlData;                          /* | XXXXXXXXXX        SZZZZ9 | */
        break;                                              /* |             SZZZZZZZ9.99 | */             
                                                            /* |--------------------------| */
                                                            
    case CLASS_RPTREGFIN_PRTDGG:                            /* |--------------------------| */
        FinData.Total.lAmount = lAmount;                    /* | XXXXXXXXXX  SZZZZZZZ9.99 | */
        break;                                              /* |--------------------------| */
    
    case CLASS_RPTREGFIN_PRTCGG:                            /* |--------------------------| */
        FinData.Total13D = Amount13;                        /* | XXXXXXXXXX  SZZZZZZZ9.99 | */
        break;                                              /* |--------------------------| */
  
    case CLASS_RPTREGFIN_PRTBONUS:                          /* |--------------------------| */
        FinData.Total.lAmount = lAmount;                    /* | ZZ9%        SZZZZZZZ9.99 | */
        FinData.Total.sCounter = (SHORT)uchBonusRate;       /* |--------------------------| */
        break;  
 
    case CLASS_RPTREGFIN_PRTNO:                             /* |--------------------------| */
        FinData.Total.lAmount = lAmount;                    /* | XXXXXXXXXX        SZZZZ9 | */
        break;                                              /* |--------------------------| */

    case CLASS_RPTREGFIN_PRTCNT:                            /* |--------------------------| */                           
    case CLASS_RPTREGFIN_PRTFCP0:                           /* |           SZZZZZZZ9999   | */
    case CLASS_RPTREGFIN_PRTFCP2:                           /* |           SZZZZZZZ99.9   | */
    case CLASS_RPTREGFIN_PRTFCP3:                           /* |           SZZZZZZZ9.99   | */
        FinData.Total.lAmount = lAmount;                    /* |--------------------------| */
        break;

    default:  
        return;
    }
    FinData.uchMajorClass = CLASS_RPTREGFIN;                /* Set Major for Print    */
    FinData.uchMinorClass = uchMinor;                       /* Set Minor for Print    */
    FinData.usPrintControl = usRptPrintStatus;              /* Set J & R print status */
    if (RptCheckReportOnMld()) {
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&FinData, 0); /* display on LCD          */ 
        FinData.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }
    PrtPrintItem(NULL,&FinData);                            /* print out              */ 
}

//----------------------------------------------------------------------------------------
// The following tables are used to print the report data in a particular format suitable
// for the report type (csv, html, xml, etc.).
// Some of the line item format types are used by more than one report. For example
// the CLASS_RPTREGFIN_xxx format types are used by both the AC 21 Cashier Report and
// the AC 23 Financial Report.
RptElementOutputClassType  rptTypeOutputXml [] = {
		{ CLASS_RPTREGFIN_PRTTTLCNT, L"<Key name=\"%s\" value=\"%L$\" count=\"%d\" />" },    // AC 21 Cashier Report and AC 23 Financial Report
		{ CLASS_RPTREGFIN_PRTDGG, L"<Key name=\"%s\" value=\"%l$\" />" },                    // AC 21 Cashier Report and AC 23 Financial Report
		{ CLASS_RPTREGFIN_PRTCGG, L"<Key name=\"%s\" value=\"%L$\" />" },                    // AC 21 Cashier Report and AC 23 Financial Report
		{ CLASS_RPTREGFIN_PRTBONUS, L"<Key name=\"%s\" value=\"%l$\" count=\"%d\" />" },     // AC 21 Cashier Report and AC 23 Financial Report
		{ CLASS_RPTREGFIN_PRTNO, L"<Key name=\"%s\" value=\"%ld\" />" },                     // AC 21 Cashier Report and AC 23 Financial Report
		{ CLASS_RPTREGFIN_PRTCNT, L"<Key name=\"%s\" value=\"%ld\" />" },                    // AC 21 Cashier Report and AC 23 Financial Report
		{ CLASS_RPTREGFIN_PRTFCP0, L"<Key name=\"%s\" value=\"%ld\" />" },                   // AC 21 Cashier Report and AC 23 Financial Report
		{ CLASS_RPTREGFIN_PRTFCP2, L"<Key name=\"%s\" value=\"%ld\" />" },                   // AC 21 Cashier Report and AC 23 Financial Report
		{ CLASS_RPTREGFIN_PRTFCP3, L"<Key name=\"%s\" value=\"%ld\" />" },                   // AC 21 Cashier Report and AC 23 Financial Report
		{ CLASS_RPTCPN_ITEM, L"<Key cpn=\"%4d\" name=\"%-12s\" count=\"%ld\" value=\"%l$\" />" },  // AC30 Coupon Report
		{ CLASS_RPTCPN_TOTAL, L"<Key name=\"%-8.8s\" count=\"%ld\" value=\"%l$\" />" },            // AC30 Coupon Report
		{0, NULL}
	};

RptElementOutputClassType  rptTypeOutputHtml [] = {
		{ CLASS_RPTREGFIN_PRTTTLCNT, L"<tr><td>%s</td><td align=\"right\">%l$</td><td align=\"right\">%d</td></tr>\r\n" },
		{ CLASS_RPTREGFIN_PRTDGG, L"<tr><td>%s</td><td align=\"right\">%l$</td><td align=\"right\"></td></tr>\r\n" },
		{ CLASS_RPTREGFIN_PRTCGG, L"<tr><td>%s</td><td align=\"right\">%L$</td><td align=\"right\"></td></tr>\r\n" },
		{ CLASS_RPTREGFIN_PRTBONUS, L"<tr><td>%s</td><td align=\"right\">%l$</td><td align=\"right\">%d</td></tr>\r\n" },
		{ CLASS_RPTREGFIN_PRTNO, L"<tr><td>%s</td><td align=\"right\">%ld</td><td align=\"right\"></td></tr>\r\n" },
		{ CLASS_RPTREGFIN_PRTCNT, L"<tr><td>%s</td><td align=\"right\">%ld</td><td align=\"right\"></td></tr>\r\n" },
		{ CLASS_RPTREGFIN_PRTFCP0, L"<tr><td>%s</td><td align=\"right\">%ld</td><td align=\"right\"></td></tr>\r\n" },
		{ CLASS_RPTREGFIN_PRTFCP2, L"<tr><td>%s</td><td align=\"right\">%ld</td><td align=\"right\"></td></tr>\r\n" },
		{ CLASS_RPTREGFIN_PRTFCP3, L"<tr><td>%s</td><td align=\"right\">%ld</td><td align=\"right\"></td></tr>\r\n" },
		{ CLASS_RPTCPN_ITEM, L"<tr><td align=\"right\">%d</td><td align=\"left\">%-12s</td><td align=\"right\">%ld</td><td align=\"right\">%l$</td></tr>\r\n" },
		{ CLASS_RPTCPN_TOTAL, L"<tr><td></td><td align=\"left\">%-8.8s</td><td align=\"right\">%ld</td><td align=\"right\">%l$</td></tr>\r\n" },
		{0, NULL}
	};

RptElementOutputClassType  rptTypeOutputCsv [] = {
		{ CLASS_RPTREGFIN_PRTTTLCNT, L"\"%s\",%l$,%d\r\n" },
		{ CLASS_RPTREGFIN_PRTDGG, L"\"%s\",%l$,\r\n" },
		{ CLASS_RPTREGFIN_PRTCGG, L"\"%s\",%L$,\r\n" },
		{ CLASS_RPTREGFIN_PRTBONUS, L"\"%s\",%l$,%d\r\n" },
		{ CLASS_RPTREGFIN_PRTNO, L"\"%s\",%ld,\r\n" },
		{ CLASS_RPTREGFIN_PRTCNT, L"\"%s\",%ld,\r\n" },
		{ CLASS_RPTREGFIN_PRTFCP0, L"\"%s\",%ld,\r\n" },
		{ CLASS_RPTREGFIN_PRTFCP2, L"\"%s\",%ld,\r\n" },
		{ CLASS_RPTREGFIN_PRTFCP3, L"\"%s\",%ld,\r\n" },
		{ CLASS_RPTCPN_ITEM, L"%d,\"%-12s\",%ld,%l$\r\n" },
		{ CLASS_RPTCPN_TOTAL, L"0,\"%-8.8s\",%ld,%l$\r\n"},
		{0, NULL}
	};

RptElementOutput  RptRegFinOutput[] = {
		{ RPTREGFIN_OUTPUT_NONE, RPTREGFIN_FOLDER_ANY, rptTypeOutputHtml, sizeof(rptTypeOutputHtml) / sizeof(rptTypeOutputHtml[0]) },
		{ RPTREGFIN_OUTPUT_CSV, RPTREGFIN_FOLDER_ANY, rptTypeOutputCsv, sizeof(rptTypeOutputCsv)/sizeof(rptTypeOutputCsv[0]) },
		{ RPTREGFIN_OUTPUT_XML, RPTREGFIN_FOLDER_ANY, rptTypeOutputXml, sizeof(rptTypeOutputXml)/sizeof(rptTypeOutputXml[0]) },
		{ RPTREGFIN_OUTPUT_HTML, RPTREGFIN_FOLDER_ANY, rptTypeOutputHtml, sizeof(rptTypeOutputHtml) / sizeof(rptTypeOutputHtml[0])},
		{ RPTREGFIN_OUTPUT_HUBWORKS, RPTREGFIN_FOLDER_ANY, rptTypeOutputCsv, sizeof(rptTypeOutputCsv) / sizeof(rptTypeOutputCsv[0])}
	};

FILE *fpRptElementStreamFile = NULL;
RptDescription dataRptElementStream = {0};

RptDescription RptDescriptionCreate (USHORT usPGACNo, UCHAR uchMinorClass, UCHAR uchType, FILE *fpFile, RptElementOutputFolder tFolder, RptElementOutputType tOutputType, RptElementFunc pOutputFunc)
{
	SHORT    i;
	RptDescription    rptDescription = {0};
	RptElementOutput  rptDefault = { RPTREGFIN_OUTPUT_HTML, RPTREGFIN_FOLDER_ANY, rptTypeOutputHtml, sizeof(rptTypeOutputHtml)/sizeof(rptTypeOutputHtml[0]) };

	// If we are given a valid or at least not NULL file pointer then we will
	// set up the environment for the historical report. However if not, then
	// we will just assume it never happened.
	if (fpFile) {
		rptDescription.fpOut = fpFile;
		rptDescription.pOutputFunc = pOutputFunc;
		rptDescription.uchMinorClass = uchMinorClass;
		rptDescription.uchType = uchType;
		rptDescription.usPGACNo = usPGACNo;
		rptDescription.rptDescrip = rptDefault;
		for (i = 0; i < sizeof(RptRegFinOutput)/sizeof(RptRegFinOutput[0]); i++) {
			if (tOutputType == RptRegFinOutput[i].usType) {
				rptDescription.rptDescrip = RptRegFinOutput[i];
				rptDescription.rptDescrip.usFolder = tFolder;
				break;
			}
		}

		rptDescription.uchUifACRptOnOffMldSave = uchUifACRptOnOffMld;
	}

	return rptDescription;
}

// Minimal RptDescription created for generating html or csv reports mainly.
RptDescription RptDescriptionCreateMini(FILE* fpFile, RptElementOutputFolder tFolder, RptElementOutputType tOutputType, RptElementFunc pOutputFunc)
{
	SHORT    i;
	RptDescription    rptDescription = { 0 };
	RptElementOutput  rptDefault = { RPTREGFIN_OUTPUT_HTML, RPTREGFIN_FOLDER_ANY, rptTypeOutputHtml, sizeof(rptTypeOutputHtml) / sizeof(rptTypeOutputHtml[0]) };

	rptDescription.fpOut = fpFile;
	rptDescription.pOutputFunc = pOutputFunc;
	rptDescription.rptDescrip = rptDefault;
	for (i = 0; i < sizeof(RptRegFinOutput) / sizeof(RptRegFinOutput[0]); i++) {
		if (tOutputType == RptRegFinOutput[i].usType) {
			rptDescription.rptDescrip = RptRegFinOutput[i];
			rptDescription.rptDescrip.usFolder = tFolder;
			break;
		}
	}

	rptDescription.uchUifACRptOnOffMldSave = uchUifACRptOnOffMld;
	uchUifACRptOnOffMld = RPT_DISPLAY_STREAM;

	return rptDescription;
}

RptDescription RptDescriptionSet (RptDescription  rptDescrip)
{
	RptDescription rptDescriptSave = dataRptElementStream;

	dataRptElementStream = rptDescrip;
	return rptDescriptSave;
}

RptElementFlags RptDescriptionSetFlag(RptElementFlags  uFlag)
{
	RptElementFlags  xFlag = dataRptElementStream.uFlags;
	dataRptElementStream.uFlags |= uFlag;

	return xFlag;
}

int RptDescriptionCheckFlag(RptElementFlags  uFlag)
{
	return (dataRptElementStream.uFlags & uFlag) != 0;
}

int RptDescriptionCheckType(RptElementOutputType tOutputType)
{
	return (dataRptElementStream.rptDescrip.usType == tOutputType);
}

RptElementOutputType RptDescriptionGetType(void)
{
	return dataRptElementStream.rptDescrip.usType;
}

RptDescription RptDescriptionGet (VOID)
{
	return dataRptElementStream;
}

FILE * RptDescriptionGetStream(VOID)
{
	return dataRptElementStream.fpOut;
}

RptDescription RptDescriptionSwap (RptDescription  rptDescrip)
{
	RptDescription    rptDescriptionSave = dataRptElementStream;

	dataRptElementStream = rptDescrip;  // overwrite with new version and return the old version.

	return rptDescriptionSave;
}

RptDescription RptDescriptionClear (VOID)
{
	RptDescription    rptDescriptionNull = {0};

	return RptDescriptionSwap (rptDescriptionNull);
}

RptDescription RptDescriptionClose(VOID)
{
	if (dataRptElementStream.fpOut) {
		fclose(dataRptElementStream.fpOut);
	}

	uchUifACRptOnOffMld = dataRptElementStream.uchUifACRptOnOffMldSave;  // restore the AC Report MLD output state

	return RptDescriptionClear();
}


VOID RptElementStream (USHORT uchTransAddr, TOTAL *pTtlData,
                D13DIGITS Amount13, DCURRENCY lAmount, 
                UCHAR uchMinor, UCHAR uchBonusRate)
{
	wchar_t    aszPrintLine[256] = {0};
	wchar_t    aszTransMnemo[24] = {0};
	RptDescription dataRptDescription = RptDescriptionGet();
    
	if (fpRptElementStreamFile == NULL) return;

	if (dataRptDescription.rptDescrip.usType == RPTREGFIN_OUTPUT_HUBWORKS) {
		RptElementHubworks(uchTransAddr, pTtlData, Amount13, lAmount, uchMinor, uchBonusRate);
		return;
	}

    if (uchTransAddr != 0) {
		RflGetTranMnem(aszTransMnemo, uchTransAddr);
		RflCleanupMnemonic(aszTransMnemo);
    }

	for (RptElementOutputClassType* rptTypeOutput = dataRptDescription.rptDescrip.outputClass; rptTypeOutput && rptTypeOutput->aszFormat; rptTypeOutput++) {
		if (rptTypeOutput->uchMinor == uchMinor) {
			switch (uchMinor) {                                      
			case CLASS_RPTREGFIN_PRTTTLCNT:
				RflSPrintf (aszPrintLine, 250, rptTypeOutput->aszFormat, aszTransMnemo, pTtlData->lAmount, pTtlData->sCounter);
				fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
				break;
		                                                            
			case CLASS_RPTREGFIN_PRTDGG:
				RflSPrintf (aszPrintLine, 250, rptTypeOutput->aszFormat, aszTransMnemo, lAmount);
				fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
				break;
		    
			case CLASS_RPTREGFIN_PRTCGG:
				RflSPrintf (aszPrintLine, 250, rptTypeOutput->aszFormat, aszTransMnemo, Amount13);
				fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
				break;
		  
			case CLASS_RPTREGFIN_PRTBONUS:
				RflSPrintf (aszPrintLine, 250, rptTypeOutput->aszFormat, aszTransMnemo, lAmount, (SHORT)uchBonusRate);
				fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
				break;  
		 
			case CLASS_RPTREGFIN_PRTNO:
				RflSPrintf (aszPrintLine, 250, rptTypeOutput->aszFormat, aszTransMnemo, (LONG)lAmount);
				fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
				break;

			case CLASS_RPTREGFIN_PRTCNT:
			case CLASS_RPTREGFIN_PRTFCP0:
			case CLASS_RPTREGFIN_PRTFCP2:
			case CLASS_RPTREGFIN_PRTFCP3:
				RflSPrintf (aszPrintLine, 250, rptTypeOutput->aszFormat, aszTransMnemo, (LONG)lAmount);
				fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
				break;

			default: 
				if (dataRptDescription.uFlags & RPTFLAGS_LOG_01) {
					// issue log of error but only once for this report to prevent cluttering up the log file.
					NHPOS_ASSERT_TEXT(0, "**ERROR: RPTFLAGS_LOG_01 - Unknown rptTypeOutput->uchMinor.");
					RptDescriptionSetFlag(RPTFLAGS_LOG_01);
				}
				break;
			}
			break;
		}
	}
}

VOID RptElementStream2(USHORT uchTransAddr, USHORT usId, wchar_t *aszMnemo, TOTAL* pTtlData,
	D13DIGITS Amount13, DCURRENCY lAmount, LONG lCounter,
	UCHAR uchMinor, UCHAR uchBonusRate)
{
	wchar_t    aszPrintLine[256] = { 0 };
	wchar_t    aszTransMnemo[24] = { 0 };
	RptDescription dataRptDescription = RptDescriptionGet();

	if (fpRptElementStreamFile == NULL) return;

	if (dataRptDescription.rptDescrip.usType == RPTREGFIN_OUTPUT_HUBWORKS) {
		return;
	}

	if (uchTransAddr != 0) {
		RflGetTranMnem(aszTransMnemo, uchTransAddr);
		RflCleanupMnemonic(aszTransMnemo);
	}

	for (RptElementOutputClassType* rptTypeOutput = dataRptDescription.rptDescrip.outputClass; rptTypeOutput && rptTypeOutput->aszFormat; rptTypeOutput++) {
		if (rptTypeOutput->uchMinor == uchMinor) {
			switch (uchMinor) {

			case CLASS_RPTCPN_ITEM:
				RflSPrintf(aszPrintLine, 250, rptTypeOutput->aszFormat, usId, aszMnemo, lCounter, lAmount);
				fprintf(fpRptElementStreamFile, "%S\r\n", aszPrintLine);
				break;

			case CLASS_RPTCPN_TOTAL:
				RflSPrintf(aszPrintLine, 250, rptTypeOutput->aszFormat, aszMnemo, lCounter, lAmount);
				fprintf(fpRptElementStreamFile, "%S\r\n", aszPrintLine);
				break;

			default:
				if (dataRptDescription.uFlags & RPTFLAGS_LOG_01) {
					// issue log of error but only once for this report to prevent cluttering up the log file.
					NHPOS_ASSERT_TEXT(0, "**ERROR: RPTFLAGS_LOG_01 - Unknown rptTypeOutput->uchMinor.");
					RptDescriptionSetFlag(RPTFLAGS_LOG_01);
				}
				break;
			}
			break;
		}
	}
}


/*
 *    An actual Hubworks Daily Summary file would looks like the following. The XML file
 *    name would be of the form "daily_YYYYMMDD.xml" where YYYYMMDD is the business date.
 *    The names that begin with a dollar sign ($) are special Hubworks keywords for those
 *    data types. Custom, non-Hubworks names should not begin with a dollar sign.
 *        <?xml version="1.0" encoding="utf-8"?>
 *        <HubWorks>
 *        <DailyKeys>
 *        <Key name="$cb_Gross_Sales_Amt" value="10742.25"/>
 *        <Key name="$cb_Net_Sales_Amt" value="10703.78"/>
 *        <Key name="$cb_Transaction_Cnt" value="621"/>
 *        <Key name="$cb_Guest_Cnt" value="646"/>
 *        <Key name="$hub_Discount_Total_Amt" value="52.32"/>
 *        <Key name="$cb_Actual_Labor_Hours_Amt" value="114.88"/>
 *        <Key name="$cb_Actual_Labor_Dollars_Amt" value="1481.76"/>
 *        <Key name="$hub_Overtime_Labor_Hours_Amt" value="6.41"/>
 *        <Key name="$hub_Overtime_Labor_Dollars_Amt" value="84.80"/>
 *        <Key name="$hub_Deposit_Total_Amt" value="1507.07"/>
 *        <Key name="$hub_Sales_Tax_Total_Amt" value="630.71"/>
 *        <Key name="$hub_PAYMENT_Total_Amt" value="11735.5"/>
 *        <Key name="$hub_PAYMENT_CASH_Amt" value="1612.3"/>
 *        <Key name="$hub_PAYMENT_AMEX_Amt" value="3967.77"/>
 *        <Key name="$hub_PAYMENT_MASTERCARD_Amt" value="2955.45"/>
 *        <Key name="$hub_PAYMENT_VISA_Amt" value="2867.23"/>
 *        <Key name="$hub_PAYMENT_GIFTCARD_Amt" value="7.79"/>
 *        <Key name="$hub_PAYMENT_TRAVCHECK_Amt" value="24.22"/>
 *        <Key name="POS_REFUNDS_AMT" value="18.23"/>
 *        </DailyKeys>
 *        </HubWorks>
**/

static  WCHAR *RptHubworksTranMnem (WCHAR *pszDest1, WCHAR *pszDest2, USHORT usAddress)
{
	struct {
		USHORT  usTransAddr;
		wchar_t  *wcsTag[2];
	} transToTagTable[] = {
		{TRN_DAIGGT_ADR,  L"$cb_Gross_Sales_Amt",  L"$cb_Transaction_Cnt"},
		{TRN_NETTTL_ADR,  L"$cb_Net_Sales_Amt", NULL},
		{TRN_PSN_ADR, L"$cb_Guest_Cnt", NULL},
		{TRN_TEND1_ADR, L"$hub_PAYMENT_CASH_Amt", NULL}
//		{0, L"$hub_Discount_Total_Amt", NULL},
//		{0, L"$hub_Sales_Tax_Total_Amt", NULL}
	};
	USHORT usIndex;

	pszDest1 [0] = 0;
	pszDest2 [0] = 0;
	for (usIndex = 0; usIndex < sizeof(transToTagTable)/sizeof(transToTagTable[0]); usIndex++) {
		if (transToTagTable[usIndex].usTransAddr == usAddress) {
			if (pszDest1 && transToTagTable[usIndex].wcsTag[0])
				wcscpy (pszDest1, transToTagTable[usIndex].wcsTag[0]);
			if (pszDest2 && transToTagTable[usIndex].wcsTag[1])
				wcscpy (pszDest2, transToTagTable[usIndex].wcsTag[1]);
			return transToTagTable[usIndex].wcsTag[0];
		}
	}

	return NULL;
}

VOID RptElementHubworks (USHORT uchTransAddr, TOTAL *pTtlData,
                D13DIGITS Amount13, DCURRENCY lAmount, 
                UCHAR uchMinor, UCHAR uchBonusRate)
{
	wchar_t  aszMnemonic1[40] = {0};
	wchar_t  aszMnemonic2[40] = {0};
	wchar_t  aszPrintLine[256] = {0};
    
	if (fpRptElementStreamFile == NULL) return;

    if (uchTransAddr != 0) {
        RptHubworksTranMnem (aszMnemonic1, aszMnemonic2, uchTransAddr);
    }

	if (uchTransAddr < 1 || aszMnemonic1[0] < 1) return;

    switch (uchMinor) {                                      
    case CLASS_RPTREGFIN_PRTTTLCNT:
		if (aszMnemonic1[0]) {
			RflSPrintf (aszPrintLine, 250, L"<Key name=\"%s\" value=\"%L$\" />", aszMnemonic1, pTtlData->lAmount);
			fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
		}
		if (aszMnemonic2[0]) {
			RflSPrintf (aszPrintLine, 250, L"<Key name=\"%s\" value=\"%d\" />", aszMnemonic2, pTtlData->sCounter);
			fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
		}
		break;
                                                            
    case CLASS_RPTREGFIN_PRTDGG:
		if (aszMnemonic1[0]) {
			RflSPrintf (aszPrintLine, 250, L"<Key name=\"%s\" value=\"%l$\" />", aszMnemonic1, lAmount);
			fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
		}
		break;
    
    case CLASS_RPTREGFIN_PRTCGG:
		if (aszMnemonic1[0]) {
			RflSPrintf (aszPrintLine, 250, L"<Key name=\"%s\" value=\"%L$\" />", aszMnemonic1, Amount13);
			fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
		}
		break;
  
    case CLASS_RPTREGFIN_PRTBONUS:
		if (aszMnemonic1[0]) {
			RflSPrintf (aszPrintLine, 250, L"<Key name=\"%s\" value=\"%l$\" />", aszMnemonic1, lAmount);
			fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
		}
		if (aszMnemonic2[0]) {
			RflSPrintf (aszPrintLine, 250, L"<Key name=\"%s\" value=\"%d\" />", aszMnemonic2, (SHORT)uchBonusRate);
			fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
		}
        break;  
 
    case CLASS_RPTREGFIN_PRTNO:
		if (aszMnemonic1[0]) {
			RflSPrintf (aszPrintLine, 250, L"<Key name=\"%s\" value=\"%ld\" />", aszMnemonic1, (LONG)lAmount);
			fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
		}
		break;

    case CLASS_RPTREGFIN_PRTCNT:
    case CLASS_RPTREGFIN_PRTFCP0:
    case CLASS_RPTREGFIN_PRTFCP2:
    case CLASS_RPTREGFIN_PRTFCP3:
		if (aszMnemonic1[0]) {
			RflSPrintf (aszPrintLine, 250, L"<Key name=\"%s\" value=\"%ld\" />", aszMnemonic1, (LONG)lAmount);
			fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
		}
        break;

    default:  
        return;
    }
}

VOID RptElementStandardXml (USHORT uchTransAddr, TOTAL *pTtlData,
                D13DIGITS Amount13, DCURRENCY lAmount, 
                UCHAR uchMinor, UCHAR uchBonusRate)
{
	USHORT   iLoop;
	wchar_t  aszMnemonic[40] = {0};
	wchar_t  aszPrintLine[256] = {0};
    
	if (fpRptElementStreamFile == NULL) return;

    if (uchTransAddr != 0) {
        RflGetTranMnem (aszMnemonic, uchTransAddr);
		RflCleanupMnemonic (aszMnemonic);
	} else {
		return;
	}

	for (iLoop = 0; iLoop < sizeof(rptTypeOutputXml)/sizeof(rptTypeOutputXml[0]); iLoop++) {
		if (rptTypeOutputXml[iLoop].uchMinor == uchMinor) {
			switch (uchMinor) {                                      
			case CLASS_RPTREGFIN_PRTTTLCNT:
				RflSPrintf (aszPrintLine, 250, rptTypeOutputXml[iLoop].aszFormat, aszMnemonic, pTtlData->lAmount, pTtlData->sCounter);
				fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
				break;
		                                                            
			case CLASS_RPTREGFIN_PRTDGG:
				RflSPrintf (aszPrintLine, 250, rptTypeOutputXml[iLoop].aszFormat, aszMnemonic, lAmount);
				fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
				break;
		    
			case CLASS_RPTREGFIN_PRTCGG:
				RflSPrintf (aszPrintLine, 250, rptTypeOutputXml[iLoop].aszFormat, aszMnemonic, Amount13);
				fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
				break;
		  
			case CLASS_RPTREGFIN_PRTBONUS:
				RflSPrintf (aszPrintLine, 250, rptTypeOutputXml[iLoop].aszFormat, aszMnemonic, lAmount, (SHORT)uchBonusRate);
				fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
				break;  
		 
			case CLASS_RPTREGFIN_PRTNO:
				RflSPrintf (aszPrintLine, 250, rptTypeOutputXml[iLoop].aszFormat, aszMnemonic, (LONG)lAmount);
				fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
				break;

			case CLASS_RPTREGFIN_PRTCNT:
			case CLASS_RPTREGFIN_PRTFCP0:
			case CLASS_RPTREGFIN_PRTFCP2:
			case CLASS_RPTREGFIN_PRTFCP3:
				RflSPrintf (aszPrintLine, 250, rptTypeOutputXml[iLoop].aszFormat, aszMnemonic, (LONG)lAmount);
				fprintf (fpRptElementStreamFile, "%S\r\n", aszPrintLine);
				break;

			default:  
				break;
			}
			break;
		}
	}
}

// Hubworks output file is a special XML file whose format and tags is specified by
// the Hubworks cloud point of sale ecosystem. See https://hubworks.com/
static FILE * ItemOpenHistorialReportsFolderHubworks (char* pathBuffer, USHORT usPGACNo, UCHAR uchMinorClass, UCHAR uchType, SHORT  iYear, SHORT  iMonth, SHORT  iDay)
{
	CONST SYSCONFIG    *pSysConfig = PifSysConfig();       /* get system config */
	FILE   *pFile = NULL;
	char   name[64] = {0};

	sprintf (name, "\\daily_%4.4d%2.2d%2.2d.xml", iYear, iMonth, iDay);
	strcat (pathBuffer, name);
	pFile = fopen (pathBuffer, "w+b");

	if (pFile) {
		fpRptElementStreamFile = pFile;
		uchUifACRptOnOffMld = RPT_DISPLAY_STREAM;
		fprintf(fpRptElementStreamFile, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
		fprintf(fpRptElementStreamFile, "<HubWorks>\r\n<DailyKeys>\r\n");
	}

	return pFile;
}

static struct {
	USHORT  usPGACNo;
	char* nameFile;       // prefix for the output file name used for the various types of reports
	char* nameTitle;      // name to use as part of the title.
	char* nameReport;     // name to use for the xml identifier
} ReportNameList[] = {
	{ AC_CASH_READ_RPT, "ac21out_", "AC 21 Operator", "ac21report" },            // AC 21 Operator report file name and title.
	{ AC_REGFIN_READ_RPT, "ac23out_", "AC 23 Financial", "ac23report"},          // AC 23 Financial report file name and title.
	{ AC_HOURLY_READ_RPT, "ac24out_", "AC 24 Hourly Activity", "ac24report" },   // AC 24 Hourly Activity report file name and title.
	{ AC_DEPTSALE_READ_RPT, "ac26out_", "AC 26 Department", "ac26report" },      // AC 26 Department report file name and title.
	{ AC_PLUSALE_READ_RPT, "ac29out_", "AC 29 PLU Sales", "ac29report" },        // AC 29 PLU report file name and title.
	{ AC_CPN_READ_RPT, "ac30out_", "AC 30 Coupon", "ac30report" },               // AC 30 Coupon report file name and title.
	{ AC_MAJDEPT_RPT, "ac122out_", "AC 122 Major Department", "ac122report" },   // AC 122 Major Department report file name and title.
	{ AC_EOD_RPT, "ac99out_", "AC 99 End of Day", "ac99report" },                // AC 99 End of Day report file name and title.
	{ 0, "ac00out_", "AC 00", "AC00report" }                                // place holder or default file name and title.
};

static char* docPathRootWeb = "C:\\FlashDisk\\NCR\\Saratoga\\Web";    // should be same as STD_FOLDER_WEBFOLDER


static FILE* ItemOpenHistorialReportsFolderXml(char* pathBuffer, USHORT usPGACNo, UCHAR uchMinorClass, UCHAR uchType, SHORT  iYear, SHORT  iMonth, SHORT  iDay)
{
	CONST SYSCONFIG* pSysConfig = PifSysConfig();       /* get system config */
	FILE* pFile = NULL;
	SHORT  iLoop;
	char   name[64] = { 0 };
	char* docPathFormat = NULL;

	switch (uchMinorClass) {
	case CLASS_TTLCURDAY:
		docPathFormat = "\\%sCurDaily_%4.4d%2.2d%2.2d.xml";
		break;
	case CLASS_TTLCURPTD:
		docPathFormat = "\\%sCurPTD_%4.4d%2.2d%2.2d.xml";
		break;
	case CLASS_TTLSAVDAY:
		docPathFormat = "\\%sSavedDaily_%4.4d%2.2d%2.2d.xml";
		break;
	case CLASS_TTLSAVPTD:
		docPathFormat = "\\%sSavedPTD_%4.4d%2.2d%2.2d.xml";
		break;
	default:
		{
			sprintf(pathBuffer, "**ERROR: ItemOpenHistorialReportsFolderXml() - Bad uchMinorClass %d", uchMinorClass);
			NHPOS_ASSERT_TEXT(0, pathBuffer)
		}
	return NULL;
	break;
	}

	for (iLoop = 0; iLoop < sizeof(ReportNameList) / sizeof(ReportNameList[0]); iLoop++) {
		if (ReportNameList[iLoop].usPGACNo == usPGACNo || ReportNameList[iLoop].usPGACNo == 0) {
			break;
		}
	}

	if (iLoop >= sizeof(ReportNameList) / sizeof(ReportNameList[0])) {
		sprintf(pathBuffer, "**ERROR: ItemOpenHistorialReportsFolderXml() - ReportNameList[] not found. Bad usPGACNo %d", usPGACNo);
		NHPOS_ASSERT_TEXT(0, pathBuffer)
			return NULL;
	}

	if (iYear == 0) {
		DATE_TIME       DT;

		PifGetDateTime(&DT);
		iYear = DT.usYear;
		iMonth = DT.usMonth;
		iDay = DT.usMDay;
	}

	sprintf(name, docPathFormat, ReportNameList[iLoop].nameFile, iYear, iMonth, iDay);
	if (strlen(pathBuffer) < 2) {
		strcpy(pathBuffer, docPathRootWeb);
	}
	strcat(pathBuffer, name);
	pFile = fopen(pathBuffer, "w+b");

	if (pFile) {
		fpRptElementStreamFile = pFile;
		uchUifACRptOnOffMld = RPT_DISPLAY_STREAM;
		fprintf(fpRptElementStreamFile, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
		fprintf(fpRptElementStreamFile, "<%s>\r\n", ReportNameList[iLoop].nameReport);
	}

	return pFile;
}

/*
 *
 *    ItemGenerateAc23Report() - generate AC23 Financial Totals Report
 *    ItemGenerateAc21Report() - generate AC21 Operator Totals report
*/
static FILE * ItemOpenHistorialReportsFolderHtml (char* pathBuffer, USHORT usPGACNo, UCHAR uchMinorClass, UCHAR uchType, SHORT  iYear, SHORT  iMonth, SHORT  iDay)
{
	CONST SYSCONFIG    *pSysConfig = PifSysConfig();       /* get system config */
	FILE   *pFile = NULL;
	SHORT  iLoop;
	char   name[64] = {0};
	char   *docHeading = NULL;
	char   *docPathFormat = NULL;

	switch (uchMinorClass) {
		case CLASS_TTLCURDAY:
			docHeading = "<!DOCTYPE html>\n<html>\r\n<head>\r\n<title>%s Current Daily Totals Report</title>\r\n</head>\r\n<body>\r\n";
			docPathFormat = "\\%sCurDaily_%4.4d%2.2d%2.2d.html";
			break;
		case CLASS_TTLCURPTD:
			docHeading = "<!DOCTYPE html>\n<html>\r\n<head>\r\n<title>%s Current Period To Date Totals Report</title>\r\n</head>\r\n<body>\r\n";
			docPathFormat = "\\%sCurPTD_%4.4d%2.2d%2.2d.html";
			break;
		case CLASS_TTLSAVDAY:
			docHeading = "<!DOCTYPE html>\n<html>\r\n<head>\r\n<title>%s Saved Daily Totals Report</title>\r\n</head>\r\n<body>\r\n";
			docPathFormat = "\\%sSavedDaily_%4.4d%2.2d%2.2d.html";
			break;
		case CLASS_TTLSAVPTD:
			docHeading = "<!DOCTYPE html>\n<html>\r\n<head>\r\n<title>%s Saved Period To Date Totals Report</title>\r\n</head>\r\n<body>\r\n";
			docPathFormat = "\\%sSavedPTD_%4.4d%2.2d%2.2d.html";
			break;
		default:
			{
				sprintf(pathBuffer, "**ERROR: ItemOpenHistorialReportsFolderHtml() - Bad uchMinorClass %d", uchMinorClass);
				NHPOS_ASSERT_TEXT(0, pathBuffer)
			}
			return NULL;
			break;
	}

	for (iLoop = 0; iLoop < sizeof(ReportNameList)/sizeof(ReportNameList[0]); iLoop++) {
		if (ReportNameList[iLoop].usPGACNo == usPGACNo || ReportNameList[iLoop].usPGACNo == 0) {
			break;
		}
	}

	if (iLoop >= sizeof(ReportNameList) / sizeof(ReportNameList[0])) {
		sprintf(pathBuffer, "**ERROR: ItemOpenHistorialReportsFolderHtml() - ReportNameList[] not found. Bad usPGACNo %d", usPGACNo);
		NHPOS_ASSERT_TEXT(0, pathBuffer)
		return NULL;
	}

	if (iYear == 0) {
		DATE_TIME       DT;

		PifGetDateTime(&DT);
		iYear = DT.usYear;
		iMonth = DT.usMonth;
		iDay = DT.usMDay;
	}

	sprintf (name, docPathFormat, ReportNameList[iLoop].nameFile, iYear, iMonth, iDay);
	if (strlen (pathBuffer) < 2) {
		strcpy (pathBuffer, docPathRootWeb);
	}
	strcat (pathBuffer, name);
	pFile = fopen (pathBuffer, "w+b");

	if (pFile) {
		fpRptElementStreamFile = pFile;
		uchUifACRptOnOffMld = RPT_DISPLAY_STREAM;
		if (docHeading) {
			fprintf(fpRptElementStreamFile, docHeading, ReportNameList[iLoop].nameTitle);
		}
	}
	else {
	sprintf(pathBuffer, "**ERROR: ItemOpenHistorialReportsFolderHtml() file open - usPGACNo %d  uchMinorClass %d  uchType %d", usPGACNo, uchMinorClass, uchType);
	NHPOS_ASSERT_TEXT(pFile != NULL, pathBuffer);
	}

	return pFile;
}

/*
 *
 *    ItemGenerateAc23Report() - generate AC23 Financial Totals Report
 *    ItemGenerateAc21Report() - generate AC21 Operator Totals report
*/
static FILE* ItemOpenHistorialReportsFolderCsv(char* pathBuffer, USHORT usPGACNo, UCHAR uchMinorClass, UCHAR uchType, SHORT  iYear, SHORT  iMonth, SHORT  iDay)
{
	CONST SYSCONFIG* pSysConfig = PifSysConfig();       /* get system config */
	FILE* pFile = NULL;
	SHORT  iLoop;
	char   name[64] = { 0 };
	char* docPathFormat = NULL;

	switch (uchMinorClass) {
	case CLASS_TTLCURDAY:
		docPathFormat = "\\%sCurDaily_%4.4d%2.2d%2.2d.csv";
		break;
	case CLASS_TTLCURPTD:
		docPathFormat = "\\%sCurPTD_%4.4d%2.2d%2.2d.csv";
		break;
	case CLASS_TTLSAVDAY:
		docPathFormat = "\\%sSavedDaily_%4.4d%2.2d%2.2d.csv";
		break;
	case CLASS_TTLSAVPTD:
		docPathFormat = "\\%sSavedPTD_%4.4d%2.2d%2.2d.csv";
		break;
	default:
	{
		sprintf(pathBuffer, "**ERROR: ItemOpenHistorialReportsFolderCsv() - Bad uchMinorClass %d", uchMinorClass);
		NHPOS_ASSERT_TEXT(0, pathBuffer)
	}
	return NULL;
	break;
	}

	if (iYear == 0) {
		DATE_TIME       DT;

		PifGetDateTime(&DT);
		iYear = DT.usYear;
		iMonth = DT.usMonth;
		iDay = DT.usMDay;
	}

	for (iLoop = 0; iLoop < sizeof(ReportNameList) / sizeof(ReportNameList[0]); iLoop++) {
		if (ReportNameList[iLoop].usPGACNo == usPGACNo || ReportNameList[iLoop].usPGACNo == 0) {
			break;
		}
	}

	if (iLoop >= sizeof(ReportNameList) / sizeof(ReportNameList[0])) {
		sprintf(pathBuffer, "**ERROR: ItemOpenHistorialReportsFolderCsv() - ReportNameList[] not found. Bad usPGACNo %d", usPGACNo);
		NHPOS_ASSERT_TEXT(0, pathBuffer)
			return NULL;
	}

	sprintf(name, docPathFormat, ReportNameList[iLoop].nameFile, iYear, iMonth, iDay);
	if (strlen(pathBuffer) < 2) {
		strcpy(pathBuffer, docPathRootWeb);
	}
	strcat(pathBuffer, name);
	pFile = fopen(pathBuffer, "w+b");

	if (pFile) {
		fpRptElementStreamFile = pFile;
		uchUifACRptOnOffMld = RPT_DISPLAY_STREAM;
	}
	else {
		sprintf(pathBuffer, "**ERROR: ItemOpenHistorialReportsFolderHtml() file open - usPGACNo %d  uchMinorClass %d  uchType %d", usPGACNo, uchMinorClass, uchType);
		NHPOS_ASSERT_TEXT(pFile != NULL, pathBuffer);
	}

	return pFile;
}

FILE* ItemOpenHistorialReportsFolder(RptElementOutputFolder tFolder, USHORT usPGACNo, UCHAR uchMinorClass, UCHAR uchType, SHORT  iYear, SHORT  iMonth, SHORT  iDay)
{
	CONST SYSCONFIG* pSysConfig = PifSysConfig();       /* get system config */
	FILE* fpFile = NULL;
	TCHAR* reportTypes[] = {
		L"",             // 0 - unused slot
		L"csv",          // 1 - Comma Separated Values output file
		L"html",         // 2 - Hypertext web browser output file
		L"xml",          // 3 - XML markup language output file
		L"report",       // 4 - report text output file
		L"hubwork"       // 5 - Hubworks ecosystem output file to work with Hubworks.com
	};
	char   pathBuffer[512] = { 0 };

	if (uchUifACRptOnOffMld == RPT_DISPLAY_ON) return NULL;   // display to window so not a printed report

	switch (tFolder) {
	case RPTREGFIN_FOLDER_WEB:
		for (int iLoop = 0; STD_FOLDER_WEBFOLDER[iLoop]; iLoop++) {
			pathBuffer[iLoop] = STD_FOLDER_WEBFOLDER[iLoop];
		}
		fpFile = ItemOpenHistorialReportsFolderHtml(pathBuffer, usPGACNo, uchMinorClass, uchType, iYear, iMonth, iDay);
		if (fpFile) {
			RptDescriptionSet(RptDescriptionCreate(usPGACNo, uchMinorClass, uchType, fpFile, tFolder, RPTREGFIN_OUTPUT_HTML, RptElementStream));
			uchUifACRptOnOffMld = RPT_DISPLAY_STREAM;
		}
		return fpFile;
		break;
	case RPTREGFIN_FOLDER_QUERY:
		for (int iLoop = 0; STD_FOLDER_QUERYFOLDER[iLoop]; iLoop++) {
			pathBuffer[iLoop] = STD_FOLDER_QUERYFOLDER[iLoop];
		}
		fpFile = ItemOpenHistorialReportsFolderHtml(pathBuffer, usPGACNo, uchMinorClass, uchType, iYear, iMonth, iDay);
		if (fpFile) {
			RptDescriptionSet(RptDescriptionCreate(usPGACNo, uchMinorClass, uchType, fpFile, tFolder, RPTREGFIN_OUTPUT_HTML, RptElementStream));
			uchUifACRptOnOffMld = RPT_DISPLAY_STREAM;
		}
		return fpFile;
		break;
	case RPTREGFIN_FOLDER_PRINT:
//		for (int iLoop = 0; STD_FOLDER_PRINTFOLDER[iLoop]; iLoop++) {
//			pathBuffer[iLoop] = STD_FOLDER_PRINTFOLDER[iLoop];
//		}
//		break;
	case RPTREGFIN_FOLDER_TEMP:
	case RPTREGFIN_FOLDER_ANY:
	case RPTREGFIN_FOLDER_HIST:
	default:
		for (int iLoop = 0; iLoop < TCHARSIZEOF(pSysConfig->tcsReportsHistoricalFolder) && pSysConfig->tcsReportsHistoricalFolder[iLoop]; iLoop++) {
			pathBuffer[iLoop] = pSysConfig->tcsReportsHistoricalFolder[iLoop];
		}

		for (int i = 0; i < sizeof(reportTypes)/sizeof(reportTypes[0]); i++) {
			if (_tcsicmp(pSysConfig->tcsReportsHistoricalType, reportTypes[i]) == 0) {
				switch (i) {
				case 1:      // cvs type of output
					fpFile = ItemOpenHistorialReportsFolderCsv(pathBuffer, usPGACNo, uchMinorClass, uchType, iYear, iMonth, iDay);
					if (fpFile) {
						RptDescriptionSet(RptDescriptionCreate(usPGACNo, uchMinorClass, uchType, fpFile, tFolder, RPTREGFIN_OUTPUT_CSV, RptElementStream));
						uchUifACRptOnOffMld = RPT_DISPLAY_STREAM;
					}
					return fpFile;
					break;
				case 2:      // html web page type of output
					fpFile = ItemOpenHistorialReportsFolderHtml(pathBuffer, usPGACNo, uchMinorClass, uchType,  iYear, iMonth, iDay);
					if (fpFile) {
						RptDescriptionSet(RptDescriptionCreate(usPGACNo, uchMinorClass, uchType, fpFile, tFolder, RPTREGFIN_OUTPUT_HTML, RptElementStream));
						uchUifACRptOnOffMld = RPT_DISPLAY_STREAM;
					}
					return fpFile;
					break;
				case 3:      // xml report type of output
					fpFile = ItemOpenHistorialReportsFolderXml(pathBuffer, usPGACNo, uchMinorClass, uchType, iYear, iMonth, iDay);
					if (fpFile) {
						RptDescriptionSet(RptDescriptionCreate(usPGACNo, uchMinorClass, uchType, fpFile, tFolder, RPTREGFIN_OUTPUT_XML, RptElementStream));
						uchUifACRptOnOffMld = RPT_DISPLAY_STREAM;
					}
					return fpFile;
					break;
				case 4:      // text report type of output
					break;
				case 5:      // Hubworks data file type of output
					fpFile = ItemOpenHistorialReportsFolderHubworks(pathBuffer, usPGACNo, uchMinorClass, uchType, iYear, iMonth, iDay);
					if (fpFile) {
						RptDescriptionSet(RptDescriptionCreate(usPGACNo, uchMinorClass, uchType, fpFile, tFolder, RPTREGFIN_OUTPUT_HUBWORKS, RptElementStream));
						uchUifACRptOnOffMld = RPT_DISPLAY_STREAM;
					}
					return fpFile;
					break;
				default:
					break;
				}
				break;
			}
		}
		break;
	}

	return fpFile;
}

SHORT ItemCloseHistorialReportsFolder (FILE *fpFile)
{
	if (!fpFile) return -1;

	if (fpRptElementStreamFile) {
		switch (RptDescriptionGetType()) {
		case RPTREGFIN_OUTPUT_HTML:
			fprintf(fpRptElementStreamFile, "</body>\n</html>\n");
			break;
		case RPTREGFIN_OUTPUT_HUBWORKS:
			fprintf(fpRptElementStreamFile, "</DailyKeys>\r\n</HubWorks>\r\n");
			break;
		case RPTREGFIN_OUTPUT_XML:
//			for (iLoop = 0; iLoop < sizeof(ReportNameList) / sizeof(ReportNameList[0]); iLoop++) {
//				if (ReportNameList[iLoop].usPGACNo == usPGACNo || ReportNameList[iLoop].usPGACNo == 0) {
//					break;
//				}
//			}

			fprintf(fpRptElementStreamFile, "</ac23report>\r\n");
			break;
		default:
			break;
		}
		fflush(fpRptElementStreamFile);
	}

	if (fpFile) {
		fclose (fpFile);
		fpRptElementStreamFile = NULL;
	}
	
	if (fpRptElementStreamFile) {
		fclose (fpRptElementStreamFile);
		fpRptElementStreamFile = NULL;
	}
	uchUifACRptOnOffMld = RPT_DISPLAY_OFF;
	RptDescriptionClear();

	return SUCCESS;
}
//---------------------------------------------------------------------------

/*
*=============================================================================
**  Synopsis: VOID RptFeed(SHORT DefaltFeed)
 
*               
*       Input:  SHORT DefaltFeed 
*      Output:  nothing
*
**  Return: nothing 
*
**  Description:  
*        Feed the line(s) on recipt.
*        There are two version of this function as a second version was created for
*        Amtrak and the Zebra printer.  The problem is that the Zebra printer is a label
*        printer so we have a workaround provided by Zebra to allow us to use the
*        Zebra printer as a receipt printer.  We use the PrintNormal() function of
*        the OPOS control to cause the Zebra printer to flush and print any buffered
*        lines.
*        
*===============================================================================
*/

VOID RptFeedPrintNormal(SHORT DefaltFeed, USHORT *pusLineCount)
{
    ITEMOTHER JRFeed;
	USHORT    usLineCountTemp = 0;

    if (uchRptMldAbortStatus) return;                       /* aborted by MLD */

	if (pusLineCount == NULL) pusLineCount = &usLineCountTemp;

    if (!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT1)) || !(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT3))) {
        JRFeed.uchMajorClass = CLASS_ITEMOTHER;             /* Set Major for print */
        JRFeed.uchMinorClass = CLASS_INSIDEFEED;            /* Set Minor for print */
        JRFeed.lAmount = (LONG)(DefaltFeed);                /* Set Amt for print   */
        JRFeed.fsPrintStatus = PRT_RECEIPT & usRptPrintStatus; /* Set R print status  */
                                                            
        if (RptCheckReportOnMld()) {
            uchRptMldAbortStatus = (UCHAR)MldDispItem(&JRFeed, DefaltFeed); /* display on LCD          */
            JRFeed.fsPrintStatus &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
        }
        PrtPrintItem(NULL,&JRFeed);                         /* print out           */
		(*pusLineCount) += DefaltFeed;
    }
	if (*pusLineCount > 30) {
		TCHAR  tchBuff[8];
		tchBuff[0] = 0;   // empty line for printImmediate()
		PrtPrintLineImmediate(PRT_RECEIPT, tchBuff);
		*pusLineCount = 0;
	}
}
VOID RptFeed(SHORT DefaltFeed)
{
	RptFeedPrintNormal(DefaltFeed, 0);
}

/*
*=============================================================================
**  Synopsis: SHORT RptNoAssume(UCHAR uchNo)
*               
*       Input:  UCHAR uchNo,
*                
*      Output:  nothing
*
**  Return:  1 -- success
*            0 -- fail 
*
**  Description:  
*            If Total Key Type Assignment(Prog.60) is No-Assume Tender(Finalize Total), 
*            then return value is 1.  
*            If other condition, return value is 0.
*===============================================================================
*/

SHORT RptNoAssume(UCHAR  uchNo)
{
    PARATTLKEYTYP   KeyTyp;

    KeyTyp.uchMajorClass = CLASS_PARATTLKEYTYP;                           
    KeyTyp.uchAddress = (UCHAR)(TLTY_NO3_ADR + uchNo);
    CliParaRead(&KeyTyp);

    if ((((KeyTyp.uchTypeIdx / CHECK_TOTAL_TYPE) == PRT_FINALIZE1) ||
		((KeyTyp.uchTypeIdx / CHECK_TOTAL_TYPE) == PRT_FINALIZE2)) &&   /* if Finalize Type ? */
        (KeyTyp.uchTypeIdx % CHECK_TOTAL_TYPE == 0))
	{
        return(1);
    } else {
        return(0);
    }
}

/*
*=============================================================================
**  Synopsis: VOID RptSvscalc(UCHAR uchNo,
*                             TOTAL *pTtlData,
*                             TOTAL *Totalcount)
*               
*       Input:  UCHAR uchNo,
*               TOTAL *pTtlData,
*               TOTAL *Totalcount
*
*      Output:  nothing
*
**  Return: nothing 
*
**  Description:   if the total key has been provisioned as a Service Total then
*                  determine which of the three Service Total Itemizers we are to
*                  use with this Total.
*
*                  We have a couple of legacy isssues to deal with.  We now have more
*                  Total Keys than originally conceived and we also have some Total Keys
*                  that have hard coded types.  So we have to do some calculations to
*                  present to the user of this function as if the differences between
*                  the Total Key type array and the Total Key Control or status do not exist.
*
*                  Total Key #1, #2, and #9 have hard coded types and can not be a Service Total.
*                  The value of uchNo takes into account Total Key #1 and #2 however Total Key #9
*                  is included in the total key provisioning data look up done by RptSvscalc() so
*                  we need to take that into account.  See function ParaTtlKeyTypRead() for comments.
*
*                  Store 3-Type Service Totals in Totalcount[3].
*      
*===============================================================================
*/
VOID RptSvscalc(UCHAR uchNo, TOTAL *pTtlData, TOTAL *Totalcount )
{
	PARATTLKEYTYP   KeyTyp = {0};

	// uchNo is zero based offset from Total #3 (uchNo value of zero means Total #3).
	// uchNo value    0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16
	// total key      3   4   5   6   7   8  10  11  12  13  14  15  16  17  18  19  20
	// uchNo is used to address Total Key #3 through Total Key #8 and then Total Key #10
	// through Total Key #20. Total Key #9 is skipped.

	// See comments with function ParaTtlKeyTypRead() that describes how total key number
	// maps to address for offset into ParaTtlKeyTyp[].
    KeyTyp.uchMajorClass = CLASS_PARATTLKEYTYP;                           
    KeyTyp.uchAddress = (UCHAR)(TLTY_NO3_ADR + uchNo);
    CliParaRead(&KeyTyp);
	// same as ItemTotal.auchTotalStatus[0] which contains total type. Same as ITM_TTL_GET_TTLTYPE().
    if (((KeyTyp.uchTypeIdx / CHECK_TOTAL_TYPE) == PRT_SERVICE1) ||
		((KeyTyp.uchTypeIdx / CHECK_TOTAL_TYPE) == PRT_SERVICE2)) {  /* if Service Type ? */
		PARATTLKEYCTL   KeyCtl = {0}; 

		if (uchNo >= 6) {
			// Total #9 is a special Total Key which has a dedicated type (Total #1 and Total #2 are
			// two other totals which also have a dedicated type which is why we start with Total #3).
			// Total #9 can not have a service total however it does have other control data so is
			// in Para.ParaTtlKeyCtl. If the uchNo >= 6 (Total #10 thru Total #20), increment by
			// one to get proper address since the uchNo is a zero based offset from Total #3 total
			// key provisioning.
			++uchNo; 
		}

        KeyCtl.uchMajorClass = CLASS_PARATTLKEYCTL;             /* Set Major for MDC Data   */
		KeyCtl.uchAddress = (UCHAR)(TLCT_NO3_ADR + uchNo);      /* Set Address for MDC Data */
		KeyCtl.uchField = 7;                                    /* Set Field for MDC Data   */
        CliParaRead(&KeyCtl);                                   /* Call function for Data   */

		// same as ItemTotal.auchTotalStatus[4] which contains TTL_STAT_FLAG_ITEMIZER_SERVICE3()
		// or ITM_TTL_FLAG_ITEMIZER_SERVICE3().
        if ((KeyCtl.uchTtlKeyMDCData & 0x0006) >= 4) {          /* if Service itemizer #3 ? */
            Totalcount[2].lAmount += pTtlData->lAmount;         /* Save Amt in buffer       */
            Totalcount[2].sCounter += pTtlData->sCounter;       /* Save Co in buffer        */ 
        } else if ((KeyCtl.uchTtlKeyMDCData & 0x0006) == 2) {   /* if Service itemizer #2 ? */
            Totalcount[1].lAmount += pTtlData->lAmount;         /* Save Amt in buffer       */
            Totalcount[1].sCounter += pTtlData->sCounter;       /* Save Co in buffer        */
        } else if ((KeyCtl.uchTtlKeyMDCData & 0x0006) == 0) {   /* if Service itemizer #1 ? */
            Totalcount[0].lAmount += pTtlData->lAmount;         /* Save Amt in buffer       */
            Totalcount[0].sCounter += pTtlData->sCounter;       /* Save Co in buffer        */
        }  
    } 
}
 
/*
*=============================================================================
**  Synopsis: UCHAR RptFCMDCChk(USHORT usMDC_Addr)
*               
*       Input:  UCHAR  uchMDC_Addr       : Type of Foreign currency Address 
*      Output:  nothing
*
**  Return:   UCHAR   uchMinor           : Type of Foreign currency Minor Class 
*
**  Description:  
*      Check BIT 1 AND 2 of MDC #79 and #80.                        Saratoga
*      uchMinor is used by function RptElement() to determine the output format.
*===============================================================================
*/
UCHAR   RptFCMDCChk(USHORT usMDC_Addr)
{
    UCHAR       uchMinor = CLASS_RPTREGFIN_PRTCNT;
    PARAMDC     MDC;

    MDC.uchMajorClass   = CLASS_PARAMDC;
    MDC.usAddress       = usMDC_Addr;
    CliParaRead(&MDC);

    switch (usMDC_Addr) {
    case    MDC_FC1_ADR:
    case    MDC_FC3_ADR:
    case    MDC_FC5_ADR:
    case    MDC_FC7_ADR:
        if (!(MDC.uchMDCData & ODD_MDC_BIT1)) {
            uchMinor = CLASS_RPTREGFIN_PRTFCP2;             /* 0.00 */
        } else if (!(MDC.uchMDCData & ODD_MDC_BIT2)) {
            uchMinor = CLASS_RPTREGFIN_PRTFCP3;             /* 0.000 */
        } else {
            uchMinor = CLASS_RPTREGFIN_PRTFCP0;             /* 0 */ 
        }
        break;

    case MDC_FC2_ADR:
    case MDC_FC4_ADR:
    case MDC_FC6_ADR:
    case MDC_FC8_ADR:
        if (!(MDC.uchMDCData & EVEN_MDC_BIT1)) {
            uchMinor = CLASS_RPTREGFIN_PRTFCP2;             /* 0.00 */
        } else if (!(MDC.uchMDCData & EVEN_MDC_BIT2)) {
            uchMinor = CLASS_RPTREGFIN_PRTFCP3;             /* 0.000 */
        } else {
            uchMinor = CLASS_RPTREGFIN_PRTFCP0;             /* 0 */
        }
        break;

    default:
        break;
    }

    return(uchMinor);
}

/*
*=============================================================================
**  Synopsis: SHORT RptPrtError( SHORT sError ) 
*               
*       Input:  usError         : Error Code    
*      Output:  Nothing
*
**  Return: SUCCESS             : Successful 
*
**  Description: Print Error Code for Report.  
*===============================================================================
*/
#if defined(RptPrtError)
SHORT RptPrtError_Special( SHORT sError );
SHORT RptPrtError_Debug( SHORT sError, char *aszFilePath, int nLineNo )
{
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

	sprintf (xBuffer, "RptPrtError_Debug(): sError = %d File %s, lineno = %d", sError, aszFilePath + iLen, nLineNo);
	NHPOS_ASSERT_TEXT(0, xBuffer);

	return RptPrtError_Special (sError);
}

SHORT RptPrtError_Special( SHORT sError )
#else
SHORT RptPrtError( SHORT sError )
#endif
{
    MAINTERRORCODE  MaintErrCode = {0};
	ITEMOTHER       JRFeed = {0};

	PifLog(MODULE_RPT_ID, LOG_EVENT_REPORT_RPTPRTERROR);
	PifLog(MODULE_ERROR_NO(MODULE_RPT_ID), (USHORT)abs(sError));

    /* Print error Code */
    MaintErrCode.uchMajorClass = CLASS_MAINTERRORCODE;
    MaintErrCode.sErrorCode = sError;
    MaintErrCode.usPrtControl = PRT_RECEIPT | PRT_JOURNAL;
    if (RptCheckReportOnMld()) {
        MldDispItem(&MaintErrCode, 0);
        MaintErrCode.usPrtControl &= ~PRT_RECEIPT;
    }
    PrtPrintItem(NULL, &MaintErrCode);
    /* Feed Line */

    JRFeed.uchMajorClass = CLASS_ITEMOTHER;                     /* Set Major for print */
    JRFeed.uchMinorClass = CLASS_INSIDEFEED;                    /* Set Minor for print */ 
    JRFeed.lAmount = (LONG)(RPT_DEFALTFEED);                    /* Set Amt for print   */
    JRFeed.fsPrintStatus = PRT_RECEIPT;                   /* Set R print status  */
    if (RptCheckReportOnMld()) {
        MldDispItem(&JRFeed, RPT_DEFALTFEED);                       /* display out         */
        return(sError);
    }
    PrtPrintItem(NULL,&JRFeed);                                 /* print out           */
    return(sError);
}

/*
*=============================================================================
**  Synopsis: LONG RptRound( LONG lAmount, SHORT sDivisor ) 
*               
*       Input:  lAmount     :   dividend
*               sDivisor    :   divisor
*
**  Return: Rounded result of division or zero if divisor is zero.
*
**  Description: Divide lAmount by sDivisor and round the result.
*
*===============================================================================
*/
DCURRENCY RptRound( DCURRENCY lAmount, DSCOUNTER sDivisor )
{
	D13DIGITS   Quot = 0;
	if (sDivisor) {                      /* Not Divide 0 Case */
		D13DIGITS   Quot1;

        if ( lAmount < 0 ) {
            Quot1 = -5;
        } else {
            Quot1 = 5;
        }
        Quot = lAmount;
		Quot *= 10;        // multiply by 10 once we have more digits available.
        Quot /= sDivisor;
        Quot += Quot1;
        Quot /= 10;
    }

    return Quot;
}

/*
*=============================================================================
**  Synopsis: BOOL RptEODChkFileExist( UCHAR uchAddress ) 
*               
*       Input:  uchAddress      : Address of Flex. Memory Assignment   
*      Output:  Nothing
*
**  Return: RPT_FILE_EXIST      : File Exist
*           RPT_FILE_NOTEXIST   : File Not Exist
*
**  Description: Check File Exist or Not.  
*===============================================================================
*/

BOOL RptEODChkFileExist( UCHAR uchAddress ) 
{
    /* Check Record Size */
    if (RflGetMaxRecordNumberByType(uchAddress) == 0) {          /* Saratoga */
        return(RPT_FILE_NOTEXIST);
    }

    return(RPT_FILE_EXIST);
}

/*
*=============================================================================
**  Synopsis: BOOL RptPTDChkFileExist( UCHAR uchAddress ) 
*               
*       Input:  uchAddress      : Address of Flex. Memory Assignment   
*      Output:  Nothing
*
**  Return: RPT_FILE_EXIST      : File Exist
*           RPT_FILE_NOTEXIST   : File Not Exist
*
**  Description: Check File Exist or Not.  
*===============================================================================
*/

BOOL RptPTDChkFileExist( UCHAR uchAddress ) 
{
	PARAFLEXMEM     FlexMem = { 0 };

    /* Read Record Size */
    FlexMem.uchMajorClass = CLASS_PARAFLEXMEM;
    FlexMem.uchAddress = uchAddress;
    CliParaRead(&FlexMem);

    /* Check Service Time File (Only Store/Recall System) */
    if (uchAddress == FLEX_GC_ADR) {
        if (FlexMem.uchPTDFlag == FLEX_STORE_RECALL) {
            return(RPT_FILE_EXIST);
        }
        return(RPT_FILE_NOTEXIST);
    }

    /* Check Record Size,   Saratoga */
    if ((FlexMem.ulRecordNumber == 0) || (FlexMem.uchPTDFlag == FLEX_PRECHK_BUFFER)) {
        return(RPT_FILE_NOTEXIST);
    }

    return(RPT_FILE_EXIST);
}

/*
*=============================================================================
**  Synopsis: UCHAR RptHourGetBlock(VOID) 
*               
*       Input:  Nothing   
*      Output:  Nothing
*
**  Return: No. of Block 
*
**  Description: Get No. of Block.  
*===============================================================================
*/

UCHAR RptHourGetBlock(VOID) 
{
    PARAHOURLYTIME  ParaHourly;          

    ParaHourly.uchMajorClass = CLASS_PARAHOURLYTIME;
    ParaHourly.uchAddress = HOUR_ACTBLK_ADR;
    CliParaRead(&ParaHourly);

    return(ParaHourly.uchBlockNo);
}

/*
*=============================================================================
**  Synopsis: SHORT RptPauseCheck(VOID) 
*               
*       Input:  Nothing   
*      Output:  Nothing
*
**  Return: SUCCESS
*           RPT_ABORT 
*
**  Description: Report Pause Proccess.  
*===============================================================================
*/

SHORT RptPauseCheck(VOID) 
{
    SHORT   asRet;

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return (RPT_ABORTED);
    }

    if (RptCheckReportOnMld() != RPT_DISPLAY_ON) {
        return(SUCCESS);
    }
    if(UieReadClearKey() != UIE_ENABLE){
        return(SUCCESS);
    }

    asRet = UieErrorMsg(LDT_CLRABRT_ADR, UIE_EM_AC|UIE_EM_TONE|UIE_ABORT);

    if (asRet == FSC_CANCEL) {
        asRet = RPT_ABORTED;
    } else {
        asRet = SUCCESS;
    }

    return(asRet);
}

/*
*=============================================================================
**  Synopsis: SHORT RptCheckReportOnMld(VOID) 
*
*       Input:  nothing
*       Output: nothing
*
**  Return: TRUE    -Display on LCD
*           FALSE   -Print on Receipt
*               
**  Description: Check Report Type whether Display or Print
*===============================================================================
*/
SHORT RptCheckReportOnMld(VOID)
{
	SHORT  sReportOnMld = RPT_DISPLAY_OFF;  // same as value of FALSE.

    switch (uchUifTrailerOnOff) {
    case AC_GCFL_READ_RPT:          /* GUEST CHECK FILE READ REPORT  */ 
    case AC_CASH_READ_RPT:          /* CASHIER READ REPORT  */  
    case AC_REGFIN_READ_RPT:        /* REGISTER FINANCIAL REPORT  */  
    case AC_HOURLY_READ_RPT:        /* HOURLY ACTIVITY REPORT  */  
    case AC_DEPTSALE_READ_RPT:      /* DEPARTMENT SALES READ REPORT  */  
    case AC_PLUSALE_READ_RPT:       /* PLU SALES READ REPORT  */  
    case AC_CPN_READ_RPT:           /* COMBINATION COUPON READ REPORT */
    case AC_WAI_READ_RPT:           /* WAITER FILE READ REPORT  */ 
    case AC_EJ_READ_RPT:            /* E/J READ REPORT  */
    case AC_MAJDEPT_RPT:            /* MAJOR DEPARTMENT SALES READ REPORT  */ 
    case AC_MEDIA_RPT:              /* MEDIA FLASH REPORT  */  
    case AC_SERVICE_READ_RPT:       /* SERVICE TIME READ REPORT */   
    case AC_PROG_READ_RPT:          /* PROGRAMABLE READ REPORT */   
    case AC_ETK_READ_RPT:           /* EMPLOYEE TIME KEEPING FILE READ REPORT */
    case AC_IND_READ_RPT:           /* INDIVIDUAL READ REPORT */ 
	case AC_CONFIG_INFO:			// Configuration Information JHHJ 10-22-04
	case AC_STORE_FORWARD:
	case AC_JOIN_CLUSTER:
	case AC_CSOPN_RPT:              // allow AC176 Open Guest Check Status report to the display.
	//GroupReport - CSMALL
	case AC_PLUGROUP_READ_RPT:
		sReportOnMld = (uchUifACRptOnOffMld == RPT_DISPLAY_ON);  // these reports are reports which may go to display.
    }

    return sReportOnMld;
}

SHORT RptCheckReportOnStream(VOID)
{
	return (uchUifACRptOnOffMld == RPT_DISPLAY_STREAM);  // indicate we are streaming to a file, not to display or printer
}

/*
*=============================================================================
**  Synopsis: VOID RptVatCalc2(D13DIGITS *pd13VatTtl,
*                              D13DIGITS *pd13VatApplTtl,
*                              UCHAR uchTaxNo)
*
*       Input: D13DIGITS   *pd13VatApplTtl
*              UCHAR       uchTaxNo
*
*      Output: D13DIGITS   *pd13VatTtl
*
**  Return: nothing
*
**  Description:   Calculate include tax. V3.3
*
*===============================================================================
*/
VOID  RptVatCalc2(D13DIGITS *pd13VatTtl, D13DIGITS *pd13VatApplTtl, UCHAR uchTaxNo)
{
	PARATAXRATETBL  WorkRate = {0};
    LONG            lRate;
    D13DIGITS       d13Vatable;

    *pd13VatTtl = 0;    // assume no tax because there is no rate

    WorkRate.uchMajorClass = CLASS_PARATAXRATETBL;
    WorkRate.uchAddress = uchTaxNo;
    CliParaRead( &WorkRate );
    if ( WorkRate.ulTaxRate != 0L ) {
        lRate = 1000000L + WorkRate.ulTaxRate;
        RflRateCalc5 (&d13Vatable, pd13VatApplTtl, lRate, RND_TAX1_ADR );
        *pd13VatTtl = *pd13VatApplTtl;
        *pd13VatTtl -= d13Vatable;
    }
}

/*
*===========================================================================
** Format  : VOID   RptFCRateCalc2(LONG *plModAmount, LONG lAmount,
*                                                    ULONG ulRate, UCHAR uchType);
*
*   Input  : LONG   lAmount       -Source Data
*            ULONG  ulRate        -Rate(5 digits under decimal point)
*            UCHAR  uchType       -Rounding Type
*   Output : LONG   *plModAmount  -modifierd amount
*   InOut  : none
*
** Return  : RFL_SUCCESS          -function success
*            RFL_FAIL             -function fail
*
** Synopsis: This function divide amount by rate
*===========================================================================
*/
VOID   RptFCRateCalc2(UCHAR uchAddress, DCURRENCY *plModAmount, DCURRENCY lAmount, ULONG ulRate, UCHAR uchType)
{

    if (CliParaMDCCheck(MDC_EURO_ADR, ODD_MDC_BIT0)) {
		UCHAR           uchMDCData;

        /* Euro enhancement, V2.2 */
		uchMDCData = MaintCurrencyMDCRead(uchAddress);
        if ((uchMDCData & ODD_MDC_BIT1) && (uchAddress != CNV_NO1_ADR)) {
            /* convert FC to FC through Euro */
            ItemTendFCRateCalcByEuroB( plModAmount, lAmount, ulRate, uchType, FC_EURO_TYPE2, uchMDCData);
        } else {
            ItemTendFCRateCalc2(uchMDCData, plModAmount, lAmount, ulRate, uchType );
        }
    } else {
        RflRateCalc2( plModAmount, lAmount, ulRate, uchType );
    }
}


/*
*===========================================================================
** Format  : N_DATE *TtlGetNdate (N_DATE *pnDate);
*
*   Input  : N_DATE *pnDate          -Special Mnemonics address
*   Output : none
*   InOut  : none
** Return  : N_DATE value
*
** Synopsis: This function gets the current date and time and creates an N_DATE
*            value from the data. N_DATE is used with many of the totals that
*            stored in order to provide a start time and an end time for the
*            time period the total covers.
*
*            This function is used in a number of places where the totals for
*            the current day or the current period to date are part of a report
*            so we need the current date/time for the end date/time when displaying
*            or printing the report. Saved totals will have an end date of when
*            the total was reset. Current totals will not yet have an end date since
*            they have not yet been reset.
*===========================================================================
*/
VOID TtlGetNdate (N_DATE *pnDate)
{
    DATE_TIME  DateTime;

    PifGetDateTime(&DateTime);                   /* Get Current Date/Time */
    pnDate->usMin = DateTime.usMin;              /* Set Period To Date    */
    pnDate->usHour = DateTime.usHour;
    pnDate->usMDay = DateTime.usMDay;
    pnDate->usMonth = DateTime.usMonth;
#if defined(DCURRENCY_LONGLONG)
    pnDate->usYear = DateTime.usYear;           // with 64 bit totals we are adding year to the date stamp
#endif
}

/****** END OF SOURCE ******/