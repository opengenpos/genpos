/*
*===========================================================================
*
*   Copyright (c) 1993-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Coin Dispenser Module
* Category    : User Interface Engine, 2170 System Application
* Program Name: UieCoin.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:     The provided function names are as follows:
*
*                   UieCoinSendData() - send the change amount
*                   UieCoinSendData2() - send the change amount
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.     Name          Description
* May-20-93  0.00.01  O.Nakada      Initial
* Mar-15-95  G0       O.Nakada      Modified all modules, correspond to
*                                   hospitality release 3.0 functions.
* Feb-19-97  G9       M.Suzuki      Added 410 coin dispenser.
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
#include <windows.h>
#ifndef _WIN32_WCE
#include <tchar.h>
#endif
#include "Scf.h"
#include "DeviceIOCoin.h"
#include "UIELDEV.h"

#include "ecr.h"
#include "pif.h"
#include "uie.h"
#include "uiel.h"
#include "uielio.h"
#include "uieio.h"
#include "BlFWif.h"


/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
STATIC  SHORT   sUieCoin = -1;              /* handle of serial port */
STATIC  USHORT  usUieCoinPort = 0;          /* serial port number */

/* for DLL interface */

LOAD_FUNCTION	funcCoin;
HINSTANCE		hCoinDll;
HANDLE			hCoin = NULL;

/* for */

VOID	UieCoinInitEx(VOID);
SHORT	UieCoinSendDataEx(DCURRENCY lChange);


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID UieCoinInit(VOID);
*
** Return:      nothing
*
** Description: This function will be called when initiale/master reset or
*               power down recovery.
*===========================================================================
*/
VOID UieCoinInit(VOID)
{
	UieCoinInitEx();
}


/*
*===========================================================================
** Synopsis:    SHORT DLLENTRY UieCoinSendData(LONG lChange);
*     Input:    lChange - change amount
*
** Return:      UIE_SUCCESS                - successful
*               UIE_COIN_ERROR_NOT_PROVIDE - coin dispenser not provide
*               UIE_COIN_ERROR_RANGE       - range error
*               UIE_COIN_ERROR_COM         - communication error
*===========================================================================
*/
SHORT DLLENTRY UieCoinSendData(DCURRENCY lChange)
{
	return UieCoinSendDataEx(lChange);
}


/*
*===========================================================================
** Synopsis:    SHORT DLLENTRY UieCoinSendData2(SHORT sChange);
*     Input:    sChange - change amount
*
** Return:      UIE_SUCCESS                - successful
*               UIE_COIN_ERROR_NOT_PROVIDE - coin dispenser not provide
*               UIE_COIN_ERROR_RANGE       - range error
*               UIE_COIN_ERROR_COM         - communication error
*===========================================================================
*/
SHORT DLLENTRY UieCoinSendData2(SHORT sChange)
{
	return UieCoinSendDataEx((DCURRENCY)sChange);
}


/*
*===========================================================================
** Synopsis:    VOID UieCoinInitEx(VOID);
*
** Return:      nothing
*
** Description: This function will be called when initiale/master reset or
*               power down recovery.
*===========================================================================
*/
VOID UieCoinInitEx(VOID)
{
//	BOOL	bResult;
	DWORD	dwResult;
	DWORD	dwCount;
	TCHAR	achDevice[SCF_USER_BUFFER];
	TCHAR	achDLL[SCF_USER_BUFFER];
//	TCHAR	*pchDevice;
//	TCHAR	*pchDLL;

	// clear buffer

	memset(achDevice, '\0', sizeof(achDevice));
	memset(achDLL,    '\0', sizeof(achDLL));

	// get active device information

	dwResult = ScfGetActiveDevice(SCF_TYPENAME_COIN, &dwCount, achDevice, achDLL);

	if (dwResult != SCF_SUCCESS)
	{
		return;
	}

	hCoin = (HANDLE)1;//Set hCoin to other than zero if SCF_SUCCESS

//	pchDevice = achDevice;
//	pchDLL    = achDLL;

//	bResult = UieLoadDevice(pchDLL, &funcCoin, &hCoinDll);

//	if (! bResult)
//	{
//		return;
//	}

//	hCoin = funcCoin.Open(pchDevice);
}


/*
*===========================================================================
** Synopsis:    SHORT DLLENTRY UieCoinSendDataEx(LONG lChange);
*     Input:    sChange - change amount
*
** Return:      UIE_SUCCESS                - successful
*               UIE_COIN_ERROR_NOT_PROVIDE - coin dispenser not provide
*               UIE_COIN_ERROR_RANGE       - range error
*               UIE_COIN_ERROR_COM         - communication error
*===========================================================================
*/
SHORT UieCoinSendDataEx(DCURRENCY lChange)
{
//	DWORD					dwResult;
//	DWORD					dwBytesWritten;
//	DEVICEIO_COIN_DISPENSE	Data;

	if (! hCoin)
	{
		return UIE_COIN_ERROR_NOT_PROVIDE;
	}

	return BlFwIfDispenseCoin(lChange);

//	Data.lAmount = lChange;

//	dwResult = funcCoin.Write(hCoin, &Data, sizeof(Data), &dwBytesWritten);

//	if (dwResult != DEVICEIO_SUCCESS)
//	{
//		return UIE_COIN_ERROR_COM;
//	}

//	return UIE_SUCCESS;
}


/* ====================================== */
/* ========== End of UieCoin.C ========== */
/* ====================================== */
