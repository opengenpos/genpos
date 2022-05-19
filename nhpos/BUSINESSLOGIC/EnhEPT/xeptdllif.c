/*
*=============================================================================
* Title       : EPT Interface for DLLs
* Program Name: xeptdllif.c
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*                   EEptDllSend ();
*                   EEptDllRecv ();
*
*-----------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name       : Description
* Apr-16-04 : 00.00.01 : R. Herrington  : Initial
*=============================================================================
*/

/**
;============================================================================
;                  I N C L U D E     F I L E s                              +
;============================================================================
**/
/**------- MS - C ------**/
#include <tchar.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>

/**------- 2170 local------**/
#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "plu.h"
#include "appllog.h"
#include "paraequ.h"
#include "para.h"
#include "pararam.h"
#include "csstbpar.h"
#include "eept.h"
#include "eeptl.h"
#include "prevept.h"
#include "EEptTrans.h"
#include "scf.h"

/**
;=============================================================================
;+              P R O G R A M    D E C L A R A T I O N s                     +
;=============================================================================
**/
UCHAR uchEEPTDLLCurrent = EEPT_DLL_DATACAP;
/*
==============================================================================
+                  S T A T I C   R A M s                                     +
==============================================================================
*/

/**
;=============================================================================
;+              P R O G R A M    D E C L A R A T I O N s                     +
;=============================================================================
**/
/**
;=============================================================================
;+              E X T E R N A L  R A M s
;=============================================================================
**/

/*
*===========================================================================
** Synopsis:    VOID    XEptDllOpen(TCHAR* tchDevice);
*
*     InPut:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return: none
*
** Description: This function opens a network connection to the specified
				Enhanced Electronic Payment (EEPT) host machine.  .
*===========================================================================
*/
VOID    XEptDllOpen(TCHAR* tchDeviceName, TCHAR* tchDllFileName)
{
	XEEptOpen(tchDeviceName);
}

/*
*===========================================================================
** Synopsis:    VOID    XEptDllClose(UCHAR uchEEPTDLL);
*
*     InPut:    uchEEPTDLL - The interface to close
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function closes NETWORK.
*===========================================================================
*/
VOID    XEptDllClose(UCHAR uchEEPTDLL)
{
	XEEptClose();
}

/*
*===========================================================================
** Synopsis:    SHORT   XEptHostSend(USHORT usLen);
*
*     InPut:    usLen       : length of send data
*     Output:   nothing
*     InOut:    nothing
*
** Return:      CPM_RET_SUCCESS     : success send
*               other               : send failure
*
** Description: This function send message by NETWORK.
*===========================================================================
*/
SHORT   XEptDllSend(VOID *pEEptDllSndBuf, USHORT usLen)
{
	EEPTREQDATA* reqData = &((EEPTREQUNION *)pEEptDllSndBuf)->u.eeptReqData;
	ULONG	ulMaskData = 0;
    SHORT   sError=0;
	USHORT usTender = 0, usTranType = 0, usTranCode = 0;
	CHAR  aszWork[16] = {0};

	memcpy(&aszWork, reqData->auchTenderID, sizeof(reqData->auchTenderID));
	usTender = atoi(aszWork);

	if (XEEptGetInterfaceType() == EEPT_DLL_UNAVAILABLE) {
		// create a response with 1006 general failure error in it.
		EEPTRSPDATA *resData = (EEPTRSPDATA*)&(EEptSpool.auchData);

		resData->auchEptStatus = EEPT_FC2_REJECTED;
		memcpy(resData->auchSrcTerminal, reqData->auchSrcTerminal, sizeof(resData->auchSrcTerminal));
		memcpy(resData->auchDesTerminal, reqData->auchDesTerminal, sizeof(resData->auchDesTerminal));
		resData->auchTerminalNo = reqData->auchTerminalNo;
		_tcscpy (resData->auchErrorText, _T("SYSTEM ERROR: DSI Unavailable."));
		_tcscpy (resData->auchMsgText, _T("SYSTEM ERROR: DSI Unavailable."));
		strncpy ((char *)(resData->auchErrorNumber), "001006", 6);  // create an error code of EEPTINTERFACE_DECRYPT_ERROR
		strcpy((char*)resData->auchAuthorize, "0.00");
		EEptSpool.sErrCode = CPM_RET_BADPROV;
		return CPM_RET_BADPROV;
	} else {
		// get a working copy of the Tran Type string then convert it.
		memcpy(&aszWork, reqData->auchTranType, sizeof(reqData->auchTranType));
		usTranType = atoi(aszWork);

		if(usTranType == TENDERKEY_TRANTYPE_PREPAID)	// if tran type is gift card
		{												// get seperate masking info
			XEptSetUpGiftMaskInfo(&ulMaskData);			// from MDC 362.
			sError = XEEptBuildXMLAndExecuteTrans(pEEptDllSndBuf, &EEptSpool, &ulMaskData);
		} else if (usTranType == TENDERKEY_TRANTYPE_FREEDOMPAY) {
			XEptSetUpGiftMaskInfo(&ulMaskData);			// from MDC 362.
			sError = XEEptBuildFreedomPayAndExecuteTrans (reqData, &EEptSpool, &ulMaskData);
		}else {
			// get a working copy of the Tran Code string then convert it.
			memcpy(&aszWork, reqData->auchTranCode, sizeof(reqData->auchTranCode));
			usTranCode = atoi(aszWork);
			if (usTranCode == TENDERKEY_TRANCODE_PREAUTH) {
				usTender += usTranCode * 100;
			}
			XEptSetUpMaskInformation (usTender, &ulMaskData);
			sError = XEEptBuildXMLAndExecuteTrans(pEEptDllSndBuf, &EEptSpool, &ulMaskData);
		}
	}

	if (sError >= EPT_SUCCESS) {                         /* E-067 corr. '94 4/26 */
        return(CPM_RET_SUCCESS);
    } else {                                        /* error case           */
        return(sError);
    }
}

