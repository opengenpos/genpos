// EEptInterface.cpp : Defines the initialization routines for the DLL.
//
/*-----------------------------------------------------------------------*\
:   Title          : ELECTRONIC PAYMENT MODULE
:   Category       : ELECTRONIC PAYMENT MODULE, NeighborhoodPOS 2.0.0 Application
:   Program Name   : EEptInterface.cpp
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
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
:  Abstract:
:   ItemTendFast()          ;   w/o amount tender
:
:  ---------------------------------------------------------------------
:  Update Histories
:    Date  : Ver.Rev. :   Name      : Description
: 12/6/03  : 02.00.00 : R.Herrington  : NeighborhoodPOS initial using DSI Client
: 05/17/15 : 02.02.01 : R.Chambers    : changed terminal initialization for Datacap Out Of Scope control
: 09/18/15 : 02.02.01 : R.Chambers    : changes to support EMV control based on testing of Datacap SDK and VX-805 CTLS
-------------------------------------------------------------------------*/

#include "stdafx.h"
#include <Winsock2.h>

#include "afxconv.h"
#include "paraequ.h"
#include "pif.h"
#include "ecr.h"
#include "eept.h"
#include "EEPTL.H"
#include "EEptTrans.h"

extern "C" __declspec(dllexport) SHORT XEEptCheckServerPing()
{
	SHORT  sRetStatus = -1;

	return sRetStatus;
}
/*
BOOL XEptOpen(TCHAR* tchRegStr)
will take a string, which is the registry key to look at for settings
the Dialog with the controls is created and if a pin pad is being used
that is setup with the registry values
*/
extern "C" __declspec(dllexport) BOOL XEEptOpen(TCHAR* tchRegStr)
{

	return FALSE;
}

extern "C" __declspec(dllexport) VOID XEEptClose()
{
}

extern "C" __declspec(dllexport) VOID XEEptSetOposValues (TCHAR *pPinPadEncryptedPin, TCHAR *m_PinPadKeySerialNumber, TCHAR *PinPadTrack2)
{
}


extern "C" __declspec(dllexport) USHORT XEEptGetInterfaceType()
{
	return EEPT_DLL_UNAVAILABLE;
}


/*
SHORT XEptBuildXMLAndExecuteTrans(VOID* pXEPTReqData, VOID* pEEptSpool)
pXEPTReqData an EEPTREQDATA Structure
pEEptSpool an EEPTSPOOL structure which contains a EEPTRESDATA structure

  take the information from the ReqData build an XML message for DATACAP
  put the returned information into the response data
*/

extern "C" __declspec(dllexport) SHORT XEEptBuildXMLAndExecuteTrans(EEPTREQDATA * pXEPTReqData, EEPTRSPDATA * pXEPTRespData)
{
	SHORT  sResultStatus = EEPT_RET_FAIL;

	if (XEEptGetInterfaceType() == EEPT_DLL_UNAVAILABLE) {
		// create a response with 1006 general failure error in it.

		pXEPTRespData->auchEptStatus = EEPT_FC2_REJECTED;
		memcpy(pXEPTRespData->auchSrcTerminal, pXEPTReqData->auchSrcTerminal, sizeof(pXEPTRespData->auchSrcTerminal));
		memcpy(pXEPTRespData->auchDesTerminal, pXEPTReqData->auchDesTerminal, sizeof(pXEPTRespData->auchDesTerminal));
		pXEPTRespData->auchTerminalNo = pXEPTReqData->auchTerminalNo;
		_tcscpy(pXEPTRespData->auchErrorText, _T("SYSTEM ERROR: DSI Unavailable."));
		_tcscpy(pXEPTRespData->auchMsgText, _T("SYSTEM ERROR: DSI Unavailable."));
		strncpy((char *)(pXEPTRespData->auchErrorNumber), "001006", 6);  // create an error code of EEPTINTERFACE_DECRYPT_ERROR
		strcpy((char*)pXEPTRespData->auchAuthorize, "0.00");
		return EEPT_RET_BADPROV;
	}

	return sResultStatus;
}

extern "C" __declspec(dllexport) SHORT XEEptReportStoreForward(SHORT sTypeDsi, EEPTF_HEADER * pEEptFileData)
{
	SHORT  sResultStatus = EEPT_RET_FAIL;
	return sResultStatus;
}

extern "C" __declspec(dllexport) ULONG XEEptCheckAndRecoverFile(SHORT sTypeDsi)
{
	ULONG  ulResultStatus = 0;
	return ulResultStatus;
}

extern "C" __declspec(dllexport) LONG XEEptGetNextBlockIterateAll(short sTypeDsi, EEPT_BLOCK_HEADER* blockHeader)
{
	LONG  sResultStatus = EEPT_RET_FAIL;

	return sResultStatus;
}

