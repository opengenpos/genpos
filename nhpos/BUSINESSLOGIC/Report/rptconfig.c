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
* Category    : Report For Configuration, NCR 2170 US Hospitality Application         
* Program Name: RPTCOM.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*        RptTerminalConfiguration(VOID) - print/display the configuration report
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
*   10/31/04   1         J. Hall       Initial version for Rel 2.0.0
*   11/16/04   2         J. Hall       Removed printer sequence now done elsewhere
*   01/04/05   3         J. Hall       Removed use of uchRptMldAbortStatus for print
*   03/22/05   4         E. Smith      Fixed number of characters bug
*   06/29/05   5         R. Chambers   Updated/consolidated source code
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


#include	<tchar.h>
#include <string.h>
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
#include <pmg.h>
#include <report.h>
#include <mld.h>
#include <mldsup.h>
#include <item.h>
#include <plu.h>
#include "rptcom.h"
#include "csstbfcc.h"

#include "BlFWif.h"
#include "rptlocal.h"
#define NUMUNLOCKADDR 3
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))


CONST TCHAR FARCONST aszPrtFlexMem[] = _T("%.10s %d:\t%4.4d\n");      /* time */
CONST TCHAR FARCONST aszPrtFlexMemAdr[] = _T("P2 ADR");
//USHORT  PmgPrint( USHORT, TCHAR *, USHORT);

/*
*=============================================================================
**  Synopsis: USHORT RptDisplayConfigurationLine(TCHAR *tsLineData) 
*
*       Input:  TCHAR *tsLineData  string of characters to print or display
*
*       Output: printed or displayed line of characters
*
**  Return:  1 if completed report, not aborted or canceled
*            0 if report was interrupted (user aborted or canceled)
*
*               
**  Description: This is a local, static function to print or display a
*                line of text to either the receipt printer or the receipt
*                display similar to the standard report options available.
*                The purpose of this function is to encapsulate the logic
*                for printing/displaying in order to hide the details of
*                the necessary code to handle both printer and display.
*===============================================================================
*/
static USHORT RptDisplayConfigurationLine (TCHAR *tsLineData)
{
	USHORT      usRetStatus = 1, usStringLen = 1;
	USHORT		usSizeOfBuffer, usBytesWritten = 0;  // assume no abort
	RPTEJ		EJEdit;

	memset(&EJEdit, 0, sizeof(EJEdit));

	usSizeOfBuffer = sizeof(EJEdit.aszLineData)/sizeof(EJEdit.aszLineData[0]);

	// we are using the RPTEJ structure to send information to be printed,
	// sort of a piggy backing method so we don't need to make a new structure.
	EJEdit.uchMajorClass = CLASS_RPTCONFIGURATION;
	EJEdit.usPrintControl = PRT_JOURNAL;
	EJEdit.uchMinorClass = CLASS_RPTEJ_LINE;


	while(usRetStatus && (usStringLen > usBytesWritten))
	{
		usStringLen = tcharlen(tsLineData);

		// copy the information over to the line data to be sent to either
		// print or display.  Then copy line of data into the buffer.
		_tcsncpy(EJEdit.aszLineData, tsLineData, usSizeOfBuffer);
		

		//if the string length is less than the size of the buffer
		//we just put a null terminated at the end, just to make sure
		if(usStringLen < usSizeOfBuffer)
		{
			EJEdit.aszLineData[usSizeOfBuffer-1] = 0x00;
			usBytesWritten += usStringLen;
		}else
		//if the character data is longer than the data
		//we make a newline and a null character at the end of 
		//the buffer
		{
			EJEdit.aszLineData[usSizeOfBuffer-2] = 0x000A;
			EJEdit.aszLineData[usSizeOfBuffer-1] = 0x00;
			tsLineData += (usSizeOfBuffer-2);
			usBytesWritten += (usSizeOfBuffer-1);
		}


		if (RptCheckReportOnMld()) {
			// user has pressed Num/Type key before AC
			// display report to terminal screen
			uchRptMldAbortStatus = (UCHAR)MldDispItem(&EJEdit, 0);
			usRetStatus = (uchRptMldAbortStatus) ? 0 : 1;
		}
		else {
			// standard printed report, print to receipt printers
			// we need to assign the major and minor classes something different
			// when we are printing on a receipt
			EJEdit.uchMajorClass = CLASS_RPTEJ;
			EJEdit.uchMinorClass = CLASS_RPTEJ_CONFIGURATION;
			EJEdit.usPrintControl = (PRT_JOURNAL | PRT_RECEIPT);
			PrtPrintItem(NULL, &EJEdit);
		}
	}


	return usRetStatus;
}