SHORT XEptStoreForwardReport(short sTypeDsi, EEPTF_HEADER * S_F_Data){
	return XEEptReportStoreForward(sTypeDsi, S_F_Data);
}

LONG	XEptGetNextBlockIterateAll(short sTypeDsi, EEPT_BLOCK_HEADER * blockHeader){
	return XEEptGetNextBlockIterateAll(sTypeDsi, blockHeader);
}

LONG	XEptGetNextBlockIterate(short sTypeDsi, EEPT_BLOCK_HEADER * blockHeader){
	return XEEptGetNextBlockIterate(sTypeDsi, blockHeader);
}

LONG	XEptGetSpecifiedBlockHeader(short sTypeDsi, ULONG ulBlockOffset, EEPT_BLOCK_HEADER * blockHeader){
	return XEEptGetSpecifiedBlockHeader(sTypeDsi, ulBlockOffset, blockHeader);
}

LONG	XEptUpdateSpecifiedBlock(short sTypeDsi, ULONG ulBlockOffset, EEPT_BLOCK_HEADER * blockHeader){
	return XEEptUpdateSpecifiedBlock(sTypeDsi, ulBlockOffset, blockHeader);
}


/*
	Start thread to forward stored transactions.

*/
SHORT XEptToggleStoreForward(SHORT onOff){
	return XEEptToggleStoreForward(onOff);
}

/*
*===========================================================================
** Synopsis:    SHORT   EEptNetRecv(USHORT usLen);
*
*     InPut:    usLen       : length of send data
*     Output:   nothing
*     InOut:    nothing
*
** Return:      CPM_RET_SUCCESS     : success send
*               other               : send failure
*
** Description: This function send message by NETWORK.
*===========================================================================
*/
SHORT   XEptDllRecv(VOID *pEEptRcvBuf, USHORT usLen)
{
    SHORT   sError=EEPT_RET_TIMEOUT, sRetry=2;
	USHORT	usTimer = 500;      // 500 millisecond time out set

	return(sError);
}

/*
*===========================================================================
** Synopsis:    VOID   XEptDllSetCurrentDLL(UCHAR uchEEPTDLL);
*
*     InPut:    uchEEPTDLL       : EEPTDll to use
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none
*
** Description: This function sets the current eeptdll.
*===========================================================================
*/
VOID	XEptDllSetCurrentDLL(UCHAR uchEEPTDLL)
{
	uchEEPTDLLCurrent = uchEEPTDLL;
}

/*
*===========================================================================
** Synopsis:    VOID   XEptDllGetPinPadType();
*
*     InPut:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none
*
** Description: This function gets the current pin pad type.
*===========================================================================
*/
USHORT XEptDllGetPinPadType()
{
	return XEEptGetPinPadType();
}

/*
*===========================================================================
** Synopsis:    VOID   XEptDllCheckPinPad();
*
*     InPut:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none
*
** Description: This function checks the status of the current pinpad.
*===========================================================================
*/
BOOL	XEptDllCheckPinPad()
{
	return XEEptCheckPinPad();
}