extern "C" __declspec(dllexport) LONG XEEptGetNextBlockIterate(short sTypeDsi, EEPT_BLOCK_HEADER* blockHeader)
{

	LONG  sResultStatus = EEPT_RET_FAIL;

	return sResultStatus;
}
extern "C" __declspec(dllexport) LONG XEEptGetSpecifiedBlockHeader(short sTypeDsi, ULONG ulBlockOffset, EEPT_BLOCK_HEADER* blockHeader)
{
	LONG  sResultStatus = EEPT_RET_FAIL;

	return sResultStatus;
}

extern "C" __declspec(dllexport) LONG XEEptUpdateSpecifiedBlock(short sTypeDsi, ULONG ulBlockOffset, EEPT_BLOCK_HEADER* blockHeader)
{
	LONG  sResultStatus = EEPT_RET_FAIL;
	return sResultStatus;
}
/*
	Forward stored transactions.
*/
extern "C" __declspec(dllexport) SHORT XEEptForwardStoreForward(USHORT usTenderType, EEPTRSPDATA *resData, ULONG *ulMaskData, ITMSTOREFORWARD* pItmStoreForwardData)
{
	SHORT  sResultStatus = EEPT_END_OF_FILE;

	return sResultStatus;
}

extern "C" __declspec(dllexport) SHORT XEEptForwardStoreForwardDecrypt(TCHAR *AcctNumberEncrypted, TCHAR *AccountNo, TCHAR *ExpireDate)
{
	SHORT  sResultStatus = EEPT_RET_SERVERCOMMS;

	return sResultStatus;
}


extern "C" __declspec(dllexport) LONG XEEptDeleteFailedSF(LONG lUieAc102_SubMenu, EEPT_BLOCK_HEADER* Totals){//Store and forward
	LONG  lResultStatus = 0;

	return lResultStatus;
}


extern "C" __declspec(dllexport) SHORT XEEptGenerateStoredResponseMessage(VOID* vresData, ULONG *pulMaskData, VOID *vreqData, ULONG	ulLastStoredConsecNo)
{

	return 0;
}

extern "C" __declspec(dllexport) SHORT XEEptToggleStoreForward(SHORT onOff)
{
	SHORT  sResultStatus = 0;	

	return sResultStatus;
}

extern "C" __declspec(dllexport) SHORT XEEptGetTenderNo(short sTypeDsi, EEPT_BLOCK_HEADER *blockHeader)
{
	SHORT  sResultStatus = -15;	

	return sResultStatus;
}

extern "C" __declspec(dllexport) SHORT XEEptUnjoinClear()
{
	SHORT  sResultStatus = 0;

	sResultStatus = EEPT_ERROR_SW_PROV;
	return sResultStatus;
}