SHORT RptTerminalConnectStatus(USHORT (*RptSampleConfigurationLine) (TCHAR *tsLineData))
{
	PARAFLEXMEM      MemFlex[MAX_FLXMEM_SIZE];
	CONST SYSCONFIG  *pSysConfig;
	UCHAR            ucTerminalPosition;
	TCHAR            infobuffer[1024];
	TCHAR            uchMaintHostName[PIF_LEN_HOST_NAME + 1] = { 0 };

	if (RptSampleConfigurationLine == NULL)
	{
		RptSampleConfigurationLine = RptDisplayConfigurationLine;
	}

	// clean all of the buffers and structures that we are using
	memset(&infobuffer, 0x00, sizeof(infobuffer));
	memset(&MemFlex, 0x00, sizeof(MemFlex));

	//BEGIN   Format the Terminal name so that it can be shown
	PifGetLocalHostName(uchMaintHostName); // we need this for Terminals Name
	_tcscpy(infobuffer, _T("Terminal Name:\t"));
	_tcscat(infobuffer, uchMaintHostName);
	_tcscat(infobuffer, _T("\n"));
	
	if (0 == RptSampleConfigurationLine (infobuffer)) {
		return 0;
	}
	//END 

	if (CstIamDisconnectedSatellite() == STUB_SUCCESS) {
		_tcscpy(infobuffer, _T("Disconnected Satellite:\tStatus: "));
		if (CstIamDisconnectedUnjoinedSatellite() == STUB_SUCCESS) {
			_tcscat(infobuffer, _T("Unjoined\n"));
		}
		else {
			_tcscat(infobuffer, _T("Joined\n"));
		}
	}
	else {
		_tcscpy(infobuffer, _T("Standard Terminal\n"));
	}

	if (0 == RptSampleConfigurationLine (infobuffer)) {
		return 0;
	}

	// Get the system configuration so that we can determine
	// this terminal's IP address and its terminal position.
	pSysConfig = PifSysConfig();
	ucTerminalPosition = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPosition);
	if (!(ucTerminalPosition >= 1 && ucTerminalPosition <= 16)) {
		ucTerminalPosition = 1;
	}
				
	//BEGIN Format the Terminal IP so that it can be shown	 
	_stprintf(infobuffer, _T("%.15s\t%3.3d.%3.3d.%3.3d.%3.3d\n"),
				_T("Terminal IP:\t"), pSysConfig->auchHaddr[0],
				pSysConfig->auchHaddr[1],pSysConfig->auchHaddr[2],
				pSysConfig->auchHaddr[3] );

	if (0 == RptSampleConfigurationLine (infobuffer)) {
		return 0;
	}
	return 1;
}

/*
*=============================================================================
**  Synopsis: SHORT RptTerminalConfiguration(USHORT (*RptSampleConfigurationLine) (TCHAR *tsLineData)) 
*
*       Input:  pointer to function to print out the report line
*       Output: nothing
*
**  Return:  1 if completed report
*            0 if report was interrupted (user did a cancel)
*
*               
**  Description: Check Report Type whether Display or Print
*===============================================================================
*/
#define  RPTTERMINALASSRTLOG(txt) RptTerminalConfigurationAssertLog(txt, __FILE__, __LINE__);