/*
*===========================================================================
** Synopsis:    SHORT XEptDllCollectStripe(SHORT sCardTimeOut, BOOL bScreen)
*
*     InPut:    sCardTimeOut	: Seconds to wait for card swipe
*     Output:   bScreen			: Control Screen (datacap only)
*     InOut:    nothing
*
** Return:      none
*
** Description: This function waits and retrieves card swipe
*===========================================================================
*/
SHORT	XEptDllCollectStripe(SHORT sCardTimeOut, BOOL bScreen)
{
	return XEEptCollectStripe(sCardTimeOut,bScreen);
}

/*
*===========================================================================
** Synopsis:    SHORT	XEptDllCollectStripeAndPin(TCHAR* strAmount,
*												   SHORT sCardTimeOut,
*												   SHORT sPinTimeOut,
*												   BOOL bScreen,
*												   UCHAR uchDecimal);
*
*     InPut:    strAmount		: Amount of transaction
*				sCardTimeOut	: Seconds to wait for card swipe
*				sPinTimeOut		: Seconds to wait for pin
*				bScreen			: Control Screen (datacap only)
*				uchDecimal		: decimal places the amount has (datacap only)
*     Output:   nothing
*     InOut:    nothing
*
** Return:      short			: Error code from Active X Control
*
** Description: This function waits and retrieves card swipe and pin
*===========================================================================
*/
SHORT	XEptDllCollectStripeAndPin(TCHAR* strAmount,
								   SHORT sCardTimeOut,
								   SHORT sPinTimeOut,
								   BOOL bScreen,
								   UCHAR uchDecimal)
{
	return XEEptCollectStripeAndPin(strAmount,
								   sCardTimeOut,
								   sPinTimeOut,
								   bScreen,
								   uchDecimal);
}

/*
*===========================================================================
** Synopsis:    SHORT	XEptDllCollectPin(TCHAR* strAmount,
*										   TCHAR strAcctNo,
*										   SHORT sPinTimeOut,
*										   BOOL bScreen,
*										   UCHAR uchDecimal);
*
*     InPut:    strAmount		: Amount of transaction
*				strAcctNo		: Account Number of transaction
*				sPinTimeOut		: Seconds to wait for pin
*				bScreen			: Control Screen (datacap only)
*				uchDecimal		: decimal places the amount has (datacap only)
*     Output:   nothing
*     InOut:    nothing
*
** Return:      short			: Error code from Active X Control
*
** Description: This function waits and retrieves a pin
*===========================================================================
*/
SHORT	XEptDllCollectPin(TCHAR* strAmount,
						  TCHAR* strAcctNo,
						  SHORT sPinTimeOut,
						  BOOL bScreen,
						  UCHAR uchDecimal)
{
	return XEEptCollectPin(strAmount,
						  strAcctNo,
						  sPinTimeOut,
						  bScreen,
						  uchDecimal);
}

/*
*===========================================================================
** Synopsis:    SHORT	XEptDllGetLastStripe(TCHAR* tchMagStripe);
*
*     InPut:    tchMagStripe		: Mag Stripe Information
*
*     Output:   nothing
*     InOut:    tchMagStripe		: Mag Stripe from MSR reader
*
** Return:      none
*
** Description: This function retreives msr stripe information from dll
*				interfaces
*===========================================================================
*/
VOID	XEptDllGetLastStripe(TCHAR* tchMagStripe)
{
	XEEptGetLastStripe(tchMagStripe);
}

/*
*===========================================================================
** Synopsis:    VOID	XEptDllResetPinPad();
*
*     InPut:    none
*
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none
*
** Description: This function resets the pinpad
*===========================================================================
*/
VOID	XEptDllResetPinPad(VOID)
{
		XEEptResetPinPad();
}