#if 0
	//-----------------------------------------------------------------------------------------------
	// Following are test structs that show the format of FreedomPay Client
	// message strings.  These are similar to the response message that the
	// FreedomPay Client will send to NHPOS in response to a request string.
	// These strings are being kept as examples as well as for debugging in
	// the case that a FreedomPay Client is not available for testing.
	// Richard Chambers,  Feb-14-2008
	static  UCHAR  FreedomPayPurchaseRxMsg01 [] = {
	// SOH, ID01, ID02, ID03, ID04, ID05, ID06, ID07, ID08, ID09,
	  0x01,  '1',  '0',  '0',  '2',  '0',  '3',  '0',  '4',  '0',
	// IN01, IN02, IN03, IN04, IN05, IN06, IN07, IN08, IN09, IN10, IN11, IN12, IN13, IN14, IN15, IN16
	  'F',  'r',  'e',  'e',  'd',  'o',  'm',  'P',  'a',  'y',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',
	// STX,  FS,  ASN1, ASN2, RTXF,
	  0x02, 0x1C, '0',  '0',  ' ', 
	//RS01, RS02, RS03, RS04, RS05, RS06, RS07, RS08, RS09, RS10, RS11, FS
	  'C',  'H',  'G',  '_',  'P',  'O',  'S',  'T',  'I',  'N',  'G',  0x1C,
	//RR01, RR02, RR03, RR04, RR05, RR06, RR07, RR08,  FS
	  'A',  'P',  'P',  'R',  'O',  'V',  'E',  'D',   0x1C,
	//BA01, BA02, BA03, BA04, BA05,  FS
	  '1',  '0',  '.',  '5',  '5',  0x1C,
	// MS01, MS02, MS03, MS04, MS05, MS06, MS07, MS08, MS09, MS10, MS11, MS12, MS13, MS14, MS15, MS16, MS17, MS18, MS19, MS20, FS
	   'F',  'r',  'e',  'e',  'd',  'o',  'm',  'P',  '0',  '1',  ' ',  'C',  'h',  'a',  'r',  'g',  'e',  'x',  'y',  'z',  0x1C,
	//DS01, DS02, DS03, 
	  'X',  'X',  'X', 
	//ETX,  CHK, EOT 
	  0x03,  0,  0x04 };

	static  UCHAR  FreedomPayPurchaseRxMsg02 [] = {
	// SOH, ID01, ID02, ID03, ID04, ID05, ID06, ID07, ID08, ID09,
	  0x01,  '1',  '0',  '0',  '2',  '0',  '3',  '0',  '4',  '0',
	// IN01, IN02, IN03, IN04, IN05, IN06, IN07, IN08, IN09, IN10, IN11, IN12, IN13, IN14, IN15, IN16
	  'F',  'r',  'e',  'e',  'd',  'o',  'm',  'P',  'a',  'y',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',
	// STX,  FS,  ASN1, ASN2, RTXF,
	  0x02, 0x1C, '0',  '0',  ' ',
	//RS01, RS02, RS03, RS04, RS05, RS06, RS07, RS08, RS09, RS10, RS11, FS
	  'C',  'H',  'G',  '_',  'P',  'O',  'S',  'T',  'I',  'N',  'G',  0x1C,
	//RR01, RR02, RR03, RR04, RR05, RR06, RR07, RR08,  FS
	  'D',  'E',  'C',  'L',  'I',  'N',  'E',  'D',   0x1C,
	//BA01, BA02, BA03, BA04, BA05,  FS
	  '0',  '1',  '.',  '0',  '0',  0x1C,
	// MS01, MS02, MS03, MS04, MS05, MS06, MS07, MS08, MS09, MS10, MS11, MS12, MS13, MS14, MS15, MS16, MS17, MS18, MS19, MS20, FS
	   'F',  'r',  'e',  'e',  'd',  'o',  'm',  'P',  '0',  '2',  ' ',  'C',  'h',  'a',  'r',  'g',  'e',  'N',  'O',  'T',  0x1C,
	//DS01, DS02, DS03, 
	  'X',  'X',  'X', 
	//ETX,  CHK, EOT 
	  0x03,  0,  0x04 };


	static  UCHAR  FreedomPayPurchaseRxMsg03 [] = {
	// SOH, ID01, ID02, ID03, ID04, ID05, ID06, ID07, ID08, ID09,
	  0x01,  '1',  '0',  '0',  '2',  '0',  '3',  '0',  '4',  '0',
	// IN01, IN02, IN03, IN04, IN05, IN06, IN07, IN08, IN09, IN10, IN11, IN12, IN13, IN14, IN15, IN16
	  'F',  'r',  'e',  'e',  'd',  'o',  'm',  'P',  'a',  'y',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',
	// STX,  FS,  ASN1, ASN2, RTXF,
	  0x02, 0x1C, '0',  '0',  ' ',
	//RS01, RS02, RS03, RS04, RS05, RS06, RS07, RS08, RS09, RS10, RS11, FS
	  'C',  'H',  'G',  '_',  'P',  'O',  'S',  'T',  'I',  'N',  'G',  0x1C,
	//RR01, RR02, RR03, RR04, RR05, RR06, RR07,  FS
	  'P',  'A',  'R',  'T',  'I',  'A',  'L',   0x1C,
	//BA01, BA02, BA03, BA04, BA05,  FS
	  '0',  '1',  '.',  '0',  '0',  0x1C,
	// MS01, MS02, MS03, MS04, MS05, MS06, MS07, MS08, MS09, MS10, MS11, MS12, MS13, MS14, MS15, MS16, MS17, MS18, MS19, MS20, FS
	   'F',  'r',  'e',  'e',  'd',  'o',  'm',  'P',  '0',  '2',  ' ',  'C',  'h',  'a',  'r',  'g',  'e',  'P',  'A',  'R',  0x1C,
	//DS01, DS02, DS03, 
	  'X',  'X',  'X', 
	//ETX,  CHK, EOT 
	  0x03,  0,  0x04 };
	//-----------------------------------------------------------------------------------------------------------------
#endif



/*
*==========================================================================
**    Synopsis:   SHORT XEEptBuildFreedomPayAndExecuteTrans (VOID* pXEPTReqData, VOID* pEEptSpool, ULONG *plMaskData)
*
*       Input:    GCNUM usRecordNumber
*                 UCHAR uchType  
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    GCF_SUCCESS
*                 GCF_NOT_CREATE
*                 GCF_ALL_LOCKED
*
**  Description:
*               Create Guest Check File.              
*==========================================================================
*/
extern "C" __declspec(dllexport) SHORT XEEptBuildFreedomPayAndExecuteTrans(VOID* pXEPTReqData, VOID* pEEptSpool)
{
	SHORT       sResultStatus = EEPT_RET_FAIL;

	return sResultStatus;
}


extern "C" __declspec(dllexport) VOID XEEptGetLastPin(TCHAR* tchPinBlock)
{
}

extern "C" __declspec(dllexport) SHORT XEEptResetPinPad()
{
	SHORT  sResultStatus = EEPT_RET_FAIL;
	return sResultStatus;
}

extern "C" __declspec(dllexport) SHORT XEEptCancelRequest()
{
	SHORT  sResultStatus = EEPT_RET_SUCCESS;

	return sResultStatus;
}