static void RptTerminalConfigurationAssertLog (TCHAR *tcsBuff, char *filepath, int nLineNo)
{
	int  i;
	char myBuff[128];
	int  iLen = _tcslen (tcsBuff);

	for (i = 0; i < iLen && i < 126; i++) {
		myBuff[i] = tcsBuff[i];
	}
	myBuff[i] = 0;
	PifLogNoAbort((UCHAR *)"", (UCHAR *)filepath, myBuff, nLineNo);
}

SHORT RptTerminalConfiguration(USHORT (*RptSampleConfigurationLine) (TCHAR *tsLineData))
{
	PARAFLEXMEM      MemFlex[MAX_FLXMEM_SIZE];
	PARAUNLOCKNO     UnlockNo;
	USHORT           i, j;
	CONST SYSCONFIG  *pSysConfig;
	UCHAR            ucTerminalPosition;
	TCHAR            infobuffer[1024];
	int              iCount, iCountEnd;
	USHORT			 usDevice;
	TCHAR            uchMaintHostName[PIF_LEN_HOST_NAME + 1] = { 0 };

	if (RptSampleConfigurationLine == NULL)
	{
		RptSampleConfigurationLine = RptDisplayConfigurationLine;
	}

	// clean all of the buffers and structures that we are using
	memset(&infobuffer, 0x00, sizeof(infobuffer));
	memset(&MemFlex, 0x00, sizeof(MemFlex));

	//BEGIN   Format the Terminal name so that it can be shown
	PifGetLocalHostName(uchMaintHostName); // we need this for Terminals Name
	_tcscpy(infobuffer, _T("Terminal Name:\t"));
	_tcscat(infobuffer, uchMaintHostName);
	_tcscat(infobuffer, _T("\n"));
	
	if (0 == RptSampleConfigurationLine (infobuffer)) {
		return 0;
	}
	//END 

	if (CstIamDisconnectedSatellite() == STUB_SUCCESS) {
		_tcscpy(infobuffer, _T("Disconnected Satellite:\tStatus: "));
		if (CstIamDisconnectedUnjoinedSatellite() == STUB_SUCCESS) {
			_tcscat(infobuffer, _T("Unjoined\n"));
		}
		else {
			_tcscat(infobuffer, _T("Joined\n"));
		}
	}
	else {
		_tcscpy(infobuffer, _T("Standard Terminal\n"));
	}

	if (0 == RptSampleConfigurationLine (infobuffer)) {
		return 0;
	}

	// Get the system configuration so that we can determine
	// this terminal's IP address and its terminal position.
	pSysConfig = PifSysConfig();
	ucTerminalPosition = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);
	if (!(ucTerminalPosition >= 1 && ucTerminalPosition <= 16)) {
		ucTerminalPosition = 1;
	}
				
	//BEGIN Format the Terminal IP so that it can be shown	 
	_stprintf(infobuffer, _T("%.15s\t%3.3d.%3.3d.%3.3d.%3.3d\n"),
				_T("Terminal IP:\t"), pSysConfig->auchHaddr[0],
				pSysConfig->auchHaddr[1],pSysConfig->auchHaddr[2],
				pSysConfig->auchHaddr[3] );

	if (0 == RptSampleConfigurationLine (infobuffer)) {
		return 0;
	}
	//END

	//Get Program 2 Information from Para
	for (j = 0, i = FLEX_DEPT_ADR; i <= FLEX_PPI_ADR && j < MAX_FLXMEM_SIZE; j++, i++)
	{
		MemFlex[j].uchMajorClass = CLASS_PARAFLEXMEM;
		MemFlex[j].uchAddress = (UCHAR) i;
		CliParaRead(&MemFlex[j]);
		
		_stprintf(infobuffer, aszPrtFlexMem, aszPrtFlexMemAdr,(j+1), MemFlex[j].ulRecordNumber);


//The following code has been removed because we MAY use different languages
//for each address, and instead of deleting it, I decided to just leave it in here
//because it may help later on.
#ifdef JGGJG				
			case FLEX_PLU_ADR: 
				_stprintf(tempbuffer,aszPrtFlexMem,
					_T("P2 ADR 2"), MemFlex[j].ulRecordNumber);
				_tcscat(infobuffer, tempbuffer);
				break;
			case FLEX_WT_ADR:
					_stprintf(tempbuffer, aszPrtFlexMem,
					_T("P2 ADR 3"), MemFlex[j].ulRecordNumber);
				_tcscat(infobuffer, tempbuffer);
				break;
			case FLEX_CAS_ADR:
					_stprintf(tempbuffer, aszPrtFlexMem,
					_T("P2 ADR 4"), MemFlex[j].ulRecordNumber);
				_tcscat(infobuffer, tempbuffer);
				break;
			case FLEX_EJ_ADR:
					_stprintf(tempbuffer, aszPrtFlexMem,
					_T("P2 ADR 5"), MemFlex[j].ulRecordNumber);
				_tcscat(infobuffer, tempbuffer);
				break;
			case FLEX_ETK_ADR:
					_stprintf(tempbuffer, aszPrtFlexMem,
					_T("P2 ADR 6"), MemFlex[j].ulRecordNumber);
				_tcscat(infobuffer, tempbuffer);
				break;
			case FLEX_ITEMSTORAGE_ADR:
					_stprintf(tempbuffer, aszPrtFlexMem,
					_T("P2 ADR 7"), MemFlex[j].ulRecordNumber);
				_tcscat(infobuffer, tempbuffer);
				break;
			case FLEX_CONSSTORAGE_ADR: 
					_stprintf(tempbuffer, aszPrtFlexMem,
					_T("P2 ADR 8"), MemFlex[j].ulRecordNumber);
				_tcscat(infobuffer, tempbuffer);
				break;
			case FLEX_GC_ADR: 
					_stprintf(tempbuffer, aszPrtFlexMem,
					_T("P2 ADR 9"), MemFlex[j].ulRecordNumber);
				_tcscat(infobuffer, tempbuffer);
				break;
			case FLEX_CPN_ADR:
					_stprintf(tempbuffer, aszPrtFlexMem,
					_T("P2 ADR 10"), MemFlex[j].ulRecordNumber);
				_tcscat(infobuffer, tempbuffer);
				break;
			case FLEX_CSTRING_ADR: 
					_stprintf(tempbuffer, aszPrtFlexMem,
					_T("P2 ADR 11"), MemFlex[j].ulRecordNumber);
				_tcscat(infobuffer, tempbuffer);
				break;
			case FLEX_PROGRPT_ADR:
					_stprintf(tempbuffer, aszPrtFlexMem,
					_T("P2 ADR 12"), MemFlex[j].ulRecordNumber);
				_tcscat(infobuffer, tempbuffer);
				break;
			case FLEX_PPI_ADR: 
					_stprintf(tempbuffer, aszPrtFlexMem,
					_T("P2 ADR 13"), MemFlex[j].ulRecordNumber);
				_tcscat(infobuffer, tempbuffer);
				break;
			default:
				break;
		}