VOID	XEptDllCancelRequest(VOID)
{
		XEEptCancelRequest();
}
/*
*===========================================================================
** Synopsis:    VOID	XEptSetUpMaskInformation();
*
*     InPut:    usTenderNo
				*plMaskData
*
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none
*
** Description: This function sets up the mask information
*===========================================================================
*/
VOID	XEptSetUpMaskInformation(USHORT usTenderNo, ULONG *plMaskData)
{
	int iMaskedChars = 0;  //number of characters to mask
	int iShownChars = 0;   //number of characerst MDC indicates should not be masked
	PARAMDC MaskMDC, TenderMDC;  //Used to read MDC masking settings and Tender Key settings
	UCHAR	oddOrEven, localMDCBitA, localMDCMask; //The following UCHARS are used in reading MDC settings of Tender Keys
	USHORT	usMDCBase, usOffset,usAddress;
	TCHAR	aszNumber[NUM_NUMBER];

	*plMaskData = 0;   // Init the mask data to zero so we will OR in settings below.

	// check to see if this is a special type of tender such as a pre-auth
	// if so then pull out the special code and then keep only the actual tender number.
	if (usTenderNo > 99) {
		// this is a Pre-Auth capture tender so we use the Pre-Auth masking rules
		MaskMDC.uchMajorClass = CLASS_PARAMDC;
		MaskMDC.usAddress = MDC_PREAUTH_MASK_ADR;
		ParaRead( &MaskMDC );

		if ((MaskMDC.uchMDCData & EVEN_MDC_BIT1) == 0) {
			*plMaskData |= EEPTINTERFACE_MASK_ACCTNUM;
			if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT2){
				//show 2 rightmost digits
				*plMaskData |= EEPTINTERFACE_MASK_MASK2;
			} else if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT3){
				//show 4 rightmost digits
				*plMaskData |= EEPTINTERFACE_MASK_MASK4;
			} else if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)){
				//show 6 rightmost digits
				*plMaskData |= EEPTINTERFACE_MASK_MASK6;
			} else {
				//mask entire account number unless otherwise indicated
				*plMaskData |= EEPTINTERFACE_MASK_MASK0;
			}
			*plMaskData |= EEPTINTERFACE_MASK_EXPNUM;
		}
		return;
	}

	memset(&aszNumber, 0x00,sizeof(aszNumber));

	// Determine the correct Address to look in for the MDC
	// The number of tender keys has changed from the initial number
	// used in NHPOS 1.4 to the 20 tender keys currently in GenPOS Rel 2.2.1.
	// So we have to more than one range of MDC address values to use.
	// For Tenders 1-11, the address ranges from 180 to 190
	// For Tender 12-20., the address ranges from 403 to 411
	if (usTenderNo >= CLASS_UITENDER1 && usTenderNo <= CLASS_UITENDER11) {
		usMDCBase = MDC_CPTEND1_ADR;
		usOffset = CLASS_UITENDER1;
	} else if (usTenderNo > CLASS_UITENDER11 && usTenderNo <= CLASS_UITENDER20) {
		usMDCBase = MDC_CPTEND12_ADR;
		usOffset = CLASS_UITENDER12;
	}
	else {
		//mask entire account number unless otherwise indicated
		*plMaskData |= EEPTINTERFACE_MASK_ACCTNUM;
		*plMaskData |= EEPTINTERFACE_MASK_MASK0;
		return;
	}

	usAddress = usMDCBase + (usTenderNo - usOffset);
	oddOrEven = (usAddress) % 2;

	localMDCBitA = oddOrEven ? ODD_MDC_BIT3 : EVEN_MDC_BIT3;
	localMDCMask = oddOrEven ? MDC_ODD_MASK : MDC_EVEN_MASK;

	//if MDC indicates use of masking
	if(ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT2) || 
	   ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT3) ||
	   ParaMDCCheck(MDC_EPT_MASK3_ADR, ODD_MDC_BIT0))
	{

		*plMaskData |= EEPTINTERFACE_MASK_ACCTNUM;

		TenderMDC.uchMajorClass = CLASS_PARAMDC;
		TenderMDC.usAddress = usAddress;
		ParaRead(&TenderMDC);

		//SR 131 cwunn 7/17/2003 Determine masking settings
		MaskMDC.uchMajorClass = CLASS_PARAMDC;

		//Tenders 12-20 use MDC 443 for digit masking
		//Tenders less than 12 use MDC 378 for digit masking
		if ((usTenderNo >= CLASS_UITENDER12) && (usTenderNo <= CLASS_UITENDER20)) { 
			MaskMDC.usAddress = MDC_EPT_MASK3_ADR;
		} else {
			MaskMDC.usAddress = MDC_EPT_MASK2_ADR;
		}

		ParaRead( &MaskMDC );

		if (((usTenderNo >= CLASS_UITENDER12) && (usTenderNo <= CLASS_UITENDER20)) &&
			ParaMDCCheck(MDC_EPT_MASK3_ADR, ODD_MDC_BIT0)) {

			if((MaskMDC.uchMDCData & (ODD_MDC_BIT3 | ODD_MDC_BIT2)) == ODD_MDC_BIT2){
				//show 2 rightmost digits
				*plMaskData |= EEPTINTERFACE_MASK_MASK2;
			}else
			if((MaskMDC.uchMDCData & (ODD_MDC_BIT3 | ODD_MDC_BIT2)) == ODD_MDC_BIT3){
				//show 4 rightmost digits
				*plMaskData |= EEPTINTERFACE_MASK_MASK4;
			}else
			if((MaskMDC.uchMDCData & (ODD_MDC_BIT3 | ODD_MDC_BIT2)) == (ODD_MDC_BIT3 | ODD_MDC_BIT2)){
				//show 6 rightmost digits
				*plMaskData |= EEPTINTERFACE_MASK_MASK6;
			}else
			{
				//mask entire account number unless otherwise indicated
				*plMaskData |= EEPTINTERFACE_MASK_MASK0;
			}
		}

		//SR 131 cwunn 7/17/2003 Indicate how many account digits to show
		if(((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA) &&
			ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT2)) { //tender status indicates EPT & MDC masks EPTs
			if((usTenderNo >= CLASS_UITENDER2) && (usTenderNo <= CLASS_UITENDER10)){ //Mask only if on Tenders 2-6
				//mask entire account number unless otherwise indicated
				iShownChars = 0;

				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT2){
					//show 2 rightmost digits
					*plMaskData |= EEPTINTERFACE_MASK_MASK2;
				}else
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT3){
					//show 4 rightmost digits
					*plMaskData |= EEPTINTERFACE_MASK_MASK4;
				}else
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)){
					//show 6 rightmost digits
					*plMaskData |= EEPTINTERFACE_MASK_MASK6;
				}else
				{
					*plMaskData |= EEPTINTERFACE_MASK_MASK0;
				}
			}
		}

		if(((TenderMDC.uchMDCData & localMDCMask) != localMDCBitA) &&
			ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT3)){ //tender status indicates CP (not EPT)
			if((usTenderNo >= CLASS_UITENDER11) && (usTenderNo <= CLASS_UITENDER20)){ //Mask only if on Tenders 7-11
				//mask entire account number unless otherwise indicated
				iShownChars = 0;
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT1 | EVEN_MDC_BIT0)) == EVEN_MDC_BIT0){
					//show 2 rightmost digits
					*plMaskData |= EEPTINTERFACE_MASK_MASK2;
				}else
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT1 | EVEN_MDC_BIT0)) == EVEN_MDC_BIT1){
					//show 4 rightmost digits
					*plMaskData |= EEPTINTERFACE_MASK_MASK4;
				}else
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT1 | EVEN_MDC_BIT0)) == (EVEN_MDC_BIT1 | EVEN_MDC_BIT0)){
					//show 6 rightmost digits
					*plMaskData |= EEPTINTERFACE_MASK_MASK6;
				}else
				{
					*plMaskData |= EEPTINTERFACE_MASK_MASK0;
				}
			}
		}
	}

	if(!ParaMDCCheck(MDC_EPT_MASK3_ADR,ODD_MDC_BIT1))
	{
		*plMaskData |= EEPTINTERFACE_MASK_EXPNUM;
	}
}

