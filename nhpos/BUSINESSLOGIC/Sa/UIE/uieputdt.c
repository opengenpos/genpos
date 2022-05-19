/*
*===========================================================================
*
*   Copyright (c) 1999, NCR Corporation, E&M-OISO. All rights reserved.
*
*===========================================================================
* Title       : abstructed data input
* Category    : User Interface Enigne, 2171 System Application
* Program Name: UiePutDt.H
* --------------------------------------------------------------------------
* Compiler    : MS-VC++ Ver.6.00 by Microsoft Corp.
* Memory Model: 
* Options     : 
* --------------------------------------------------------------------------
* Abstract:     The provided function names are as follows:
*
*                   UiePutData()     - put data from user i/f device
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Sep-28-99  01.00.00  M.Teraki     Initial
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

/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <tchar.h>

#include "ecr.h"
#include "pif.h"
#include "bl.h"
#include "uie.h"
#include "uiel.h"
#include "fsc.h"

#include    "cscas.h"
#include "BlFWif.h"
/*
*===========================================================================
*   General Declarations
*===========================================================================
*/

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

SHORT  UieCheckMsrDeviceType (USHORT usFlags)
{
	const SYSCONFIG *pSysConfig = PifSysConfig();

	return ((pSysConfig->uchIntegMSRType & usFlags) != 0);
}

/*
*===========================================================================
** Synopsis:    LONG UiePutData(CONST _DEVICEINPUT *pData, VOID *pResult);
*     Input:    pData   - pointer to input data
*    Output:    pResult - pointer to result data(reserved)
*
** Return:      reserved
*===========================================================================
*/
LONG UiePutData(CONST _DEVICEINPUT *pData, VOID *pResult)
{
    LONG lRet = -1;

    switch (pData->ulDeviceId) {
		case BL_DEVICE_DATA_MSR:		/* MSR */
			lRet = UiePutMsrData(pData->ulLength, pData->pvAddr, pResult);
			break;

		case BL_DEVICE_DATA_SCANNER:	/* SCANNER */
			lRet = UiePutScannerData(pData->ulLength, pData->pvAddr, pResult);
			break;

		case BL_DEVICE_DATA_KEY:		/* KEYBOARD, KEYLOCK, KEYERROR... */
			CasAutoSignOutReset();
			lRet = UiePutKeyData(pData->ulLength, pData->pvAddr, pResult);
			// If this key press is either an AC key or a #/Type key then lets send a control string event
			{
				CHARDATA* pCharData = pData->pvAddr;

				if ( (pCharData->uchASCII == 0xCA && pCharData->uchScan == 0x00) ||  // CWindowButton::BATypeACKey or FSC_AC
					 (pCharData->uchASCII == 0xC2 && pCharData->uchScan == 0x00) )   // CWindowButton::BATypeNumType or FSC_NUM_TYPE
				{
//					UifSignalStringWaitEventIfWaiting (CNTRL_STRING_EVENT_WINDOW_INPUT);     // send event if FSC_AC or FSC_NUM_TYPE
				}
			}
			break;

		case BL_DEVICE_DATA_DRAWER:	/* DRAWER */
			lRet = UiePutDrawerData(pData->ulLength, pData->pvAddr, pResult);
			break;

		case BL_DEVICE_DATA_SERVERLOCK:	/* SERVER LOCK */
			lRet = UiePutServerLockData(pData->ulLength, pData->pvAddr, pResult);
			break;

		case BL_DEVICE_DATA_POWERDOWN:
			PifControlPowerSwitch(-1);//UiePutPowerDownData();
			break;

		case BL_WM_CHAR:	/*WM_CHAR message added for TOUCHSCREEN */
			CasAutoSignOutReset();
			lRet = UieSendKeyboardInput(pData->ulLength, pData->pvAddr);
			break;

		case BL_DEVICE_DATA_VIRTUALKEY:
			CasAutoSignOutReset();
			// KEYBOARD_VIRTUAL message with VIRTUALKEY_BUFFER to support FreedomPay, etc.
			lRet = UiePutVirtualKeyData(pData->ulLength, pData->pvAddr, pResult);
			break;

		case BL_DEVICE_DATA_PINPAD:		/* PINPad device */
			lRet = UiePutPinPadData(pData->ulLength, pData->pvAddr, pResult);
			break;

		case BL_DEVICE_DATA_SIGCAP:		/* PINPad device */
			lRet = UiePutSignatureCaptureData(pData->ulLength, pData->pvAddr, pResult);
			break;

		case BL_DEVICE_DATA_OPERATOR:	/* Operator Action message */
			lRet = UiePutOperatorActionData(pData->ulLength, pData->pvAddr, pResult);
			break;

		default:
			NHPOS_ASSERT_TEXT(0, "UiePutData(): Unknown ulDeviceId.");
			break;
	}

	return lRet;
}

/* ===================================== */
/* ========== End of uieputdt.c ======== */
/* ===================================== */