#endif

		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
	}

#if defined(VBOLOCK)

	// Add functionality (Switch statement) that upon 
	// case 1 -> reads the ' terminal serial number'
	// case 2 -> reads the "software serial"
	// case 3 -> reads the UNLOCK code...
	for(i=1; i<=NUMUNLOCKADDR; i++){
		infobuffer[0] = 0;
		memset(&UnlockNo, 0x00, sizeof(UnlockNo));
		UnlockNo.uchAddress = (UCHAR) i;
		switch(UnlockNo.uchAddress){
		case 1:
			//Read the Terminal's Serial Number from the Registry
			maintUnlockNoRegRead(&UnlockNo);	//retrieve security data
			_tcscat(infobuffer, _T("P95 ADR 1:\t"));
			_tcscat(infobuffer, UnlockNo.aszUnlockNo);
			_tcscat(infobuffer, _T("\n"));
			break;
		case 2:
			//Read the Software Serial
			_tcscat(infobuffer, _T("P95 ADR 2:\t"));
			BlFwIfReadSerial(UnlockNo.aszUnlockNo);
			_tcscat(infobuffer, UnlockNo.aszUnlockNo);
			_tcscat(infobuffer, _T("\n"));
			break;
		case 3:

			_tcscat(infobuffer, _T("P95 ADR 3:\t"));
			_tcscat(infobuffer, _T("N/A VBOLOCK ")/*UnlockNo.aszUnlockNo*/);
			_tcscat(infobuffer, _T("\n"));
			break;
		default:
			break;
		}

		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
	}