/*
*===========================================================================
** Synopsis:    VOID	XEptSetUpGiftMaskInfo(ULONG *plMaskData);
*
*     InPut:    none
*
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none
*
** Description: This function determines how many numbers to mask of a gift
*				card number.
*===========================================================================
*/
VOID	XEptSetUpGiftMaskInfo(ULONG *plMaskData)
{
	PARAMDC MaskMDC;
	
	*plMaskData = 0;
	if(!ParaMDCCheck(MDC_GIFT_MASK, EVEN_MDC_BIT1))
	{
		MaskMDC.uchMajorClass = CLASS_PARAMDC;
		MaskMDC.usAddress = MDC_GIFT_MASK;
		ParaRead(&MaskMDC);
		
		*plMaskData |= EEPTINTERFACE_MASK_ACCTNUM;

		if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT2){
			//show 2 rightmost digits
			*plMaskData |= EEPTINTERFACE_MASK_MASK2;
		}else
		if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT3){
			//show 4 rightmost digits
			*plMaskData |= EEPTINTERFACE_MASK_MASK4;
		}else
		if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)){
			//show 6 rightmost digits
			*plMaskData |= EEPTINTERFACE_MASK_MASK6;
		}else
		{
			//mask entire account number unless otherwise indicated
			*plMaskData |= EEPTINTERFACE_MASK_MASK0;
		}
	}
}
/* ---------- End of xeptdllif.c ---------- */