#else
	for(i=1; i<=NUMUNLOCKADDR; i++){
		infobuffer[0] = 0;
		memset(&UnlockNo, 0x00, sizeof(UnlockNo));
		UnlockNo.uchAddress = (UCHAR) i;
		switch(UnlockNo.uchAddress){
		case 1:
			//Read the Terminal's Serial Number from the Registry
			maintUnlockNoRegRead(&UnlockNo);	//retrieve security data
			_tcscat(infobuffer, _T("P95 ADR 1:\t"));
			_tcscat(infobuffer, UnlockNo.aszUnlockNo);
			_tcscat(infobuffer, _T("\n"));
			break;
		case 2:
			//Read the Software Serial
			_tcscat(infobuffer, _T("P95 ADR 2:\t"));
			BlFwIfReadSerial(UnlockNo.aszUnlockNo);
			_tcscat(infobuffer, UnlockNo.aszUnlockNo);
			_tcscat(infobuffer, _T("\n"));
			break;
		case 3:
			//Read the Unlock Code from the registry
			//if the Unlock number is Null read from the
			//Parameters
			maintUnlockNoRegRead(&UnlockNo);
			if(UnlockNo.aszUnlockNo[0] == 0x00){
				TermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
				TermInfo.uchAddress = ucTerminalPosition;
				ParaTerminalInfoParaRead (&TermInfo);
				for(j = 0, k = 0; j < 24;){
					//place the first number, which is the HIBYTE of aszUnlockCode
					//then place the second number, which is the LOBYTE of aszUnlockCode
					UnlockNo.aszUnlockNo[j++] = LOBYTE(TermInfo.TerminalInfo.aszUnLockCode[k]);
					UnlockNo.aszUnlockNo[j++] = HIBYTE(TermInfo.TerminalInfo.aszUnLockCode[k++]);
				}
				UnlockNo.aszUnlockNo[j] = 0x00;
			}
			_tcscat(infobuffer, _T("P95 ADR 3:\t"));
			_tcscat(infobuffer, UnlockNo.aszUnlockNo);
			_tcscat(infobuffer, _T("\n"));
			break;
		default:
			break;
		}

		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
	}
#endif

	_tcscpy (infobuffer, _T("Software Version Numbers"));
	if (0 == RptSampleConfigurationLine (infobuffer)) {
		return 0;
	}

	// Get OS Version and information.  This function appends to the
	// provided string so we need to truncate string to zero length first.
	infobuffer[0] = 0;
	GetNHPOSSystemInformation(infobuffer);

	// We stop the loop once we hit a null character
	// because that signifies the end of the buffer.
	// Infobuffer will not be full, however this size
	// may need to change once more DLL's and executables
	// are added to our product.

	iCountEnd = _tcslen(infobuffer);

	//The following loop goes through the buffer and sends one line of information at at time 
	//and sends in the buffer into the Report function.  
	for (iCount = 0; iCount < iCountEnd && infobuffer[iCount]; iCount++)
	{
		int iCountSave = 0;

		//we go character by character so that we just get one line of information
		//at a time, 0x000A is a "\n" in Hex.
		iCountSave = iCount;
		for (iCountSave = iCount;
				(iCount < iCountEnd) && (infobuffer[iCount] != 0x000A) && infobuffer[iCount];
				iCount++) ;

		infobuffer[iCount] = 0;
		if (0 == RptSampleConfigurationLine (&infobuffer[iCountSave])) {
			return 0;
		}
	}

	//we go through the list of OPOS controls and report them.
	//this function pulls the version information from the opos controls
	//and reports them on the display or printer.
	for(usDevice = 1; usDevice <= BLFWIF_NUM_OPOS_CONTROLS; usDevice++)
	{
		memset(infobuffer, 0x00, sizeof(infobuffer));
		BlFwIfDeviceVersion(usDevice, infobuffer);

		if (0 == RptSampleConfigurationLine (infobuffer)) 
		{
				return 0;
		}
	}

	// Now we provide the network tally information for the report.
	//
	// **  WARNING  **  Additions to these tally structs may require a change to a static buffer in
	//                  the function PifNetSystemInfo() that is used for the AC888 report.
	//                  See comments for variable PifNetworkSystemInfoBuffer in PifNetSystemInfo().
	{
		PIF_NETSYSTALLY  NetTally = {0};
		PIF_NETUSERTALLY  UserTally[30] = {0};
		USHORT            usUserCount = 20, usI;

		usUserCount = 20;
		PifNetSystemInfo (&NetTally, UserTally, &usUserCount);

		_tcscpy (infobuffer, _T("AC888 Network Layer Statistics"));
		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
		RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

		_stprintf(infobuffer, _T("  Total Messages Sent %d"), NetTally.ulTotalSent);
		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
		RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

		_stprintf(infobuffer, _T("  Error Messages Sent %d"), NetTally.ulSentError);
		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
		RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

		_stprintf(infobuffer, _T("  Total Messages Recvd %d"), NetTally.ulTotalRecv);
		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
		RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

		_stprintf(infobuffer, _T("  Error Messages Recvd %d"), NetTally.ulRecvError);
		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
		RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

		_stprintf(infobuffer, _T("  Undelivrd Messages Recvd %d"), NetTally.ulRecvUndeliver);
		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
		RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

		_stprintf(infobuffer, _T("  Illegal Messages Recvd %d"), NetTally.ulIllegalMessage);
		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
		RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

		_stprintf(infobuffer, _T("  Received Lost Time Out %d"), NetTally.ulRecvLostTimeOut);
		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
		RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

		_stprintf(infobuffer, _T("  Received Lost Queue Full %d"), NetTally.ulRecvLostQueueFull);
		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
		RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

		_stprintf(infobuffer, _T("  Received Lost Resources %d"), NetTally.ulOutOfResources);
		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
		RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

		_stprintf(infobuffer, _T("  Received Lost Net Closed %d"), NetTally.ulRecvLostNetClosed);
		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
		RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

		_stprintf(infobuffer, _T("  Received Lost Busy Ack Sent %d"), NetTally.ulAckSentStatusBusy);
		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
		RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

		_stprintf(infobuffer, _T(" Net Unreachable Error on Send %d"), NetTally.ulSendUnreachableError);
		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
		RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

		_tcscpy (infobuffer, _T("Per Thread Network Statistics"));
		if (0 == RptSampleConfigurationLine (infobuffer)) {
			return 0;
		}
		RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

		for (usI = 0; usI < usUserCount; usI++) {
			SHORT   sPrintPort;
			TCHAR   *tcsPortNames[] = { 0, _T("SERVER"), _T("ISPSERVER"), _T("CLIENT"), _T("NOTICEBOARD")};

			if (UserTally[usI].usUserPort == 0)
				break;

			sPrintPort = 0;
			sPrintPort = ((UserTally[usI].usUserPort == PIF_PORT_SERVER) ? 1 : sPrintPort);
			sPrintPort = ((UserTally[usI].usUserPort == PIF_PORT_ISPSERVER) ? 2 : sPrintPort);
			sPrintPort = ((UserTally[usI].usUserPort == PIF_PORT_STUB) ? 3 : sPrintPort);
			sPrintPort = ((UserTally[usI].usUserPort == PIF_PORT_NB) ? 4 : sPrintPort);
			if (sPrintPort) {
				_stprintf(infobuffer, _T("  Thread %s port %d"), tcsPortNames[sPrintPort], UserTally[usI].usUserPort);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usExeNetSendCount %d"), UserTally[usI].usExeNetSendCount);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usExeNetReceiveCount %d"), UserTally[usI].usExeNetReceiveCount);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usTotalSent %d"), UserTally[usI].usTotalSent);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usSentError %d"), UserTally[usI].usSentError);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usTotalRecv %d"), UserTally[usI].usTotalRecv);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usRecvError %d"), UserTally[usI].usRecvError);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usRecvLost  %d"), UserTally[usI].usRecvLost);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usPendingHighWater  %d"), UserTally[usI].usPendingHighWater);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usRecvLostTimeOut  %d"), UserTally[usI].usRecvLostTimeOut);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usRecvLostQueueFull  %d"), UserTally[usI].usRecvLostQueueFull);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usRecvRequestTimeOut  %d"), UserTally[usI].usRecvRequestTimeOut);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usSendRequestTimeOut  %d"), UserTally[usI].usSendRequestTimeOut);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usSendTargetBusy  %d"), UserTally[usI].usSendTargetBusy);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usRecvTargetClear  %d"), UserTally[usI].usRecvTargetClear);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usSendTargetClear  %d"), UserTally[usI].usSendTargetClear);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usRecvPutOnQueue  %d"), UserTally[usI].usRecvPutOnQueue);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usRecvTakenOffQueue  %d"), UserTally[usI].usRecvTakenOffQueue);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usRecvGivenNow  %d"), UserTally[usI].usRecvGivenNow);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usRecvBadSeqNoPort  %d"), UserTally[usI].usRecvBadSeqNoPort);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usRecvOutOfResources  %d"), UserTally[usI].usRecvOutOfResources);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usAckSentStatusBusy  %d"), UserTally[usI].usAckSentStatusBusy);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usRecvLostNetClosed  %d"), UserTally[usI].usRecvLostNetClosed);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usRecvRequestWaitStart  %d"), UserTally[usI].usRecvRequestWaitStart);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usRecvMessageAlreadyExist  %d"), UserTally[usI].usRecvMessageAlreadyExist);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usSendMessageAlreadyExist  %d"), UserTally[usI].usSendMessageAlreadyExist);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usTimerCheckSkipped  %d"), UserTally[usI].usTimerCheckSkipped);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

				_stprintf(infobuffer, _T("      usTimerSendWithRecvPending  %d"), UserTally[usI].usTimerSendWithRecvPending);
				if (0 == RptSampleConfigurationLine (infobuffer)) {
					return 0;
				}
				RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888
			}
		}
	}

	PifSnapShotTables ();
	_stprintf(infobuffer, _T("SNAPSHOT003 file generated."));
	if (0 == RptSampleConfigurationLine (infobuffer)) {
		return 0;
	}
	RPTTERMINALASSRTLOG(infobuffer);    // print the network statistics to the ASSRTLOG file as well when doing AC888

//  Removed per memo from Dan Parliman June 28, 2010 relaying request from
//  NCR Weights and Measures to remove this line.  It is hard ware dependent
//  and can change frequently.
//	_stprintf(infobuffer, _T(" Weights and Measures Certificate No: 06-032"));
//	if (0 == RptSampleConfigurationLine (infobuffer)) {
//		return 0;
//	}
	return 1;
}






