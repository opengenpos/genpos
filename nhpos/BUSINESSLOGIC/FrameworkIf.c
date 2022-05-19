/*************************************************************************
 *
 * FrameworkIf.cpp
 *
 * $Workfile:: FrameworkIf.cpp                                           $
 *
 * $Revision::                                                           $
 *
 * $Archive::                                                            $
 *
 * $Date::                                                               $
 *
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
 * Description:  FrameworkIf is the Framework Interface to various functions
 *               within the Businesslogic DLL.  These functions have a published
 *               function name using the BUSINESSLOGIC_API type as defined
 *               in the include file bl.h which must be included into any
 *               file using these functions.  These functions are typically
 *               used by the Framework subsystem to access specific BusinessLogic
 *               functionality in order to tie together the Microsoft Windows
 *               API functionality with the older BusinessLogic and EventSubsystem
 *               modules.
 *
 *     BOOL   BlInitialize(TCHAR *pVersion)
 *     BOOL   BlDelayBalance(VOID)
 *     BOOL   BlUninitialize(VOID *pvDummy)
 *     LONG   BlNotifyData(CONST BL_NOTIFYDATA *pData, VOID *pResult)
 *     USHORT BlGetKeyboardLockPosition (VOID)
 *     USHORT BlGetNoticeBoardStatus (int iTestComs)
 *     BOOL   BlSetFrameworkFunc(ULONG ulCommand, VOID* pFunc)
 *     LONG   BlSendKeyboardData(CONST BL_NOTIFYDATA *pData, VOID *pResult)
 *     SHORT  BlCheckAndSetTerminalLock(VOID)
 *     UCHAR  BlFwIfReadMode(VOID)
 *     SHORT  BlResetTerminalLock(SHORT sTermLockHandle)
 *     SHORT  BlFwIfMldSetReceiptWindow(USHORT usScroll)
 *     USHORT BlFwIfMldGetVosWinHandle(USHORT usScroll)
 *     SHORT  BlFwIfMldNotifyCursorMoved(USHORT usScroll)
 *     WCHAR *BlFwIfTerminalLayout(TERMINALINFO *pTermInfo)
 *     VOID  BlFwIfSetWindowHandle(VOID *pMainHwnd)
 *     int   BlFwIfGetIndPluInformation(USHORT usGroupNumber, TCHAR * puchPLUNumber, BL_PLU_INFO *pPluInfo)
 *     int   BlFwIfGetGroupPluInformationNext(BL_PLU_INFO *pPluInfo)
 *     int   BlFwIfGetGroupPluInformationFirst(USHORT usGroupNumber, BL_PLU_INFO *pPluInfo)
 *     int   BlFwIfConvertPLU (UCHAR *aszPluNo, BL_PLU_INFO *pPluInfo)
 *     BOOL  BlFwIfIsPluFileUnLocked ()
 *
 *************************************************************************
 *
 * Copyright (c) NCR Corporation 2000.  All rights reserved.
 *
 *************************************************************************
 *
 * $History:: FrameworkIf.cpp                                            $
 *
** GenPOS **
* Feb-19-18: 02.02.02  : R.Chambers  : BlFwIfSetOEPGroups() return error if no OEP windows
 ************************************************************************/

#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "..\..\ConnEngineMfc\ConnEngine.h"

#include "pif.h"
#include "FrameworkIO.h"
#include "DeviceIOPrinter.h"
#define __BLNOTIFY_C__
#include "bl.h"
#include "BlFWif.h"
#include "uie.h"
#include "uic.h"
#include "uiedisp.h"
#include "uieio.h"
#include "uieecho.h"
#include "paraequ.h"
#include "para.h"
#include "ECR.h"
#include "regstrct.h"
#include "csetk.h"
#include "transact.h"
#include "regstrct.h"
#include "mld.h"
#include "csstbpar.h"
#include "csstbgcf.h"
#include "..\BusinessLogic\mld\INCLUDE\mldlocal.h"
#include "..\BusinessLogic\Item\Include\itmlocal.h"
#include "evs.h"
#include "nb.h"
#include "..\BusinessLogic\nb\nbcom.h"
//#include "mldlcd.h"
#include "uifsup.h"
#include "display.h"
#include "uifpgequ.h"
#include "rfl.h"
#include "fsc.h"
#include "csop.h"
#include "csstbopr.h"
#include    <csstbpar.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csstbstb.h>
#include    <csserm.h>
#include  "uireg.h"
#include  "pifmain.h"
#include  "EEPTL.H"
#include "maint.h"
#include "uieerror.h"

#include "httpmain.h"
#include "cscas.h"
#include "csstbcas.h"
#include "csgcs.h"
#include  "ConnEngineObjectIf.h"
#include "item.h"
#include "pmg.h"

extern UCHAR ParaMDCCheck( USHORT usAddress, UCHAR uchMaskData );
extern VOID  ParaRead(VOID *pDATA);

VOID HttpCreateMain(VOID);

// Following macro is used to standardize the decision as to whether
// the check digit on a PLU number should be removed or not.  This
// macro uses the memcmp () function to determine if the PLU is a 5 digit
// PLU number or not by checking to see if there are 7 leading zeros in the
// PLU number.  If there are then the memcmp () function returns 0.
#define IS_REMOVE_PLU_CHECK_DIGIT(x) (memcmp (_T("0000000"), (x), 7 * sizeof(TCHAR)) == 0)


static  ULONG  	BlFwPmgLastError = 0;

typedef struct {
	USHORT    usGroupNo;
	USHORT    usTableNo;
	USHORT    usFilePart;
	USHORT    usFlags;
	BL_PLU_INFO  PluPara;
} PLU_OEP_INDEX;


static PLU_OEP_INDEX  PluOepIndexArray[3000];

static SHORT InsertOepPluItem (BL_PLU_INFO  *pPluPara)
{
	SHORT  sRetStatus = -1;
	int i = 0;

	for (i = 0; i < sizeof (PluOepIndexArray)/sizeof(PluOepIndexArray[0]); i++) {
		if (PluOepIndexArray[i].usGroupNo == pPluPara->usGroupNo && PluOepIndexArray[i].usTableNo == pPluPara->usTableNo && PluOepIndexArray[i].usFilePart == pPluPara->usFilePart) {
			if ((memcmp (&(pPluPara->auchPluNo), &(PluOepIndexArray[i].PluPara.auchPluNo), sizeof(pPluPara->auchPluNo)) == 0) &&
					(pPluPara->uchPluAdj == PluOepIndexArray[i].PluPara.uchPluAdj) ) {
				PluOepIndexArray[i].PluPara = *pPluPara;
				PluOepIndexArray[i].usFlags |= 1;
				PluOepIndexArray[i].usGroupNo = pPluPara->usGroupNo;
				PluOepIndexArray[i].usTableNo = pPluPara->usTableNo;
				PluOepIndexArray[i].usFilePart = pPluPara->usFilePart;
				sRetStatus = 0;
				pPluPara->ulInternalCounter = i+1;
				break;
			}
		}

		if (PluOepIndexArray[i].usFlags == 0) {
			PluOepIndexArray[i].PluPara = *pPluPara;
			PluOepIndexArray[i].usFlags |= 1;
			PluOepIndexArray[i].usGroupNo = pPluPara->usGroupNo;
			PluOepIndexArray[i].usTableNo = pPluPara->usTableNo;
			PluOepIndexArray[i].usFilePart = pPluPara->usFilePart;
			sRetStatus = 0;
			pPluPara->ulInternalCounter = i+1;
			break;
		}
	}

	if (i >= sizeof (PluOepIndexArray)/sizeof(PluOepIndexArray[0]) ) {
		NHPOS_ASSERT_TEXT(0, "--WARNING: InsertOepPluItem(): PluOepIndexArray[] bounds exceeded.");
	}
	return sRetStatus;
}

static SHORT FindOepPluItem (BL_PLU_INFO  *pPluPara)
{
	SHORT  sRetStatus = -1;
	int    i = 0;
    ULONG  ulCounter;
    ULONG  ulInternalCounter;

	for (i = pPluPara->ulInternalCounter; i < sizeof (PluOepIndexArray)/sizeof(PluOepIndexArray[0]); i++) {
		if (PluOepIndexArray[i].usGroupNo == pPluPara->usGroupNo && PluOepIndexArray[i].usTableNo == pPluPara->usTableNo && PluOepIndexArray[i].usFilePart == pPluPara->usFilePart) {
			// if we are doing filtering on the adjective value then check to make sure that this cached PLU data is for the
			// correct PLU variation.  Previously we may have used a different adjective of this PLU and it will be in the cache
			// however we do not want to use a cached PLU varient that does not match the window filter.
			if (pPluPara->ulFilterOptions & BL_PLU_INFO_FILTER_ADJ) {
				if ((pPluPara->ulFilterOptions & BL_PLU_INFO_FILTER_ADJ_LOGIC_LT)&& PluOepIndexArray[i].PluPara.uchPluAdj >= pPluPara->usFilterValue) {
					continue;
				}
				if ((pPluPara->ulFilterOptions & BL_PLU_INFO_FILTER_ADJ_LOGIC_GT)&& PluOepIndexArray[i].PluPara.uchPluAdj <= pPluPara->usFilterValue) {
					continue;
				}
				if ((pPluPara->ulFilterOptions & (BL_PLU_INFO_FILTER_ADJ_LOGIC_LT | BL_PLU_INFO_FILTER_ADJ_LOGIC_GT)) == 0 && pPluPara->usFilterValue != PluOepIndexArray[i].PluPara.uchPluAdj) {
					continue;
				}
			}
			// if we are doing filtering on keeping only PLU varients that have a non-zero price
			if ((pPluPara->ulFilterOptions & BL_PLU_INFO_FILTER_ADJNONZERO) && pPluPara->usFilterValue == PluOepIndexArray[i].PluPara.uchPluAdj) {
				int  iAdj = ((pPluPara->usFilterValue < 6) ? pPluPara->usFilterValue : 0);

				if (PluOepIndexArray[i].PluPara.ulUnitPrice[iAdj] < 1)
					continue;
			}
			ulCounter = pPluPara->ulCounter;
			ulInternalCounter = pPluPara->ulInternalCounter;
			*pPluPara = PluOepIndexArray[i].PluPara;
			sRetStatus = 0;
			i++;
			break;
		}
		if (PluOepIndexArray[i].usFlags == 0) {
			break;
		}
	}
	pPluPara->ulInternalCounter = i;
	return sRetStatus;
}

/*
*===========================================================================
*   Local Definitions
*===========================================================================
*/

static BLCBPROC	s_pfnBlInterface = 0;
static BLAYPROC s_pfnBlInterfaceLay = 0;
static BLAYPROC s_pfnBlInterfaceOep = 0;
static BLAYPROC s_pfnBlNagScreen = 0;
static BLAYPROC s_pfnBlDefaultUserScreen = 0;
static BOEPPROC s_pfnBlSetOepGroups = 0;
static BOEPIMAGE s_pfnBlDisplayOEPImagery = 0;
static BOEPPROC2 s_pfnBlGetMaxOepButtons = 0;
static BTIMEOUT s_pfnBlSetAutoSignOutTime = 0;
static BLAYPROC s_pfnBlForceShutdown = 0;
static BMSGPROC s_pfnBlInterfaceFrameworkMessage = 0;

//AXControl interface for security with Softlocx
static BLCBPROC s_pfnBlAXInterface = 0;
static BLAXPROC s_pfnBlAXExp = 0;
static BLAXPROC s_pfnBlAXSt = 0;

// VBOControl interface for security with VBOlock
static BLCBPROC s_pfnBlVBOInterface = 0;
static BLAXPROC s_pfnBlVBOExp = 0;
static BLAXPROC s_pfnBlVBOSt = 0;


/*
*===========================================================================
** Synopsis:    BOOL BlInitialize(VOID *pvDummy)
*
*     Input:    pvData  - not use
*
*    Output:    nothing
*
** Return:      Reserved <not defined yet>
*===========================================================================
*/
__declspec(dllexport)
BOOL BlInitialize(TCHAR *pVersion)
{
    /* initialize business logic */

	memset (&PluOepIndexArray, 0, sizeof(PluOepIndexArray));

    PifMain(pVersion);

    return TRUE;
}

__declspec(dllexport)
BOOL BlInitializeOepTable(VOID)
{
    /* initialize business logic */

	memset (&PluOepIndexArray, 0, sizeof(PluOepIndexArray));

    return TRUE;
}

__declspec(dllexport)
BOOL BlFinalizeAndClose(VOID)
{
	CliParaSaveFarData();
	return TRUE;
}

/*
*===========================================================================
** Synopsis:    BOOL BlInitHttp(VOID *pvDummy)
*
*     Input:    pvData  - not use
*
*    Output:    nothing
*
** Return:      Reserved <not defined yet>
*===========================================================================
*/
__declspec(dllexport)
BOOL BlInitHttp(VOID *pvDummy)
{
    /* initialize business logic */

	HttpThreadMyInterface = * (ServerProcInterface *) pvDummy;
	HttpCreateMain ();

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    BOOL BlInitConnEngine(VOID *pvDummy)
*
*     Input:    pvData  - not use
*
*    Output:    nothing
*
** Return:      Reserved <not defined yet>
*===========================================================================
*/
__declspec(dllexport)
int BlInitConnEngine(int nPort, HWND hWinHandle, UINT wReceiveMsgId)
{
	DWORD   dwResult = 0, dwCount = 0;
	TCHAR   achDevice[SCF_USER_BUFFER] = {0};
    TCHAR   achDLL[SCF_USER_BUFFER] = {0};
	USHORT usConnEngineObjectEchoElectronicJournalFlag = 0;

	if (CliParaMDCCheckField(MDC_EJ_ADR, MDC_PARAM_BIT_C)) {
		usConnEngineObjectEchoElectronicJournalFlag = 1;
	}

    dwResult = ScfGetActiveDevice(SCF_TYPENAME_CONNENGINEMFC, &dwCount, achDevice, achDLL); // SCF_DATANAME_ISENABLED
	if (ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_AMTRAK) && usConnEngineObjectEchoElectronicJournalFlag) {
		// if Amtrak and Connection Engine Interface Electronic Journal is turned on then force the
		// Connection Engine Interface to be on regardless of setting.
		dwResult = dwCount = 1;
		_tcscpy (achDevice, SCF_DATANAME_ISENABLED);
		_tcscpy (achDLL, _T("1"));
	}
	if (dwResult >= 0 && dwCount > 0) {
		if (_tcscmp (achDevice, SCF_DATANAME_ISENABLED) == 0) {
			if (_ttoi (achDLL)) {
				/* initialize business logic */
				NHPOS_NONASSERT_NOTE("==STATE", "==STATE:  --CONNECTIONENGINE is turned ON.");

				if (usConnEngineObjectEchoElectronicJournalFlag) {
					NHPOS_NONASSERT_TEXT("    -- TOTALEJG is turned on.");
				} else {
					NHPOS_NONASSERT_TEXT("    -- TOTALEJG is turned off.");
				}

				ConnEngineInitCriticalRegion (usConnEngineObjectEchoElectronicJournalFlag);

				return fnConnEngineStartEngine (nPort, hWinHandle, wReceiveMsgId);
			}
		}
	}

	NHPOS_NONASSERT_NOTE("==STATE", "==STATE:  --CONNECTIONENGINE is turned OFF.");
	return 0;
}

__declspec(dllexport)
int BlConnEngineSendButtonPress (TCHAR *tcsWindowName, TCHAR *tcsButtonName,  TCHAR *tcsActionType, int iExtendedFsc, TCHAR *tcsArgument)
{
    /* initialize business logic */
	return ConnEngineSendButtonPress (tcsWindowName, tcsButtonName, tcsActionType, iExtendedFsc, tcsArgument);
}

__declspec(dllexport)
int BlScriptSendButtonPress (TCHAR *tcsWindowName, TCHAR *tcsButtonName,  TCHAR *tcsActionType, int iExtendedFsc, TCHAR *tcsArgument)
{
	return 0;
}

/*
*===========================================================================
** Synopsis:    BOOL BlProcessTimerList(VOID *pvDummy)
*
*     Input:    pvData  - not use
*
*    Output:    nothing
*
** Return:      Reserved <not defined yet>
*===========================================================================
*/
typedef struct {
	SHORT  hsSemaphore;              // if zero or greater than semaphore that needs to be released
	LONG   (*pFunction)(VOID *pData);  // if not NULL then function pointer to be called
	VOID   *pFunctionData;
} BlThreadFunctionTimerListEntry;

static BlThreadFunctionTimerListEntry  myTimerList [] = {
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0},
	{-1, 0, 0}
	};

__declspec(dllexport)
BOOL BlProcessTimerList(VOID *pvDummy)
{
	LONG iIndex = 0;

    // process the list of threads and functions that are queued
	// on the one second recurring timer list.
	for (iIndex = 0; iIndex < sizeof(myTimerList)/sizeof(myTimerList[0]); iIndex++) {
		if (myTimerList[iIndex].hsSemaphore != -1) {
			PifReleaseSem (myTimerList[iIndex].hsSemaphore);
		} else if (myTimerList[iIndex].pFunction != 0 && myTimerList[iIndex].pFunctionData != 0)
		{
			myTimerList[iIndex].pFunction (myTimerList[iIndex].pFunctionData);
		}
	}

    return TRUE;
}

/*
 *  The following functions are used with the timer list functionality to either
 *  add a timer object onto the timer list or to remove a timer object from the timer list.
 *
 *  WARNING: The timer list has pointers to the timer objects which are provided by the
 *           source code using this functionality.
 *
 *           ANY TIMER OBJECTS MUST BE AVAILABLE DURING THE ENTIRE OBJECT LIFECYCLE.
 *
 *           Use of static modifier to create a timer object that is not on the stack is
 *           probably the safest course followed by allocating a timer object on the heap.
 *           Using a stack variable for a timer object may introduce difficult to find
 *           defects in which memory areas are mysteriously modifed as the timer objects
 *           pointer points to an area of the stack that is being used for something else.
**/
LONG BlAddThreadFunctionToTimerList (SHORT  hsSemaphore, LONG (*pFunction)(VOID *pFunctionData), VOID *pFunctionData)
{
	LONG iIndex = 0;

	for (iIndex = 0; iIndex < sizeof(myTimerList)/sizeof(myTimerList[0]); iIndex++) {
		if (myTimerList[iIndex].hsSemaphore == -1 &&
			myTimerList[iIndex].pFunction == 0 && myTimerList[iIndex].pFunctionData == 0)
		{
			myTimerList[iIndex].hsSemaphore = hsSemaphore;
			myTimerList[iIndex].pFunctionData = pFunctionData;
			myTimerList[iIndex].pFunction = pFunction;
			break;
		}
	}

	if (iIndex >= sizeof(myTimerList)/sizeof(myTimerList[0])) {
		NHPOS_ASSERT_TEXT(iIndex < sizeof(myTimerList)/sizeof(myTimerList[0]), "**ERROR: BlAddThreadFunctionToTimerList() full list.");
		iIndex = -1;
	}
    return iIndex;
}

LONG BlRemoveThreadFunctionFromTimerList (LONG iIndex, SHORT  hsSemaphore, LONG (*pFunction)(VOID *pFunctionData), VOID *pFunctionData)
{
	if (iIndex < 0) {
		for (iIndex = 0; iIndex < sizeof(myTimerList)/sizeof(myTimerList[0]); iIndex++) {
			if (myTimerList[iIndex].hsSemaphore == -1 && myTimerList[iIndex].pFunction == 0 && myTimerList[iIndex].pFunctionData == 0)
				continue;
			if (myTimerList[iIndex].hsSemaphore == hsSemaphore ||
				(myTimerList[iIndex].pFunction == pFunction && myTimerList[iIndex].pFunctionData == pFunctionData))
			{
					break;
			}
		}

		if (iIndex >= sizeof(myTimerList)/sizeof(myTimerList[0])) {
			iIndex = -1;
		}
	}
	
	if (iIndex >= 0) {
		myTimerList[iIndex].hsSemaphore = -1;
		myTimerList[iIndex].pFunction = 0;
		myTimerList[iIndex].pFunctionData = 0;
	}

    return iIndex;
}

/*
*===========================================================================
** Synopsis:    BOOL BlDelayBalance(VOID)
*
*     Input:    pvData  - not use
*
*    Output:    nothing
*
** Return:      After we load up all the layouts and everything, we do a check
*				to see if the terminal is in delay balance, and then we 
*				do a call to display the sign in prompt, just to make sure
*				that it is showing correctly.
*===========================================================================
*/
__declspec(dllexport)
BOOL BlDelayBalance(VOID)
{
	UIFREGMODEIN myRegModeIn = {0};

	PifSleep(3000);          // BlDelayBalance() initialize business logic.

    PifCheckDelayBalance();

	myRegModeIn.uchMajorClass = CLASS_UIFREGMODE;  /* set mode in class */
    myRegModeIn.uchMinorClass = CLASS_UIREGMODEIN;

	ItemModeInDisplay(&myRegModeIn);

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    BOOL BlUninitialize(VOID *pvDummy)
*
*     Input:    pvData  - not use
*
*    Output:    nothing
*
** Return:      Reserved <not defined yet>
*===========================================================================
*/
__declspec(dllexport)
BOOL BlUninitialize(VOID *pvDummy)
{
    return FALSE;
}

/*
*===========================================================================
** Synopsis:    LONG BlNotifyData(CONST BL_NOTIFYDATA *pData, VOID *pResult)
*
*     Input:    pData   - Data to notify
*
*    Output:    pResult - Reserved <not defined yet>
*
** Return:      Reserved <not defined yet>
*===========================================================================
*/
__declspec(dllexport)
LONG BlNotifyData(CONST BL_NOTIFYDATA *pData, VOID *pResult)
{
    LONG lRet = -1;     /* return value to caller */

    extern LONG UiePutData(CONST _DEVICEINPUT *, VOID *);

    switch (pData->ulType) {
      case BL_INPUT_DATA:
          lRet = UiePutData(&(pData->u.Data), pResult);
          break;
      default:
			NHPOS_ASSERT_TEXT(0, "BlNotifyData(): Unknown ulType.");
          break;
    }

    return lRet;
}

/*
*===========================================================================
** Synopsis:    USHORT BlGetKeyboardLockPosition (VOID)
*
*     Input:    None
*
*    Output:    Nothing
*
** Return:      indicator as to current keyboard lock position.  One of
*                  PIF_NCRKLOCK_MODE_REG
*                  PIF_NCRKLOCK_MODE_SUP
*                  PIF_NCRKLOCK_MODE_PROG
*                  PIF_NCRKLOCK_MODE_LOCK
*                  PIF_NCRKLOCK_MODE_EX
*===========================================================================
*/
__declspec(dllexport)
USHORT BlGetKeyboardLockPosition (VOID)
{
	return UieGetKeyboardLockMode();
}

/*
*===========================================================================
** Synopsis:    ULONG BlRestrictButtonPressMask (int iOp, ULONG  ulMask)
*
*     Input:    int  iOp     - the operation code indicating the transformation on the restriction mask
*               ULONG ulMask - the mask with bits to be used int the transformation
*
*    Output:    Nothing
*
** Return:      copy of the button restrict mask before modification.
*
** Description: Button processing restrictions mask to enable/disable button pressing restrictions.
*               Button processing restrictions are used to limit allowed button presses when in a particular
*               mode because allowing the button would cause user interface problems.  This is not to be
*               use lightly but is intended for specific circumstances such as when processing an OEP
*               window and we want to make sure the user can not cover the OEP window with another window
*               rendering the window unavailable.
*===========================================================================
*/
__declspec(dllexport)
ULONG BlRestrictButtonPressMask (int iOp, ULONG  ulMask)
{
	static ULONG  m_ulButtonRestrictMask = 0;   // bit map for restricted button functionality.
	ULONG         ulCurrentMask = m_ulButtonRestrictMask;

	switch (iOp) {
		case BL_BUTTONRESTRICT_OP_READEXISTING:
			// if iOp is zero then nothing to do, just return current value
			break;
		case BL_BUTTONRESTRICT_OP_ENABLE:
			// indicates to bitwise Or ulMask with existing restrictions mask
			// this will enable what ever restrictions are turned on in the mask
			m_ulButtonRestrictMask |= ulMask;
			break;
		case BL_BUTTONRESTRICT_OP_DISABLEEXCEPT:
			// indicates to bitwise And ulMask with existing restrictions mask
			// this will disable all restrictions except for the ones in the mask
			m_ulButtonRestrictMask &= ulMask;
			break;
		case BL_BUTTONRESTRICT_OP_TOGGLE:
			// indicates to bitwise Xor ulMask with existing restrictions mask
			// this will toggle restictions specified in the mask
			//    enable -> disable   or   disable -> enable
			m_ulButtonRestrictMask ^= ulMask;
			break;
		case BL_BUTTONRESTRICT_OP_DISABLE:
			// indicates to bitwise Not then And ulMask with existing restrictions mask
			// this will turn off restictions specified in the mask
			//    enable -> disable   or   disable -> disable
			m_ulButtonRestrictMask &= ~ulMask;
			break;
		default:
			// if iOp is unknown then nothing to do, just return current value
			NHPOS_ASSERT_TEXT(0, "==WARNING: BlRestrictButtonPressMask() iOp unknown.");
			break;
	}

	return ulCurrentMask;
}

/*
*===========================================================================
** Synopsis:    SHORT BlUifSupIntervent (USHORT usClear)
*
*     Input:    usClear  indicates operation to perform
*                  BLI_SUPR_INTV_READ  -  read the current setting
*                  BLI_SUPR_INTV_CLEAR -  clear the current setting (reset)
*                  BLI_SUPR_INTV_SET   -  set the current setting (enable intervention)
*
*    Output:    Nothing
*
** Return:      indicator as to current Supervisor Intervention setting.
*                     returns              if UifSupInterventRetStatus is
*                  BLI_SUPR_INTV_APPROVED  -> UIF_SUCCESS
*                  BLI_SUPR_INTV_CLEARED   -> UIFREG_CLEAR
*                  BLI_SUPR_INTV_FAILED    -> UIFREG_ABORT
*===========================================================================
*/
__declspec(dllexport)
SHORT BlUifSupIntervent (USHORT usClear)
{
	SHORT  sRetStatus;

	/*
		The global variable UifSupInterventRetStatus contains the following values:
			UIFREG_ABORT ->
			UIFREG_CLEAR ->
			UIF_SUCCESS -> Good Supervisor number entered, intervention successful.
	*/
	sRetStatus = UifSupInterventRetStatus;
	if (sRetStatus == UIF_SUCCESS) {
		sRetStatus = BLI_SUPR_INTV_APPROVED;
	}
	else if (sRetStatus == UIFREG_CLEAR) {
		sRetStatus = BLI_SUPR_INTV_CLEARED;
	}
	else {
		sRetStatus = BLI_SUPR_INTV_FAILED;
	}
	if (usClear == BLI_SUPR_INTV_CLEAR) {
		UifSupInterventRetStatus = UIFREG_CLEAR;
	}
	else if (usClear == BLI_SUPR_INTV_SET) {
		UifSupInterventRetStatus = UIF_SUCCESS;
	}

	return sRetStatus;
}


/*
*===========================================================================
** Synopsis:    USHORT BlGetNoticeBoardStatus()
*
*     Input:    Nothing
*
*    Output:    Nothing
*
** Return:      USHORT bit mask indicating the current status of the
*               notice board subsystem.
*
** Description: This function checks the Notice Board status to determine
*               the terminal is operating as a
*===========================================================================
*/
USHORT BlIntrnlGetNoticeBoardStatus (int iTestComs)
{
    USHORT      fsComStatus = 0;
    USHORT      fsServerFlag = 0;
	SYSCONFIG CONST * pSysConfig = PifSysConfig();    // Get pointer to System Configuration info
    NBMESSAGE   NbInf;

	// Check the current Notice Board status bits to determine
	// if the Master terminal and the Backup Master terminal status
	// is up to date.  We also check to see if the Master and Backup
	// Master are off-line or not.
    NbReadAllMessage(&NbInf);

    if (NbInf.fsSystemInf & NB_MTUPTODATE) {
        fsComStatus |= BLI_STS_M_UPDATE;
    }
    if (NbInf.fsSystemInf & NB_BMUPTODATE) {
        fsComStatus |= BLI_STS_BM_UPDATE;
    }
    if (0 == (NbInf.fsSystemInf & NB_SETMTONLINE)) {
        fsComStatus |= BLI_STS_M_OFFLINE;
    }
    if (0 == (NbInf.fsSystemInf & NB_SETBMONLINE)) {
        fsComStatus |= BLI_STS_BM_OFFLINE;
    }
    if (0 == (fsComStatus & (BLI_STS_M_UPDATE | BLI_STS_BM_UPDATE))) {
        fsComStatus |= BLI_STS_M_UPDATE;
    }

	// Determine if we are a Master terminal or a Backup Master terminal.
	// If we are then we need to check the synchronization status.
	// If we are not, then we are a Satellite terminal so lets just see
	// if Notice Board is indicating that we need to stop.
	// We determine type of terminal by looking at the IP address in the
	// System Configuration data to see if we are terminal 1 or terminal 2.
	//     CLI_TGT_MASTER == 1
	//     CLI_TGT_BMASTER == 2
	if (pSysConfig->auchLaddr[3] == 1 || pSysConfig->auchLaddr[3] == 2) {
		SstReadFlag(&fsServerFlag);
		if (fsServerFlag & SER_SPESTS_INQUIRY) {
			fsComStatus |= BLI_STS_INQUIRY;
		}
		if (fsServerFlag & SER_SPESTS_INQTIMEOUT) {
			fsComStatus |= BLI_STS_INQTIMEOUT;
		}
	}
	else {
        if (fsComStatus & BLI_STS_M_UPDATE) {
            if (NbInf.fsMBackupInf & NB_STOPALLFUN) { 
                fsComStatus |= BLI_STS_STOP;
            }
        } else if (fsComStatus & BLI_STS_BM_UPDATE) {
            if (NbInf.fsBMBackupInf & NB_STOPALLFUN) {
                fsComStatus |= BLI_STS_STOP;
            }
        }
	}

	if (1 == usBMOption) {
		fsComStatus |= BLI_STS_BACKUP;
	}

	{
		USHORT  usCstComReadStatus = 0;
		usCstComReadStatus = CstComReadStatus();    // read the Notice Board system status for CstSendMaster()
		if ((fsComStatus & BLI_STS_M_OFFLINE) != 0 && iTestComs) {
			if ((usCstComReadStatus & BLI_STS_M_REACHABLE) == 0) {
				CliReqSystemStatus(&usCstComReadStatus);
			}
		}
		if (usCstComReadStatus & BLI_STS_M_REACHABLE) {
			fsComStatus |= BLI_STS_M_REACHABLE;
		}
	}
	return fsComStatus;
}

__declspec(dllexport)
USHORT BlGetNoticeBoardStatus (int iTestComs)
{
	return BlIntrnlGetNoticeBoardStatus(iTestComs);
}

__declspec(dllexport)
SHORT _cdecl BlSubmitFileToFilePrint (TCHAR *aszFileName)
{
	UifRequestSharedPrintFile (aszFileName);
	return 0;
}

BUSINESSLOGIC_API SHORT _cdecl BlProcessEmployeeChange (EMPLOYEECHANGE  *pEmployeeChange)
{
	SHORT    sRetStatus;
	TCHAR    *EmployeeListActionType = _T("Employee-Data");

	if (0 == _tcsncmp (pEmployeeChange->tcsActionType, EmployeeListActionType, _tcslen(EmployeeListActionType)) ) {
		ETK_DETAILS  tempArray[40] = {0};
		SHORT        sStatus = 0;

		sStatus = EtkCurAllLoggedInRead(sizeof(tempArray)/sizeof(tempArray[0]), tempArray);

		if (sStatus < 0) {
			NHPOS_ASSERT_TEXT((sStatus >= 0), "BlProcessEmployeeChange(): EtkCurAllLoggedInRead() returned error.");
			sStatus = 0;
		}

		if (sStatus >= 0) {
			ConnEngineSendEmployeeDataField (EmployeeListActionType, tempArray, sStatus);
		}
	}
	else {
		ETK_JOB  EtkJob = {0};
		TCHAR    aszEmployeeName[21] = {0};

		sRetStatus = EtkIndJobRead(pEmployeeChange->ulEmployeeId, &EtkJob, aszEmployeeName);

		if (sRetStatus == ETK_SUCCESS) {
			if (pEmployeeChange->usJobCode != 0)
				EtkJob.uchJobCode1 = pEmployeeChange->usJobCode;
			
			if (pEmployeeChange->tcsMnemonic[0] != 0) {
				_tcsncpy (aszEmployeeName, pEmployeeChange->tcsMnemonic, 20);
				aszEmployeeName[20] = 0;   // make sure end of string is in place.
			}

			sRetStatus = EtkAssign(pEmployeeChange->ulEmployeeId, &EtkJob, aszEmployeeName);

			if (sRetStatus == SUCCESS) {
				CASIF   CasIf = {0};
				SHORT   sError;

				CasIf.ulCashierNo = pEmployeeChange->ulEmployeeId;
				sError = SerCasIndRead(&CasIf);

				if (sError == CAS_PERFORM) {
					if (pEmployeeChange->tcsMnemonic[0] != 0) {
						_tcsncpy (CasIf.auchCashierName, pEmployeeChange->tcsMnemonic, 20);
						aszEmployeeName[20] = 0;   // make sure end of string is in place.
					}

					sRetStatus = SerCasAssign(&CasIf);
				}
			}
		}
	}

	return 0;
}

/********************************************
*
* Description:  Handle a BL_CONNENGINE_MSG_GUESTCHECK_RTRV directive from Connection Engine
*               by retrieving the information requested and sending it back out through
*               Connection Engine.
*********************************************/
BUSINESSLOGIC_API SHORT _cdecl BlProcessGuestCheck_Rtrv (GUESTCHECK_RTRV  *pGuestCheck_Rtrv)
{
	TCHAR    aszConnEngTmpFileName[]   = _T("CONNTMP");
	USHORT   usGCFNo;
	SHORT    hsFileHandle;
	USHORT   usSize = 64000;
    SHORT    sStatus, sGcfStatus = 0;
	ULONG    ulActualBytesRead = 0;

	usGCFNo = pGuestCheck_Rtrv->gcnum;

	if (pGuestCheck_Rtrv->gcnum > 0) {
		hsFileHandle = PifOpenFile(aszConnEngTmpFileName, auchTEMP_NEW_FILE_READ_WRITE);    /* saratoga */
		if (hsFileHandle == PIF_ERROR_FILE_EXIST) {
			PifDeleteFile(aszConnEngTmpFileName);
			hsFileHandle = PifOpenFile(aszConnEngTmpFileName, auchTEMP_NEW_FILE_READ_WRITE);    /* saratoga */
		}

		sGcfStatus = CliGusIndReadFH (usGCFNo, hsFileHandle, sizeof(USHORT) + sizeof(USHORT), usSize, &ulActualBytesRead);
		sStatus = ConnEngineSendTransactionFH (0, sGcfStatus, usGCFNo, hsFileHandle, sizeof(USHORT) + sizeof(USHORT), ulActualBytesRead);

		PifCloseFile (hsFileHandle);
	}
	else {
		GCF_STATUS_HEADER Gcf_FileHed;
		GCF_STATUS_STATE  RcvBuffer[GCF_MAX_GCF_NUMBER + 1];
		USHORT            usRcvMaxCount;
		SHORT             sRcvActualCount;

		usRcvMaxCount = sizeof(RcvBuffer)/sizeof(RcvBuffer[0]);
		sRcvActualCount = GusReadAllGuestChecksWithStatus (&RcvBuffer[0], usRcvMaxCount, &Gcf_FileHed);
		// If there was an error then just send back an empty list.
		if (sRcvActualCount < 0)
			sRcvActualCount = 0;
		ConnEngineSendGuestCheckList (RcvBuffer, sRcvActualCount, &Gcf_FileHed, pGuestCheck_Rtrv->gcnum);
	}

	return(sStatus);                                            /* success */
}

/********************************************
*
* Description:  Handle a BL_CONNENGINE_MSG_CASHIERACTION directive from Connection Engine
*               by processing the information requested, creating the kind of Cashier Action
*               requested.  The function that is called to execute the Cashier Action will
*               send a response back out.
*********************************************/
static TCHAR  *CashierActionErrorText[] = {
	_T("1000 - Invalid Total/Tender key indicated."),
	_T("1001 - Unable to lock environment."),
	_T("1002 - Invalid employee id.")
};

TCHAR  * BlConnectionEngineErrorMessage (SHORT  sRetStatus)
{
	static TCHAR  tcsErrorMessage[62];
	SHORT   sErrorTextIndex;

	memset(tcsErrorMessage, 0, sizeof(tcsErrorMessage));

	if (sRetStatus >= 1000) {
		sErrorTextIndex = sRetStatus - 1000;
		if (sErrorTextIndex < sizeof(CashierActionErrorText)/sizeof(CashierActionErrorText[0]))
			_tcsncpy(tcsErrorMessage, CashierActionErrorText[sErrorTextIndex], 60);
		else
			_tcsncpy(tcsErrorMessage, _T("See error code."), 60);
	} else if (sRetStatus > 0 && sRetStatus < MAX_LEAD_NO) {
		RflGetLead (tcsErrorMessage, sRetStatus);
	} else if (sRetStatus == 0) {
		_tcsncpy(tcsErrorMessage, _T("Success."), 60);
	} else {
		_tcsncpy(tcsErrorMessage, _T("See error code."), 60);
	}

	return tcsErrorMessage;
}
BUSINESSLOGIC_API SHORT _cdecl BlProcessCashierAction (CASHIERACTION  *pCashierAction)
{
    SHORT            sRetStatus = 0;

    if (_tcscmp (pCashierAction->tcsActionType, CONNENGINE_ACTIONTYPE_PAIDOUT) == 0) {    /* paid out ? */ 
		UIFREGMISC       UifRegData = {0};

		UifRegData.uchMajorClass = CLASS_UIFREGMISC;             /* set single transaction class */
        UifRegData.uchMinorClass = CLASS_UIPO;
		UifRegData.lAmount = pCashierAction->lAmount;            /* set input amount */
		_tcsncpy (UifRegData.aszNumber, pCashierAction->tcsReference, STD_NUMBER_LEN);
		UifRegData.ulEmployeeNo = pCashierAction->ulEmployeeId;  /* set input employee id */
        sRetStatus = ItemMiscPO (&UifRegData, 0);                /* Misc Module */
		if (sRetStatus != UIF_FINALIZE) {
			ConnEngineSendCashierActionWithError (pCashierAction->tcsActionType, 0, sRetStatus, BlConnectionEngineErrorMessage(sRetStatus));
		}
    } else if (_tcscmp (pCashierAction->tcsActionType, CONNENGINE_ACTIONTYPE_RECEIVEACCOUNT) == 0) {  /* receive on account ? */ 
		UIFREGMISC       UifRegData = {0};

		UifRegData.uchMajorClass = CLASS_UIFREGMISC;             /* set single transaction class */
        UifRegData.uchMinorClass = CLASS_UIRA;
		UifRegData.lAmount = pCashierAction->lAmount;            /* set input amount */
		_tcsncpy (UifRegData.aszNumber, pCashierAction->tcsReference, STD_NUMBER_LEN);
		UifRegData.ulEmployeeNo = pCashierAction->ulEmployeeId;  /* set input employee id */
        sRetStatus = ItemMiscRA (&UifRegData, 0);                /* Misc Module */
		if (sRetStatus != UIF_FINALIZE) {
			ConnEngineSendCashierActionWithError (pCashierAction->tcsActionType, 0, sRetStatus, BlConnectionEngineErrorMessage(sRetStatus));
		}
    } else if (_tcscmp (pCashierAction->tcsActionType, CONNENGINE_ACTIONTYPE_LOAN) == 0) {  /* loan ? */ 
		MAINTLOANPICKUP  LoanPickupData = {0};

		LoanPickupData.uchMajorClass = CLASS_MAINTLOAN;              /* Set Major Class */
		LoanPickupData.uchMinorClass = pCashierAction->usTotalId;    /* Set Minor Class */
        LoanPickupData.lAmount = pCashierAction->lAmount;
		if (pCashierAction->usVoidIndic)
			LoanPickupData.usModifier |= MAINT_MODIFIER_VOID;
		LoanPickupData.uchLoanPickStatus |= MAINT_DATA_INPUT_CE;                    // indicate that the loan is from Connection Engine hence no user interface actions
		LoanPickupData.ulCashierNo = pCashierAction->ulEmployeeId;                  /* set input employee id */
		LoanPickupData.HeadTrail.ulCashierNo = pCashierAction->ulEmployeeId;        /* set input employee id */

		if (pCashierAction->usTotalId < 0xff) 
			sRetStatus = ItemMiscLoanPickup (&LoanPickupData);         /* Misc Module */
		else
			sRetStatus = 1000;

		if (sRetStatus != SUCCESS) {
			ConnEngineSendCashierActionWithError (pCashierAction->tcsActionType, 0, sRetStatus, BlConnectionEngineErrorMessage(sRetStatus));
		}
    } else if (_tcscmp (pCashierAction->tcsActionType, CONNENGINE_ACTIONTYPE_PICKUP) == 0) {  /* pickup ? */ 
		MAINTLOANPICKUP  LoanPickupData = {0};

		LoanPickupData.uchMajorClass = CLASS_MAINTPICKUP;              /* Set Major Class */
		LoanPickupData.uchMinorClass = pCashierAction->usTotalId;    /* Set Minor Class */
        LoanPickupData.lAmount = pCashierAction->lAmount;
		if (pCashierAction->usVoidIndic)
			LoanPickupData.usModifier |= MAINT_MODIFIER_VOID;
		LoanPickupData.uchLoanPickStatus |= MAINT_DATA_INPUT_CE;                    // indicate that the loan is from Connection Engine hence no user interface actions
		LoanPickupData.ulCashierNo = pCashierAction->ulEmployeeId;  /* set input employee id */

		if (pCashierAction->usTotalId < 0xff) 
			sRetStatus = ItemMiscLoanPickup (&LoanPickupData);         /* Misc Module */
		else
			sRetStatus = 1000;

		if (sRetStatus != SUCCESS) {
			ConnEngineSendCashierActionWithError (pCashierAction->tcsActionType, 0, sRetStatus, BlConnectionEngineErrorMessage(sRetStatus));
		}
	} else {
		char xBuff[128] = {0};
		sprintf (xBuff, "Unknown type %S", pCashierAction->tcsActionType);
		NHPOS_ASSERT_TEXT((pCashierAction->tcsActionType[0] == 0xFFFF), xBuff);
		sRetStatus = -1;  // an error
		ConnEngineSendCashierActionWithError (pCashierAction->tcsActionType, 0, sRetStatus, _T("Action Type not allowed."));
	}

	return(sRetStatus);                                            /* success */
}

BUSINESSLOGIC_API SHORT _cdecl BlProcessEjEntryAction (EJENTRYACTION  *pEjEntryAction)
{
    SHORT            sRetStatus = 0;

	if (_tcscmp (pEjEntryAction->tcsActionType, _T("first")) == 0) {
		sRetStatus = ConnEngineEjEntryFirst ();
	} else if (_tcscmp (pEjEntryAction->tcsActionType, _T("next")) == 0) {
		sRetStatus = ConnEngineEjEntryNext (pEjEntryAction->ulFilePositionHigh, pEjEntryAction->ulFilePositionLow);
	} else if (_tcscmp (pEjEntryAction->tcsActionType, _T("clear")) == 0) {
		sRetStatus = ConnEngineEjEntryClear (pEjEntryAction->ulFilePositionHigh, pEjEntryAction->ulFilePositionLow);
	} else {
		sRetStatus = ConnEngineSendResponseWithError (_T("EJENTRYACTION"), pEjEntryAction->tcsActionType, 0, -1, _T("Illegal action type specified."));
	}
	return sRetStatus;
}

BUSINESSLOGIC_API SHORT _cdecl BlProcessMacroAction (MACROACTION  *pMacroAction)
{
    SHORT   sRetStatus = -1;
    CSTRIF  WorkCstr = {0};

	WorkCstr.usCstrNo = pMacroAction->usStringNumber;
	ConnEngineSendResponseWithError (_T("MACROACTION"), 0, 0, -1, _T("Unsupported at this time."));

#if 0
	if (pMacroAction->usStringNumber < 1) {
		sRetStatus = -1;
		ConnEngineSendResponseWithError (_T("MACROACTION"), 0, 0, -1, _T("Illegal string number specified."));
	} else if (CliOpCstrIndRead(&WorkCstr, 0) != OP_PERFORM) {
		sRetStatus = -2;
		ConnEngineSendResponseWithError (_T("MACROACTION"), 0, 0, -2, _T("Error accessing string."));
    } else {
        if (WorkCstr.IndexCstr.usCstrLength == 0) {
			sRetStatus = -3;
			ConnEngineSendResponseWithError (_T("MACROACTION"), "run-string", 0, -3, _T("String has no commands."));
		} else {
			int i=0;
			CHARDATA	    CharData;
			BL_NOTIFYDATA   BlNotify;

			BlNotify.ulType = BL_INPUT_DATA;
			BlNotify.u.Data.ulDeviceId = BL_DEVICE_DATA_KEY;
			BlNotify.u.Data.ulLength = sizeof(CharData);
			BlNotify.u.Data.pvAddr = &CharData;

			memset (&CharData, 0, sizeof(CharData));
			CharData.uchFlags = CHARDATA_FLAGS_TOUCHUTTON;

			//if a number is pressed before this button
			//we will need to clear out the ring buffer
			//so that no information is sent prior to this
			//
			CharData.uchASCII = FSC_CNT_STRING_GUARD_ASCII;
			CharData.uchScan = FSC_CNT_STRING_GUARD_SCAN;
			BlNotifyData(&BlNotify, NULL);

			{
				char   uchPage[16];
				char   *pData = &(uchPage[0]);

				_itoa(pMacroAction->usStringNumber, pData, 10);
				for ( ; *pData; pData++){
					if (*pData >= '0' && *pData <= '9')
					{
						CharData.uchASCII = *pData;
						BlNotifyData(&BlNotify, NULL);
					}
				}
			}

			CharData.uchASCII = 0x41;
			CharData.uchScan = 0x00;
			BlNotifyData(&BlNotify, NULL);

			sRetStatus = 0;
		}
	}
#endif
	return sRetStatus;
}

extern  int DataQueryStartParsing (TCHAR *tcBuffer, DataQueryObject *pDataQueryObject);

static TCHAR tcsMegaBigBufferDataQuery [64000];
static TCHAR *pMegaBigBufferDataQuery = tcsMegaBigBufferDataQuery;

// Function BlProcessDataQuery() to process a BL_CONNENGINE_MSG_DATAQUERY message
// from the Connection Engine Interface.
BUSINESSLOGIC_API SHORT _cdecl BlProcessDataQuery(DATAQUERY  *pDataQuery)
{
	int    iRetStatus = -1;
	SHORT  sRetStatus = -1;
	DataQueryObject myObject = {0};

	myObject.ptcsBuffer = pMegaBigBufferDataQuery;
	myObject.ulBufferSize = 16000;

	if (_tcslen(pDataQuery->tcsQueryStatement) < 250) {
		iRetStatus = DataQueryStartParsing (pDataQuery->tcsQueryStatement, &myObject);
		sRetStatus = 0;
		if (iRetStatus < 0) {
			sRetStatus = -2;
		} else {
			ConnEngineSendDataQueryResult (pDataQuery->tcsActionType, myObject.ptcsBuffer);
		}
	}

	if (sRetStatus < 0) {
		ConnEngineSendResponseWithError (_T("DATAQUERY"), 0, 0, -5, _T("Error with query."));
	}

	return sRetStatus;
}

static struct {
	TCHAR         tcsServiceName[48];
	ULONG         ulCurrentEvent;
	STATECHANGE   StateChange;
} BlStateChangeRegistery[10] = { {0} };

BOOL BlRegisterStateChangeService (CONST TCHAR *tcsServiceName )
{
	int iLoop;
	for (iLoop = 0; iLoop < sizeof(BlStateChangeRegistery)/sizeof(BlStateChangeRegistery[0]); iLoop++) {
		if (BlStateChangeRegistery[iLoop].tcsServiceName[0] == 0) {
			_tcsncpy (BlStateChangeRegistery[iLoop].tcsServiceName, tcsServiceName, 44);
			break;
		}
	}

	return iLoop < sizeof(BlStateChangeRegistery)/sizeof(BlStateChangeRegistery[0]);
}

BOOL BlRegisterStateChangeEvent (CONST TCHAR *tcsServiceName, ULONG ulEventId )
{
	int iLoop;
	for (iLoop = 0; iLoop < sizeof(BlStateChangeRegistery)/sizeof(BlStateChangeRegistery[0]); iLoop++) {
		if (BlStateChangeRegistery[iLoop].tcsServiceName[0]) {
			if (_tcscmp (BlStateChangeRegistery[iLoop].tcsServiceName, tcsServiceName) == 0) {
				BlStateChangeRegistery[iLoop].ulCurrentEvent = ulEventId;
				memset (&(BlStateChangeRegistery[iLoop].StateChange), 0, sizeof(BlStateChangeRegistery[iLoop].StateChange));
				break;
			}
		}
	}

	return iLoop < sizeof(BlStateChangeRegistery)/sizeof(BlStateChangeRegistery[0]);
}

int BlSetStateChangeEvent (CONST TCHAR *tcsServiceName, ULONG ulEventId, CONST TCHAR *tcsActionType, USHORT usNextState, STATECHANGE  *pStateChange )
{
	int iRetStatus = -1;
	int iLoop;
	for (iLoop = 0; iLoop < sizeof(BlStateChangeRegistery)/sizeof(BlStateChangeRegistery[0]); iLoop++) {
		if (BlStateChangeRegistery[iLoop].tcsServiceName[0]) {
			if (_tcscmp (BlStateChangeRegistery[iLoop].tcsServiceName, tcsServiceName) == 0) {
				if (BlStateChangeRegistery[iLoop].ulCurrentEvent == ulEventId) {
					if (pStateChange) {
						BlStateChangeRegistery[iLoop].StateChange = *pStateChange;
					}
					if (tcsActionType) {
						int iLen = TCHARSIZEOF(BlStateChangeRegistery[iLoop].StateChange.tcsActionType);
						_tcsncpy (BlStateChangeRegistery[iLoop].StateChange.tcsActionType, tcsActionType, iLen);
						iLen--;
						if (iLen >= 0) {
							BlStateChangeRegistery[iLoop].StateChange.tcsActionType[iLen] = 0;  // ensure end of string
						}
						BlStateChangeRegistery[iLoop].StateChange.usStateCount = usNextState;
					}
					iRetStatus = 0;
					if (BlStateChangeRegistery[iLoop].StateChange.tcsActionType[0])
						iRetStatus = 1;
					break;
				}
			}
		}
	}

	return iRetStatus;
}

int BlCheckStateChangeEvent (CONST TCHAR *tcsServiceName, ULONG ulEventId, STATECHANGE  *pStateChange )
{
	int iRetStatus = -1;
	int iLoop;
	for (iLoop = 0; iLoop < sizeof(BlStateChangeRegistery)/sizeof(BlStateChangeRegistery[0]); iLoop++) {
		if (BlStateChangeRegistery[iLoop].tcsServiceName[0]) {
			if (_tcscmp (BlStateChangeRegistery[iLoop].tcsServiceName, tcsServiceName) == 0) {
				if (BlStateChangeRegistery[iLoop].ulCurrentEvent == ulEventId) {
					if (pStateChange) {
						*pStateChange = BlStateChangeRegistery[iLoop].StateChange;
					}
					iRetStatus = 0;
					if (BlStateChangeRegistery[iLoop].StateChange.tcsActionType[0])
						iRetStatus = 1;
					break;
				}
			}
		}
	}
// Start the message loop. 
	{
		MSG msg;

		while(PeekMessage(&msg, NULL,  0, 0, PM_NOREMOVE))
		{
			BOOL bRet;

			bRet = GetMessage(&msg, NULL, 0, 0 );
			if (bRet == -1)
			{
				// handle the error and possibly exit
				break;
			}
			else
			{
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			}
		}
	}
	return iRetStatus;
}


// Function BlProcessStateChange() to process a BL_CONNENGINE_MSG_STATECHANGE message
// from the Connection Engine Interface.
BUSINESSLOGIC_API SHORT _cdecl BlProcessStateChange(STATECHANGE  *pStateChange)
{
	int iLoop;
	SHORT  sRetStatus = -1;

	for (iLoop = 0; iLoop < sizeof(BlStateChangeRegistery)/sizeof(BlStateChangeRegistery[0]); iLoop++) {
		if (BlStateChangeRegistery[iLoop].tcsServiceName[0]) {
			if (_tcscmp (BlStateChangeRegistery[iLoop].tcsServiceName, pStateChange->tcsService) == 0) {
				if (BlStateChangeRegistery[iLoop].ulCurrentEvent == pStateChange->uleventid) {
					BlStateChangeRegistery[iLoop].StateChange = *pStateChange;
					sRetStatus = 0;
					break;
				}
			}
		} else {
			break;
		}
	}
	return sRetStatus;
}

BUSINESSLOGIC_API SHORT _cdecl BlProcessTransaction (TRANSACTION  *pTransaction)
{
	if (ConnEngineSendRequestTransactionStatus (-1, 0) != 0) {
		// some other functionality is waiting on a transaction message
		// we will indicate that a transaction is available along with the
		// error code that was indicated.
		ConnEngineSendRequestTransactionStatus (2, &(pTransaction->lErrorCode));
	} else {
		TCHAR  tcsUniqueIdentifier[48] = {0};
		NHPOS_ASSERT_TEXT(0, "BlProcessTransaction(): No outstanding Request.");
		UifConvertUniqueIdReturningString (tcsUniqueIdentifier, pTransaction->uchUniqueIdentifier);
		ConnEngineSendResponseWithError (_T("TRANSACTION"), 0, tcsUniqueIdentifier, -5, _T("No pending transaction request."));
	}
	return 0;
}

// we have received a message through our Connection Engine Interface that needs to be
// forwarded to another terminal.
BUSINESSLOGIC_API SHORT _cdecl BlProcessConnEngine (AllObjects  *pAllObjects)
{
	switch (pAllObjects->ConnEngine.iObjectType) {
		case BL_CONNENGINE_MSG_FRAMEWORK:              // BlProcessConnEngine (AllObjects  *pAllObjects) - process message
			CliOpConnEngineObject (pAllObjects);
			break;

		case BL_CONNENGINE_MSG_EJENTRYACTION:          // BlProcessConnEngine (AllObjects  *pAllObjects) - process message
			CliOpConnEngineObject (pAllObjects);
			break;

		case BL_CONNENGINE_MSG_TRANSACTION:            // BlProcessConnEngine (AllObjects  *pAllObjects) - process message
			CliOpConnEngineObject (pAllObjects);
			break;

		default:
			NHPOS_ASSERT_TEXT(0, "BlProcessConnEngine(): Unsupported object type in forward request");
			ConnEngineSendResponseWithError (_T("CONNENGINE"), 0, 0, -3, _T("Illegal object type specified."));
			break;
	}
	return 0;
}

/*
*===========================================================================
** Synopsis:    BOOL BlSetFrameworkFunc()
*
*     Input:    dwCommand	- function command
*				pFunc       - ptr. to call back function
*
*    Output:    nothing
*
** Return:      result code
*===========================================================================
*/
__declspec(dllexport)
BOOL BlSetFrameworkFunc(ULONG ulCommand, VOID* pFunc)
{
	/* save call back function address */

	switch(ulCommand){
	case CMD_DEVICEENGINE_FUNC:
		s_pfnBlInterface = (BLCBPROC)pFunc;
		break;
	/* Security CALLBACKS if we're using VBOLOCK
	   then the softlocx callbacks must be set to NULL
	   and vice versa --TLDJR */
	// Softlocx Security
	case CMD_AXCONTROL_FUNC:
		s_pfnBlAXInterface = (BLCBPROC)pFunc;
		break;
	case CMD_AXCONTROL_EXP:
		s_pfnBlAXExp = (BLAXPROC)pFunc;
		break;
	case CMD_AXCONTROL_ST:
		s_pfnBlAXSt = (BLAXPROC)pFunc;
		break;
	case CMD_FWRELOADLAY_FUNC:
		s_pfnBlInterfaceLay = (BLAYPROC)pFunc;
		break;
	case CMD_SET_OEP_GROUPS:
		s_pfnBlSetOepGroups = (BOEPPROC)pFunc;
		break;
	case CMD_DISPLAY_OEP_IMAGE:
		s_pfnBlDisplayOEPImagery = (BOEPIMAGE)pFunc;
		break;
	case CMD_CALL_NAG_SCREEN:
		s_pfnBlNagScreen = (BLAYPROC)pFunc;
		break;
	case CMD_FWRELOADOEP_FUNC:
		s_pfnBlInterfaceOep = (BLAYPROC)pFunc;
		break;
	case CMD_GET_MAX_OEP_BUTTONS:
		s_pfnBlGetMaxOepButtons = (BOEPPROC2)pFunc;
		break;
	case CMD_AUTO_SIGN_OUT:
		s_pfnBlSetAutoSignOutTime = (BTIMEOUT)pFunc;
		break;
	case CMD_FORCE_SHUTDOWN:
		s_pfnBlForceShutdown = (BLAYPROC)pFunc;
		break;
	case CMD_FRAMEWORK_MSG:
		s_pfnBlInterfaceFrameworkMessage = (BMSGPROC)pFunc;
		break;
	default:
		return FALSE;
		break;
	}

	return TRUE;
}

/*
*===========================================================================
** Synopsis:    BOOL BlSendKeyboardData(CONST BL_NOTIFYDATA *pData, VOID *pResult)
*
*     Input:    BL_NOTIFYDATA* - Contains the character data and info of type of data
				Void* - NULL
*
*    Output:
*
** Return:      UIE_ERROR - buffer full
				UIE_SUCCESS - data entered to buffer

		Added to support TOUCHSCREEN
*===========================================================================
*/
__declspec(dllexport)
LONG BlSendKeyboardData(CONST BL_NOTIFYDATA *pData, VOID *pResult)
{
	/*Send WM_CHAR data to Business Logic From Framework
	so the data input is used for alpha numeric entry*/

	LONG	lRet;

	lRet = UiePutData(&pData->u.Data, pResult);

	return lRet;
}

/*
*===========================================================================
** Synopsis:    BOOL BlSendKeyboardData(CONST BL_NOTIFYDATA *pData, VOID *pResult)
*
*     Input:    BL_NOTIFYDATA* - Contains the character data and info of type of data
				Void* - NULL
*
*    Output:
*
** Return:      UIE_ERROR - buffer full
				UIE_SUCCESS - data entered to buffer

		Added to support TOUCHSCREEN
*===========================================================================
*/
__declspec(dllexport)
LONG BlSendKeybrdData(CONST BL_NOTIFYDATA *pData, VOID *pResult)
{
	LONG lreturn = 0;

	if (ItemCommonGetStatus(COMMON_PROHIBIT_SIGNOUT) != 0) {
		// Sign-out is not prohibitted so lets send the keyboard data.
		lreturn = BlSendKeyboardData(pData, pResult);
	}
		
	return lreturn;
}


/*
*===========================================================================
** Synopsis:    SHORT BlCheckAndSetTerminalLock(VOID)
*
*     Input:    NONE
*
*    Output:    USHORT usHandle
*
** Return:       usHandle
				UIC_NOT_IDLE

  Check terminal lock flag, if idle then set "Executing ".

		Added to support TOUCHSCREEN
*
** Description: Check terminal lock flag, if idle then set "Executing ".
*               Check and set the keyboard and transaction lock.  this lock is used
*               to ensure that Cashier can not Sign-in if there is a remote application
*               that has locked the keyboard for actions that must not be done in
*               parallel to Cashier actions.
*
*               WARNING: if the lock works, you must use BlResetTerminalLock() to unlock.
*                        we have seen mysterious "During Lock" (error 22) error
*                        dialogs displayed under some circumstance which were due
*                        to a successful lock that was not reset when an error
*                        occurred causing the action to be aborted or rejected.
*===========================================================================
*/
__declspec(dllexport)
SHORT BlCheckAndSetTerminalLock(VOID)
{
	SHORT sTermLockHandle;

	sTermLockHandle = UicCheckAndSet();  //check to see if idle
	return sTermLockHandle;
}

/*
*===========================================================================
** Synopsis:    USHORT BlResetTerminalLock(USHORT usTermLockHandle)
*
*     Input:    USHORT usTermLockHandle
*
*    Output:    USHORT usHandle
*
** Return:

  Interface for Framework to BusinessLogic in order to Lock the terminal

		Added to support TOUCHSCREEN
*===========================================================================
*/
__declspec(dllexport)
SHORT BlResetTerminalLock(SHORT sTermLockHandle)
{
	SHORT sResult;

	sResult = UicResetFlag(sTermLockHandle);
	return sResult;

}

/*
*===========================================================================
** Synopsis:    UCHAR BlFwIfReadMode(VOID)
*
*     Input:    NONE
*
*    Output:    UCHAR uchMode
*
** Return:       uchMode
*
*				UIE_POSITION0       0               P-2 mode
*				UIE_POSITION1       1               power off mode
*				UIE_POSITION2       2               lock mode
*				UIE_POSITION3       3               register mode
*				UIE_POSITION4       4               supervisor mode
*				UIE_POSITION5       5               program mode
*
*
*			Check the current mode that the terminal is in
*			Added to support TOUCHSCREEN
*===========================================================================
*/
__declspec(dllexport)
UCHAR BlFwIfReadMode(VOID)
{
	UCHAR	uchMode;

	uchMode = UieReadMode();

	return uchMode;
}

/*
*===========================================================================
** Synopsis:    USHORT BlFwIfMldSetReceiptWindow(USHORT usScroll)
*
*     Input:    USHORT usScroll - scroll display
*
*    Output:    USHORT usHandle
*

		Added to support TOUCHSCREEN

  ** Return:
*===========================================================================
*/

_declspec(dllexport)
SHORT BlFwIfMldSetReceiptWindow(USHORT usScroll)
{
	return MldSetCursor(usScroll);
}

_declspec(dllexport)
SHORT BlFwIfMldCursorUp (USHORT usScroll)
{
	return MldUpCursor ();
}


_declspec(dllexport)
SHORT BlFwIfMldCursorDown (USHORT usScroll)
{
	return MldDownCursor ();
}

/*
*===========================================================================
** Synopsis:    USHORT BlFwIfMldSetReceiptWindow(USHORT usScroll)
*
*     Input:    USHORT usScroll - scroll display
*
*    Output:    USHORT usHandle
*
** Return:      TRUE - set
				FALSE - not set

				Scroll 1 / Main = MLD_SCROLL_1
				Scroll 2 = MLD_SCROLL_2
				Scroll 3 = MLD_SCROLL_3

		Added to support TOUCHSCREEN
*===========================================================================
*/

_declspec(dllexport)
USHORT BlFwIfMldGetVosWinHandle(USHORT usScroll)
{
	return MldGetVosWinHandle(usScroll);
}

/*
*===========================================================================
** Synopsis:    USHORT BlFwIfMldSetReceiptWindow(USHORT usScroll)
*
*     Input:    USHORT usScroll - scroll display
*
*    Output:    USHORT usHandle
*
** Return:      TRUE - set
				FALSE - not set

				Scroll 1 / Main = MLD_SCROLL_1
				Scroll 2 = MLD_SCROLL_2
				Scroll 3 = MLD_SCROLL_3

		Added to support TOUCHSCREEN
*===========================================================================
*/

_declspec(dllexport)
SHORT BlFwIfMldNotifyCursorMoved(USHORT usScroll)
{
	return MldSetMoveCursor(usScroll);
}

/*
*===========================================================================
** Synopsis:    WCHAR * BlFwIfTerminalLayout(TERMINALINFO *pTermInfo)
*
*     Input:    TERMINALINFO *pTermInfo - if not NULL, pointer to TERMINALINFO struct
*
*    Output:    TERMINALINFO *pTermInfo - if not NULL, terminal info for this terminal
*
** Return:      WCHAR *   TerminalInfo.aszDefaultMenuFileName
*
** Description:	This function looks in the Para database area to obtain the layout
*				file name to be used with this terminal.  This function provides
*				that layout file name to the caller.  It will also provide the 
*				complete TERMINALINFO struct for this terminal if the caller provides
*				an address for a data area to receive the struct.
*===========================================================================
*/

_declspec(dllexport)
WCHAR * BlFwIfTerminalLayout(TERMINALINFO *pTermInfo)
{
	static PARATERMINALINFO  myTermInfo;
	CONST SYSCONFIG *pSysConfig = PifSysConfig();

    myTermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
    myTermInfo.uchMinorClass = 0;
    myTermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);

	ParaTerminalInfoParaRead (&myTermInfo);

	if (pTermInfo) {
		*pTermInfo = myTermInfo.TerminalInfo;
	}
	return myTermInfo.TerminalInfo.aszDefaultMenuFileName ;
}


__declspec(dllexport)
VOID * BlFwIfSetFrameworkTouchWindow(VOID *pMainFramework)
{
	static VOID *pMainTouchWindow = 0;

	if (pMainFramework) {
		pMainTouchWindow = pMainFramework;
	}

	return pMainTouchWindow;
}

__declspec(dllexport)
VOID * BlFwIfGetFrameworkTouchWindow(void)
{
	return BlFwIfSetFrameworkTouchWindow(0);
}

// Set the parent Window handle to be used by the MessageBox () function
// as its parent window.  This will allow MessageBox () to perform SYSMODAL
// functionality correctly so as to render the parent NHPOS window inactive
// until the MessageBox () is dismissed through a user action.
__declspec(dllexport)
VOID BlFwIfSetWindowHandle(VOID *pMainHwnd)
{
	PifSetWindowHandle (pMainHwnd);
}


/*
*===========================================================================
** Synopsis:    int   BlFwIfIntrnlGetGroupPluInformationNext(BL_PLU_INFO *pPluInfo);
*     InPut:    usGCNumber  - GC number to assign
*               pPluInfo->uchGroupNo - the group number for the list of PLUs;
*               pPluInfo->ulCounter  - iteration counter, if 0 start from first PLU
*               
** Return:      0    PLU found and data being returned.
*               1    PLU not found.
*               2    Error retrieving the PLU information
*               3    Master down	
*
** Description: This function obtains the PLU information for the purposes of
*               obtaining the PLU mnemonic and basic information for OEP window
*               display and Dynamic PLU buttons..
*===========================================================================
**/
int BlFwIfIntrnlGetGroupPluInformationNext(BL_PLU_INFO *pPluInfo)
{
    PLUIF_OEP_VARY  *pIndex = 0;
	PLU_PARA        *pPara = 0;
    SHORT            sReadStat = OP_NOT_IN_FILE;
	UCHAR            uchNoOrderPmt = 1;
	PARAADJMNEMO     Adj = {0};
	OPPLUOEPIF       PluOepIf;

	pIndex = (PLUIF_OEP_VARY *) alloca (sizeof(PLUIF_OEP_VARY) + sizeof(PLU_PARA) * (uchNoOrderPmt + 1));
	NHPOS_ASSERT(pIndex);
	if (!pIndex) return 2;

	memset (pIndex, 0, sizeof(PLUIF_OEP_VARY) + sizeof(PLU_PARA) * (uchNoOrderPmt + 1));

	pIndex->usGroupNo = pPluInfo->usGroupNo;
	pIndex->culCounter = pPluInfo->ulCounter;

    memset(pPluInfo->auchPluNo, 0, sizeof(pPluInfo->auchPluNo));       // Clear PLU No
    memset(pPluInfo->auchAdjName, 0, sizeof(pPluInfo->auchAdjName));   // Clear the Adjective Name
    memset(pPluInfo->uchPluName, 0, sizeof(pPluInfo->uchPluName));     // Clear PLU name or mnemonic 

	pPluInfo->uchPluAdj = 0;
	pPluInfo->uchPluAdjMod = 0;

	if (FindOepPluItem (pPluInfo) == 0) {
		return 0;
	}

	while (1)
	{
		PLUIF            Plu = {0};

		sReadStat = CliOpOepPluReadMultiTable(pPluInfo->usTableNo, pPluInfo->usFilePart, pIndex, 0, uchNoOrderPmt);
		if (sReadStat == OP_NOT_IN_FILE)
		{
			// increment the counter and then return with a code indicating to skip this PLU variant.
			pPluInfo->ulCounter = pIndex->culCounter;
			return 100;
		}
		else if (sReadStat != OP_PERFORM)
		{
			return(2);
		}

		if (pIndex->PluPara20[0].auchPluNo[0] == 0)
		{
			return 4;
		}

		memset(&PluOepIf, 0, sizeof(PluOepIf));
		_tcsncpy(PluOepIf.PluNo.auchPluNo[0], pIndex->PluPara20[0].auchPluNo, NUM_PLU_LEN);

		sReadStat = CliOpPluOepRead(&PluOepIf, 0);
		if (sReadStat == OP_M_DOWN_ERROR) {
			return 3;
		}
		else if (sReadStat == OP_NOT_IN_FILE)
		{
			// increment the counter and then return with a code indicating to skip this PLU variant.
			pPluInfo->ulCounter = pIndex->culCounter;
			return 100;
		}
		else if (sReadStat != OP_PERFORM) {
			return 2;
		}

		if ((PluOepIf.PluName.uchItemType[0]  & PLU_TYPE_MASK_REG) != PLU_ADJ_TYP)
		{
			// not an adjective PLU so no need to do any adjective related processing.
			break;
		}

		// transfer the needed PLU information from the PLU record read into our PLU data struct
		// for further processing. In the case of missing information that needs to be added, there
		// may be a need for modification of function OpPluOepRead() which reads the physical PLU record
		// and then puts the data into the struct that is returned.
    	_tcsncpy(Plu.auchPluNo, pIndex->PluPara20[0].auchPluNo, NUM_PLU_LEN);
		Plu.uchPluAdj = pIndex->PluPara20[0].uchPluAdj;
		Plu.ParaPlu.ContPlu.uchItemType = PluOepIf.PluName.uchItemType[0];
    	Plu.ParaPlu.ContPlu.auchContOther[2] = PluOepIf.PluName.auchContOther[0][0];
    	Plu.ParaPlu.ContPlu.auchContOther[3] = PluOepIf.PluName.auchContOther[1][0];
    	Plu.ParaPlu.ContPlu.auchContOther[4] = PluOepIf.PluName.auchContOther[2][0];  //SR 336 Transfer the control information for
																	                  //prohibited adjectives being displayed JHHJ

		// if this PLU is a condiment then just go ahead and use it.
		// However if it is not a condiment then we need to check to see if it has an
		// adjective.  If it doesn't then just go ahead and use it.
		// If it is a PLU with various types (uses adjectives such as Large or Small) then
		// check to see if the adjective we have found is prohibitted or not.  If not prohibitted
		// then go ahead and use it other wise lets go to the next PLU or PLU adjective variant.
		if ((Plu.ParaPlu.ContPlu.auchContOther[2] & PLU_CONDIMENT) == 0)
		{
			if (Plu.uchPluAdj)
			{
				if (ITM_SUCCESS == ItemSalesSetCheckValidation(PluOepIf.PluName.auchContOther[2][0], Plu.uchPluAdj))
				{
					// filter out Adjective type so lets skip it.
					// increment the counter and then return with a code indicating to skip this PLU variant.
					if (pPluInfo->ulFilterOptions & BL_PLU_INFO_FILTER_ADJ) {
						if ((pPluInfo->ulFilterOptions & BL_PLU_INFO_FILTER_ADJ_LOGIC_LT)&& Plu.uchPluAdj >= pPluInfo->usFilterValue) {
							pPluInfo->ulCounter = pIndex->culCounter;
							return 100;
						}
						if ((pPluInfo->ulFilterOptions & BL_PLU_INFO_FILTER_ADJ_LOGIC_GT)&& Plu.uchPluAdj <= pPluInfo->usFilterValue) {
							pPluInfo->ulCounter = pIndex->culCounter;
							return 100;
						}
						if ((pPluInfo->ulFilterOptions & (BL_PLU_INFO_FILTER_ADJ_LOGIC_LT | BL_PLU_INFO_FILTER_ADJ_LOGIC_GT)) == 0 && pPluInfo->usFilterValue != Plu.uchPluAdj) {
							pPluInfo->ulCounter = pIndex->culCounter;
							return 100;
						}
					}
#if 0
					// if there is no implied adjective key then we will specify the adjective as part of
					// the button data. however if there is an implied adjective key then we will use that
					// adjective variation in the caption but will not specify it in the PLU adjective data
					// for this button. by not specifying the PLU adjective, we allow the default to flow
					// through and it also allows for the use of an adjective key to modify the PLU adjective.
					if ((Plu.ParaPlu.ContPlu.auchContOther[4] & PLU_IMP_ADJKEY_MASK) != PLU_COMP_ADJKEY) {
						if (Plu.uchPluAdj != (Plu.ParaPlu.ContPlu.auchContOther[4] & PLU_IMP_ADJKEY_MASK)) {
							pPluInfo->ulCounter = pIndex->culCounter;
							return 100;
						}
					}
					Adj.uchMajorClass = CLASS_PARAADJMNEMO;
					Adj.uchAddress    = ItemSalesOEPGetAdjNo(&Plu);
    				CliParaRead(&Adj);
					if ((Plu.ParaPlu.ContPlu.auchContOther[4] & PLU_IMP_ADJKEY_MASK) != PLU_COMP_ADJKEY) {
						pIndex->PluPara20[0].uchPluAdj = Adj.uchAddress = 0;   // there is an implied adjective so clear this and let if flow on through.
					}
#else
					Adj.uchMajorClass = CLASS_PARAADJMNEMO;
    				Adj.uchAddress    = ItemSalesOEPGetAdjNo(&Plu);
        			CliParaRead(&Adj);
#endif
				}
				else {
					// Prohibitted Adjective type so lets skip it.
					// increment the counter and then return with a code indicating to skip this PLU variant.
					pPluInfo->ulCounter = pIndex->culCounter;
					return 100;
				}
			}
		}
		break;
	}

	pPara = &(pIndex->PluPara20[0]);
    _tcsncpy(pPluInfo->auchPluNo, pPara->auchPluNo, STD_PLU_NUMBER_LEN);   // Set PLU No
	if (IS_REMOVE_PLU_CHECK_DIGIT(pPara->auchPluNo)) {
		pPluInfo->auchPluNo[STD_PLU_NUMBER_LEN - 1] = 0;                   // remove last digit, which is check digit
	}

    _tcsncpy(pPluInfo->uchPluName, PluOepIf.PluName.auchPluName[0], STD_PLU_MNEMONIC_LEN);  // Set PLU name or mnemonic 
	pPluInfo->uchPluName[STD_PLU_MNEMONIC_LEN] = 0;

	pPluInfo->uchPluAdj = pPara->uchPluAdj;
	pPluInfo->uchPluAdjMod = Adj.uchAddress;
    pPluInfo->auchContOther[2] = PluOepIf.PluName.auchContOther[0][0];
    pPluInfo->auchContOther[3] = PluOepIf.PluName.auchContOther[1][0];
    pPluInfo->auchContOther[4] = PluOepIf.PluName.auchContOther[2][0];  //SR 336 Transfer the control information for
																	    //prohibited adjectives being displayed JHHJ
	_tcsncpy(pPluInfo->auchAdjName, Adj.aszMnemonics, PARA_ADJMNEMO_LEN);   // Set the Adjective Name
	pPluInfo->ulCounter = pIndex->culCounter;

	InsertOepPluItem (pPluInfo);

	return 0;
}

int BlFwIfIntrnlGetGroupPluInformationFirst(USHORT usTableNumber, USHORT usGroupNumber, USHORT usFilePart, ULONG ulFilter, USHORT usFilterValue, BL_PLU_INFO *pPluInfo)
{
	pPluInfo->usGroupNo = usGroupNumber;
	pPluInfo->ulCounter = 0;
	pPluInfo->ulInternalCounter = 0;
	pPluInfo->usTableNo = usTableNumber;
	pPluInfo->usFilePart = usFilePart;
	pPluInfo->ulFilterOptions = ulFilter;
	pPluInfo->usFilterValue = usFilterValue;

	return BlFwIfIntrnlGetGroupPluInformationNext(pPluInfo);
}

__declspec(dllexport)
int BlFwIfGetGroupPluInformationNext(BL_PLU_INFO *pPluInfo)
{
	return BlFwIfIntrnlGetGroupPluInformationNext(pPluInfo);
}

__declspec(dllexport)
int BlFwIfGetGroupPluInformationFirst(USHORT usTableNumber, USHORT usGroupNumber, USHORT usFilePart, ULONG ulFilter, USHORT usFilterValue, BL_PLU_INFO *pPluInfo)
{
	return BlFwIfIntrnlGetGroupPluInformationFirst(usTableNumber, usGroupNumber, usFilePart, ulFilter, usFilterValue, pPluInfo);
}


__declspec(dllexport)
int BlFwIfGetPluInformation(UCHAR *aszPluNo, UCHAR uchPluAdj, BL_PLU_INFO *pPluInfo)
{
	// The PLU information is set in AC 68, PLU Maintenance.  This function
	// allows us to auto populate buttons designated with the CWindowButton::BATypePLU
	// type with the PLU mnemonic as the caption.
	int      sRetStatus = -1;
	PLUIF    Plu = {0};        /* PLU record receive buffer */
	TCHAR    tszPluNo[16] = {0};
	SHORT    sReturnStatus;
	size_t   iLen = 14 - strlen(aszPluNo);

	_RflStrcpyUchar2Tchar (tszPluNo, aszPluNo);
	ItemSalesLabelKeyedOnlyProc (tszPluNo);
	_tcsncpy (Plu.auchPluNo, tszPluNo, NUM_PLU_LEN);
	Plu.uchPluAdj = uchPluAdj;

	//if we are still at 0, that means there is no certain price level, assign to 1
	if(Plu.uchPluAdj == 0) {
		Plu.uchPluAdj = 1;
	}

	if ((sReturnStatus = CliOpPluIndRead(&Plu, 0)) == OP_PERFORM) {
		if ((Plu.ParaPlu.ContPlu.uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP) {
			PARAADJMNEMO     Adj = {0};

			Adj.uchMajorClass = CLASS_PARAADJMNEMO;
			if ( (Adj.uchAddress = ItemSalesOEPGetAdjNoImplied(&Plu)) == 0) {
				_tcsncpy(pPluInfo->auchAdjName, L"**", PARA_ADJMNEMO_LEN);     // Set the Adjective Name
			} else {
				// there is an Implied Adjective specified so lets get it.
				CliParaRead(&Adj);
				_tcsncpy(pPluInfo->auchAdjName, Adj.aszMnemonics, PARA_ADJMNEMO_LEN);     // Set the Adjective Name
			}
		}

		_tcsncpy (pPluInfo->auchPluNo, Plu.auchPluNo, NUM_PLU_LEN);
		_tcsncpy (pPluInfo->uchPluName, Plu.ParaPlu.auchPluName, STD_PLU_MNEMONIC_LEN);
		_tcsncpy (pPluInfo->uchPluNameAlt, Plu.ParaPlu.auchAltPluName, STD_PLU_MNEMONIC_LEN);
		pPluInfo->uchPluAdj = Plu.usAdjCo;
		pPluInfo->uchPluAdjMod = ItemSalesOEPGetAdjNo(&Plu);
		pPluInfo->usDeptNo = Plu.ParaPlu.ContPlu.usDept;
		pPluInfo->usGroupNo = Plu.ParaPlu.uchGroupNumber;
		pPluInfo->usTableNo = Plu.ParaPlu.uchTableNumber;

		sRetStatus = 0;    // indicate success at retrieving the PLU data.
	} 

	return sRetStatus;
}


__declspec(dllexport)
int BlFwIfGetCouponInformation(USHORT usCouponNo, BL_CPN_INFO *pCpnInfo)
{
	// The coupon information is set in AC 161, Coupon Maintenance, which is also
	// used for the Auto Coupon functionality.  This function allows us to auto populate
	// buttons designated with the CWindowButton::BATypeCoupon type.
	int     sRetStatus = -1;

	if (usCouponNo > 0) {
		CPNIF   WorkCoupon = {0};

		WorkCoupon.uchCpnNo = usCouponNo;
		if ((sRetStatus = CliOpCpnIndRead(&WorkCoupon, 0)) == OP_PERFORM) {
			pCpnInfo->usCpnNo = usCouponNo;
			pCpnInfo->uchAutoCPNStatus = WorkCoupon.ParaCpn.uchAutoCPNStatus;
			pCpnInfo->ulCouponAmount = WorkCoupon.ParaCpn.ulCouponAmount;
			_tcsncpy (pCpnInfo->aszCouponMnem, WorkCoupon.ParaCpn.aszCouponMnem, 20);
			pCpnInfo->aszCouponMnem[STD_CPNNAME_LEN] = 0;
			RflCleanupMnemonic (pCpnInfo->aszCouponMnem);
		}
	}
	return sRetStatus;
}

/*
*
*  Description:  Retrieve from the memory resident database a mnemonic of the type specified.
*
*                WARNING: Some databases use the tilde (~) character in mnemonics in order to
*                         cause the printing of the mnemonic on the receipt printer to be
*                         more noticable or visibile by printing in double wide characters.
*                         See function RflCleanupMnemonic () in Framework subsystem.
*/
__declspec(dllexport)
int BlFwIfGetMemoryResidentMnemonic(USHORT usMnemonicType, USHORT usAddress, TCHAR tcsMnemonic[32])
{
	int     sRetStatus = -1;

	tcsMnemonic[0] = 0;
	switch (usMnemonicType) {
		case BLI_MNEMOTYPE_P20_TRANS:
			RflCleanupMnemonic(RflGetTranMnem (tcsMnemonic, usAddress));
			sRetStatus = 0;
			break;
		case BLI_MNEMOTYPE_P21_LEADTHRU:
			RflCleanupMnemonic(RflGetLead (tcsMnemonic, usAddress));
			sRetStatus = 0;
			break;
		default:
			break;
	}
	return sRetStatus;
}

__declspec(dllexport)
int BlFwIfGetDepartInformation(USHORT usDepartNo, BL_DEPT_INFO *pDeptInfo)
{
	// The department information is set in AC 114, Department Maintenance.  This function
	// allows us to auto populate buttons designated with the CWindowButton::BATypeDept type.
	int     sRetStatus = -1;

	if (usDepartNo > 0) {
		DEPTIF   WorkDepartment = {0};

		WorkDepartment.usDeptNo = usDepartNo;
		if ((sRetStatus = CliOpDeptIndRead (&WorkDepartment, 0)) == OP_PERFORM) {
			pDeptInfo->usDeptNo = usDepartNo;
			_tcsncpy (pDeptInfo->aszDeptMnem, WorkDepartment.ParaDept.auchMnemonic, 20);
			pDeptInfo->aszDeptMnem[STD_CPNNAME_LEN] = 0;
			RflCleanupMnemonic (pDeptInfo->aszDeptMnem);
		}
	}
	return sRetStatus;
}

__declspec(dllexport)
int BlFwIfGetAdjectiveMnemonic(USHORT usAddress, TCHAR tcsMnemonic[32])
{
	// The adjective labels are set in P 46, Adjective Mnemonics.  This function
	// allows us to auto populate buttons designated with the CWindowButton::BATypeAdj type.
	int     sRetStatus = OP_PERFORM;

	RflCleanupMnemonic(RflGetAdj(tcsMnemonic, usAddress));
	return sRetStatus;
}


__declspec(dllexport)
int BlFwIfGetPrintModifierMnemonic(USHORT usAddress, TCHAR tcsMnemonic[32])
{
	// The print modifier mnemonics are set in P 47, Print Modifier Mnemonics.  This function
	// allows us to auto populate buttons designated with the Print Modifier mnemonic
	// identifier allowing these mnemonics to be used with button captions.
	int     sRetStatus = OP_PERFORM;

	RflCleanupMnemonic(RflGetMnemonicByClass(CLASS_PARAPRTMODI, tcsMnemonic, usAddress));
	return sRetStatus;
}

__declspec(dllexport)
int BlFwIfGetPresetCashMnemonic(USHORT usAddress, TCHAR tcsMnemonic[32])
{
	// The P15 Preset Cash Tender mnemonics must be created on the fly.  This function
	// allows us to auto populate buttons designated with the Preset Cash Tender mnemonic
	// identifier allowing these mnemonics to be used with button captions.
	int     sRetStatus = -1;

	tcsMnemonic[0] = 0;
	if (usAddress > 0 && usAddress <= PRESET_AMT_ADR_MAX) {
		PARAPRESETAMOUNT    ParaPresetAmount = {0};
		TCHAR   MemoryMnemonic[PARA_TRANSMNEMO_LEN + 1] = {0};

		RflCleanupMnemonic(RflGetTranMnem (MemoryMnemonic, TRN_TEND1_ADR));

        ParaPresetAmount.uchMajorClass = CLASS_PARAPRESETAMOUNT;
        ParaPresetAmount.uchStatus = 0;                             /* Set W/ Amount Status */
        ParaPresetAmount.uchAddress = usAddress;
		CliParaRead(&ParaPresetAmount);                             /* Get Preset Cash Amount */

		RflSPrintf (tcsMnemonic, 32, L"%l$ %s", (DCURRENCY)ParaPresetAmount.ulAmount, MemoryMnemonic);  // format %l$ requires DCURRENCY.
		sRetStatus = OP_PERFORM;
	}
	return sRetStatus;
}


__declspec(dllexport)
int BlFwIfGetReasonCodeMnemonic(USHORT usRecordNo, TCHAR tcsMnemonic[32])
{
	// The reason code mnemonic is set in P 72, Reason Code Mnemonics.  This function
	// allows us to auto populate buttons designated with the Reason Code identifier in the caption.
	// The use of Reason Codes allows us to access a button caption mnemonic that can change easily.
	int     sRetStatus = -1;

	tcsMnemonic[0] = 0;
	if (usRecordNo > 0 && usRecordNo <= MAX_RSNMNEMO_SIZE) {
		OPMNEMONICFILE MnemonicFile = {0};

		MnemonicFile.usMnemonicFileId = OPMNEMONICFILE_ID_REASON;         /* Set Major for Adj Mnemo */
		MnemonicFile.usMnemonicAddress = usRecordNo;                      /* Set Adj Address for Adj Mnemo */
		OpMnemonicsFileIndRead(&MnemonicFile, 0);                         /* Get Adjective Mnemonics */
		_tcsncpy (tcsMnemonic, MnemonicFile.aszMnemonicValue, 20);
		tcsMnemonic[20] = 0;
		RflCleanupMnemonic (tcsMnemonic);
		sRetStatus = OP_PERFORM;
	}
	return sRetStatus;
}


/*
*===========================================================================
** Synopsis:    int BlFwIfConvertPLU (UCHAR *aszPluNo, BL_PLU_INFO *pPluInfo)
*
*     Input:    BL_PLU_INFO *pPluInfo - contains the PLU information
*
*    Output:    UCHAR *aszPluNo - contains the converted PLU number
*
** Return:      result code
*
** Description: converts a PLU number from the internal representation to an
*               external representation suitable for use within the Framework
*               button logic for handling button presses and converting the
*               button press to a PLU number.  This function uses the standard
*               RFL function RflConvertPLU () to convert the PLU number to the
*               internal representation.  It then converts the wide character
*               result to a UCHAR array by stripping off the high byte of the
*               WCHAR character.  This is done because the button press logic
*               uses UCHAR for the PLU number rather than WCHAR..
*===========================================================================
*/
__declspec(dllexport)
int BlFwIfConvertPLU (UCHAR *aszPluNo, WCHAR *auchPluNo)
{
	int    ijkl = 0;
	WCHAR  wszCaption[64] = {0};

	RflConvertPLU(&wszCaption[0], &auchPluNo[0]);
	for (ijkl = 0; wszCaption[ijkl] && ijkl < STD_PLU_NUMBER_LEN; ijkl++) {
		aszPluNo[ijkl] = (UCHAR)wszCaption[ijkl];
	}
	aszPluNo[ijkl] = 0;
	return 1;
}

__declspec(dllexport)
BOOL BlFwIfIsPluFileUnLocked ()
{
	return OpIsUnLockedPluFile();
}

/*
*===========================================================================
** Synopsis:    BOOL BlFwIfReloadOEPGroup()
*
*     Input:    
*
*    Output:    nothing
*
** Return:      result code
*
** Description: forces terminal application to reload any windows that contain
*               OEP groups or dynamic PLU buttons so that the displayed buttons
*               will be up to date.
*
*               The function indicator is CMD_FWRELOADOEP_FUNC when 
*               The target function kicks off the FRAME_TIMER_CMD_RELOADOEP timer
*               and the timer will trigger a call to CFrameworkWndDoc::RefreshOepWindows().
*
*               The function pointer s_pfnBlInterfaceOep is normally assigned to FwReloadOepWindow().
*===========================================================================
*/
BOOL BlFwIfReloadOEPGroup ()
{
	BOOL bResult = FALSE;

	NHPOS_ASSERT(s_pfnBlInterfaceOep);

	if(s_pfnBlInterfaceOep){
		bResult = s_pfnBlInterfaceOep();
	}
	return bResult;
}

/*
*===========================================================================
** Synopsis:    USHORT BlFwIfPostMessage(UINT MsgType, WPARAM wparam, TCHAR *ptcMsgPayload)
*
*     Input:    MsgType			- a UINT indicating the type of message to send
*				wparam			- a flag to be sent with the message to further
*									indicate what to do with it
*				ptcMsgPayload	- TCHAR array containing the payload of the
									message to send
*
*    Output:    nothing
*
** Return:      result code
*
** Description: calls into Framework with the message indicated by the calling
*				function.  The message payload is placed in a static buffer
*				for the asynchronous PostMessage call.
*===========================================================================
*/
USHORT BlFwIfPostMessage(UINT MsgType, WPARAM wparam, TCHAR *ptcMsgPayload)
{
	static TCHAR tcStringPool[1024];
	static TCHAR *ptcNextString = tcStringPool;
	TCHAR *ptcThisString = 0;

	DWORD   dwLastError = 0;
	HANDLE  hWnd = PifGetWindowHandle(0);
	USHORT  usRet = 0;
		
	if (hWnd) {
		if (ptcMsgPayload) {
			ptcThisString = ptcNextString;
			_tcsncpy (ptcThisString, ptcMsgPayload, 255);
			*(ptcNextString + 255) = 0;  // ensure there is an end of string
			ptcNextString += _tcslen (ptcThisString) + 1;
			if (ptcNextString > tcStringPool + sizeof(tcStringPool)/sizeof(tcStringPool[0]) - 260)
				ptcNextString = tcStringPool;
		}
		usRet = PostMessage(hWnd, MsgType, wparam, (LPARAM)ptcThisString);
		dwLastError = GetLastError();
	}

	return usRet;
}

/*
*===========================================================================
** Synopsis:    USHORT  BlFwIfPopupWindowName (TCHAR *ptcWindowName)
*
*     Input:    ptcWindowName - TCHAR array containing the Layout Manager window name
*
*    Output:    nothing
*
** Return:      result code
*
** Description: calls into Framework to invoke a window popup.
*               The window name specified is looked up within Framework and if
*               a window with that name exists then the action is similar to
*               if a button whose action is to popup a window is pressed.
*===========================================================================
*/
USHORT  BlFwIfPopupWindowName (TCHAR *ptcWindowName){
	return BlFwIfPostMessage((UINT)WU_EVS_POPUP_WINDOW, (WPARAM)0, ptcWindowName);
}

/*
*===========================================================================
** Synopsis:    USHORT  BlFwIfPopupWindowPrefix (int WindowPrefix)
*
*     Input:    WindowPrefix - int containing the Layout Manager window prefix
*
*    Output:    nothing
*
** Return:      result code
*
** Description: calls into Framework to invoke a window popup.
*               The window prefix specified is looked up within Framework and if
*               a window with that prefix exists then the action is similar to
*               if a button whose action is to popup a window is pressed.
*===========================================================================
*/
USHORT BlFwIfPopupWindowPrefix (TCHAR *WindowPrefix){
	return BlFwIfPostMessage((UINT)WU_EVS_POPUP_WINDOW, (WPARAM)3, WindowPrefix);
}

USHORT BlFwIfPopupWindowGroup (TCHAR *WindowPrefix){
	return BlFwIfPostMessage((UINT)WU_EVS_POPUP_WINDOW, (WPARAM)5, WindowPrefix);
}

/*
*===========================================================================
** Synopsis:    USHORT  BlFwIfPopupMessageWindow (TCHAR *tcsMessage)
*
*     Input:    WindowPrefix - int containing the Layout Manager window prefix
*
*    Output:    nothing
*
** Return:      result code
*
** Description: calls into Framework to invoke a window popup.
*               The window prefix specified is looked up within Framework and if
*               a window with that prefix exists then the action is similar to
*               if a button whose action is to popup a window is pressed.
*===========================================================================
*/
USHORT BlFwIfPopupMessageWindow (TCHAR *tcsMessage) {
	return BlFwIfPostMessage((UINT)WU_EVS_POPUP_WINDOW, (WPARAM)4, tcsMessage);
}

USHORT BlFwIfPopdownMessageWindow (VOID) {
	return BlFwIfPostMessage((UINT)WU_EVS_POPDOWN_WINDOW, (WPARAM)4, 0);    // m_pMessageDialog->UnshowAndRemove ()
}

// Process the list of windows in the displayed layout and popdown all windows
// that are marked as Popup windows.
USHORT BlFwIfPopdownMessageAllVirtual (VOID) {
	return BlFwIfPostMessage((UINT)WU_EVS_POPDOWN_WINDOW, (WPARAM)6, 0);   // CFrameworkWndDoc::SearchForItemByTypeApplyFunc()
}


/*
*===========================================================================
** Synopsis:    USHORT  BlFwIfPopupWindowEventTrigger (int SpecWinEventTrigger_WId)
*
*     Input:    ptcWindowName - TCHAR array containing the Layout Manager window name
*
*    Output:    nothing
*
** Return:      result code
*
** Description: calls into Framework to invoke a window popup.
*               The window name specified is looked up within Framework and if
*               a window with that name exists then the action is similar to
*               if a button whose action is to popup a window is pressed.
*===========================================================================
*/
USHORT  BlFwIfPopupWindowEventTrigger (int SpecWinEventTrigger_WId)
{
	static TCHAR tcStringPool[512];
	static TCHAR *ptcNextString = tcStringPool;
	DWORD   dwLastError = 0;
	TCHAR   *ptcThisString = 0;
    BOOL    status;

	HANDLE hWnd = PifGetWindowHandle(0);

	if (hWnd) {
		status = SendMessage(hWnd, (UINT)WU_EVS_POPUP_WINDOW, (WPARAM)2, (LPARAM)SpecWinEventTrigger_WId);
		dwLastError = GetLastError();
		if (status) {
			status = PostMessage(hWnd, (UINT)WU_EVS_POPUP_WINDOW, (WPARAM)1, (LPARAM)SpecWinEventTrigger_WId);
			dwLastError = GetLastError();
		}
	}
	return status;
}

/*
*===========================================================================
** Synopsis:    USHORT  BlFwIfPopdownWindowName (TCHAR *ptcWindowName)
*
*     Input:    ptcWindowName - TCHAR array containing the Layout Manager window name
*
*    Output:    nothing
*
** Return:      result code
*
** Description: calls into Framework to invoke a window popdown.
*               The window name specified is looked up within Framework and if
*               a window with that name exists then the action is similar to
*               if a button whose action is to popdown a window is pressed.
*===========================================================================
*/
USHORT  BlFwIfPopdownWindowName (TCHAR *ptcWindowName){
	return BlFwIfPostMessage((UINT)WU_EVS_POPDOWN_WINDOW, (WPARAM)0, ptcWindowName);
}

/*
*===========================================================================
** Synopsis:    USHORT  BlFwIfPopdownWindowPrefix (int WindowPrefix)
*
*     Input:    WindowPrefix - int containing the Layout Manager window prefix
*
*    Output:    nothing
*
** Return:      result code
*
** Description: calls into Framework to invoke a window popdown.
*               The window prefix specified is looked up within Framework and if
*               a window with that prefix exists then the action is similar to
*               if a button whose action is to popdown a window is pressed.
*===========================================================================
*/
USHORT BlFwIfPopdownWindowPrefix (TCHAR *WindowPrefix){
	return BlFwIfPostMessage((UINT)WU_EVS_POPDOWN_WINDOW, (WPARAM)1, WindowPrefix);
}

USHORT BlFwIfPopdownWindowGroup (TCHAR *WindowPrefix){
	return BlFwIfPostMessage((UINT)WU_EVS_POPDOWN_WINDOW, (WPARAM)5, WindowPrefix);   // CFrameworkWndDoc::DismissWindowsByGroup()
}

/*
*===========================================================================
** Synopsis:    BOOL FwReloadLayout2()
*
*     Input:    dwCommand	- function command
*				pFunc       - ptr. to call back function
*
*    Output:    nothing
*
** Return:      result code
*===========================================================================
*/
BOOL	BlFwIfReloadLayout2()
{
	BOOL bResult = FALSE;

	if(s_pfnBlInterfaceLay){
		bResult = s_pfnBlInterfaceLay();
	}
	return bResult;
}

/*
*===========================================================================
** Synopsis:    BOOL BlFwIfDeviceControl()
*
*     Input:
*
*    Output:
*
** Return:
*===========================================================================
*/
BOOL BlFwIfDeviceControl(USHORT usDevice, BOOL fState)
{
	BOOL					bResult;
	FRAMEWORK_IOCTL_STATE	Req = {0};

	// make up parameter
	switch (usDevice)
	{
	case BLFWIF_KEYLOCK:	Req.dwDevice = FRAMEWORK_KEYLOCK;		break;
	case BLFWIF_DRAWER:		Req.dwDevice = FRAMEWORK_DRAWER;		break;
	case BLFWIF_MSR:		Req.dwDevice = FRAMEWORK_MSR;			break;
	case BLFWIF_SCANNER:	Req.dwDevice = FRAMEWORK_SCANNER;		break;
	case BLFWIF_SCALE:		Req.dwDevice = FRAMEWORK_SCALE;			break;
	case BLFWIF_SERVERLOCK:	Req.dwDevice = FRAMEWORK_SERVERLOCK;	break;
	default:
		return FALSE;
	}

	Req.bState = fState;

	/* call framework */
	bResult = s_pfnBlInterface(FRAMEWORK_IOCTL_CONTROL, &Req, sizeof(Req));
	return bResult;
}
/*
*===========================================================================
** Synopsis:    BOOL BlFwIfDeviceVersion()
*
*     Input:
*
*    Output:
*
** Return:
*===========================================================================
*/
BOOL BlFwIfDeviceVersion(USHORT usDevice, TCHAR* pVersion)
{
	BOOL					bResult;
	FRAMEWORK_IOCTL_STATE	Req = {0};

//	Req.pVersion = pVersion;

	// make up parameter

	switch (usDevice)
	{
	case BLFWIF_KEYLOCK:	Req.dwDevice = FRAMEWORK_KEYLOCK;		break;
	case BLFWIF_DRAWER:		Req.dwDevice = FRAMEWORK_DRAWER;		break;
	case BLFWIF_MSR:		Req.dwDevice = FRAMEWORK_MSR;			break;
	case BLFWIF_SCANNER:	Req.dwDevice = FRAMEWORK_SCANNER;		break;
	case BLFWIF_SCALE:		Req.dwDevice = FRAMEWORK_SCALE;			break;
	case BLFWIF_SERVERLOCK:	Req.dwDevice = FRAMEWORK_SERVERLOCK;	break;
	case BLFWIF_PRINTER:	Req.dwDevice = FRAMEWORK_PRINTER;		break;
	case BLFWIF_CDISPLAY:	Req.dwDevice = FRAMEWORK_CDISPLAY;		break;
	case BLFWIF_COIN:		Req.dwDevice = FRAMEWORK_COIN;			break;
	default:
		return FALSE;
	}

	/* call framework */
	bResult = s_pfnBlInterface(FRAMEWORK_IOCTL_VERSION_INFO, &Req, sizeof(Req));
	if(bResult)
	{
		_tcsncpy(pVersion, Req.Version, sizeof(Req.Version)/sizeof(Req.Version[0]));
	}
	return bResult;
}
/*
*===========================================================================
** Synopsis:    USHORT BlFwIfDrawerOpen()
*
*     Input:
*
*    Output:
*
** Return:
*===========================================================================
*/
USHORT BlFwIfDrawerOpen(USHORT usNumber)
{
	ULONG						ulDrawer[2];
	USHORT						usStatus;
	FRAMEWORK_IO_DRAWER_OPEN	Req = {0};

	// make up parameter
	Req.dwNumber = (DWORD)usNumber;

	/* call framework */
#if 0
	bResult = s_pfnBlInterface(FRAMEWORK_IOCTL_DRAWER_OPEN, &Req, sizeof(Req));
	PifSleep (100);    // Give the OPOS control time to process the cash drawer open
#else
	UifDeviceMessageSystemSendWait (FRAMEWORK_IOCTL_DRAWER_OPEN, &Req, sizeof(Req));
#endif
	usStatus = BlFwIfDrawerStatus(ulDrawer);

	return usStatus;
}


/*
*===========================================================================
** Synopsis:    USHORT BlFwIfDrawerStatus()
*
*     Input:
*
*    Output:
*
** Return:
*===========================================================================
*/
USHORT BlFwIfDrawerStatus(ULONG *ulDrawerStatus)
{
	static  void               *pLastGood = 0;
	BOOL						bResult = FALSE;
	FRAMEWORK_IO_DRAWER_STATUS	Res = {0};

	/* call framework */
	// we are doing a check here on the pointer for accessing the Framework function
	// to check the cash drawer status.  This function check, much like a canary in a mine
	// will let us know if there is some kind of a memory trampler that is overwriting
	// memory areas that should be not modified.
	if (s_pfnBlInterface) {
		if (pLastGood == 0) {
			pLastGood = s_pfnBlInterface;
		}
		if (pLastGood != s_pfnBlInterface) {
			NHPOS_ASSERT_TEXT(0, "**ERROR: BlFwIfDrawerStatus() Bad pointer s_pfnBlInterface.");
		} else {
			bResult = s_pfnBlInterface(FRAMEWORK_IOCTL_DRAWER_STATUS, &Res, sizeof(Res));
		}
	} else {
		NHPOS_ASSERT_TEXT(0, "**ERROR: BlFwIfDrawerStatus() Bad pointer s_pfnBlInterface.");
	}

	if (! bResult)
	{
        Res.dwDrawerA = BLFWIF_DRAWER_CLOSED;
        Res.dwDrawerB = BLFWIF_DRAWER_CLOSED;

		bResult = BLFWIF_DRAWER_CLOSED;
	}

	*ulDrawerStatus =  Res.dwDrawerA;
	ulDrawerStatus++;
	*ulDrawerStatus =  Res.dwDrawerB;

	return bResult;
}

/*
*===========================================================================
** Synopsis:    BOOL BlFwIfScannerNOF()
*
*     Input:
*
*    Output:
*
** Return:
*===========================================================================
*/
BOOL BlFwIfScannerNOF(USHORT usControl)
{
	BOOL						bResult;
	FRAMEWORK_IO_SCANNER_NOF	Req = {0};

	// make up parameter 02/03/01
	Req.bState = (DWORD)usControl;

	/* call framework */
	bResult = s_pfnBlInterface(FRAMEWORK_IOCTL_SCANNER_NOF, &Req, sizeof(Req));

	return bResult;
}

/*
*===========================================================================
** Synopsis:    SHORT BlFwIfScaleReadWeight()
*
*     Input:
*
*    Output:
*
** Return:
*===========================================================================
*/
SHORT BlFwIfScaleReadWeight(PBLFWIF pWeight)
{
	BOOL				bResult;
	SHORT				sResult;
	FRAMEWORK_IO_SCALE_DATA	Req = {0};

	/* call framework */
	bResult = s_pfnBlInterface (FRAMEWORK_IOCTL_SCALE_READWEIGHT, &Req, sizeof(Req));
	if (bResult)
	{
		pWeight->uchUnit = Req.uchUnit;
		pWeight->uchDec  = Req.uchDec;
		pWeight->ulData  = Req.ulData;
		sResult          = Req.sStatus;
	}
	else
	{
		memset (pWeight, 0, sizeof(BLFWIF));
		sResult = UIE_SCALE_ERROR;
	}

	return sResult;
}

/*
*===========================================================================
** Synopsis:    SHORT BlFwIfScaleReadWeight()
*
*     Input:
*
*    Output:
*
** Return:
*===========================================================================
*/
SHORT BlFwIfReadMSR (PBLFMSRIF pMsrTrackData)
{
	BOOL					bResult;
	SHORT				    sResult = 1;
	FRAMEWORK_IO_MSR_DATA	Req = {0};

	/* call framework */
	memset (pMsrTrackData, 0, sizeof(BLFMSRIF));

	bResult = s_pfnBlInterface (FRAMEWORK_IOCTL_MULTI_MSR_READ, &Req, sizeof(Req));

	if (bResult)
	{
		// Successful read of the MSR device.  Put the data into a KEYMSG
		// and send it into the data stream.
		memset (pMsrTrackData, 0, sizeof(BLFMSRIF));

		pMsrTrackData->uchLength1 = Req.uchLength1;
		memcpy(pMsrTrackData->auchTrack1, Req.auchTrack1, sizeof(pMsrTrackData->auchTrack1));
		pMsrTrackData->uchLength2 = Req.uchLength2;
		memcpy(pMsrTrackData->auchTrack2, Req.auchTrack2, sizeof(pMsrTrackData->auchTrack2));
	}
	else
	{
		sResult = UIE_SCALE_ERROR;
	}

	memset (&Req, 0, sizeof(Req));   // clear memory per the PABP recommendations for track data security.
	return sResult;
}

SHORT BlFwIfReadSignatureCapture (PBLFSIGCAPIF pSigCapData)
{
	BOOL					  bResult;
	SHORT				      sResult = 1;
	FRAMEWORK_IO_SIGCAP_DATA  Req = {0};

	/* call framework */
	bResult = s_pfnBlInterface (FRAMEWORK_IOCTL_SIGNATURECAPTURE_CAPTURE, &Req, sizeof(Req));
	if (!bResult)
	{
		sResult = UIE_SCALE_ERROR;
	}

	return sResult;
}


SHORT BlFwIfReadSignatureCustomerDisplay (TCHAR *aszFormName)
{
	BOOL					  bResult;
	SHORT				      sResult = 1;

	if (aszFormName && aszFormName[0] != 0) {
		bResult = s_pfnBlInterface (FRAMEWORK_IOCTL_SIGNATURECAPTURE_CUSTOMERDISPLAY, aszFormName, 0);

		if (!bResult)
		{
			sResult = UIE_SCALE_ERROR;
		}
	}
	return sResult;
}

SHORT BlFwIfReadSignatureClear (VOID)
{
	return BlFwIfReadSignatureCustomerDisplay (_T("FormClear"));
}

SHORT BlFwIfReadSignatureLineDisplay (VOID)
{
	return BlFwIfReadSignatureCustomerDisplay (_T("FormLineDisplay"));
}


SHORT BlFwIfReadSignatureCaptureRead (PBLFSIGCAPIF pSigCapData, USHORT usFscMajor, USHORT usFscMinor)
{
	BOOL					  bResult;
	SHORT				      sResult = 1;
	FRAMEWORK_IO_SIGCAP_DATA  Req = {0};
	TRANINFORMATION          *WorkTran;

	if (usFscMajor == FSC_TENDER && ItemTenderCheckTenderMdc(usFscMinor, 6, MDC_PARAM_BIT_D) == 0) {
		return 0;
	}

	/* call framework */
    TrnGetTranInformation(&WorkTran);
	Req.usConsNo     = WorkTran->TranCurQual.usConsNo;
	Req.ulStoreregNo = WorkTran->TranCurQual.ulStoreregNo;
	Req.ulCashierID  = WorkTran->TranModeQual.ulCashierID;
	Req.usGuestNo    = WorkTran->TranGCFQual.usGuestNo;
	Req.usHour = WorkTran->TranGCFQual.auchCheckOpen[0];
	Req.usMinute = WorkTran->TranGCFQual.auchCheckOpen[1];
	Req.usSecond = WorkTran->TranGCFQual.auchCheckOpen[2];
	Req.usYear = WorkTran->TranGCFQual.auchCheckOpen[3];
	Req.usMonth = WorkTran->TranGCFQual.auchCheckOpen[4];
	Req.usDay = WorkTran->TranGCFQual.auchCheckOpen[5];
	Req.usFscMajor = usFscMajor;
	Req.usFscMinor = usFscMinor;

	memcpy (Req.tchTranInvoiceNum, WorkTran->invno.auchInvoiceNo, sizeof(Req.tchTranInvoiceNum));
	memcpy (Req.uchUniqueIdentifier, WorkTran->TranGCFQual.uchUniqueIdentifier, sizeof(Req.uchUniqueIdentifier));
	memcpy (Req.uchUniqueIdentifierReturn, WorkTran->TranGCFQual.uchUniqueIdentifierReturn, sizeof(Req.uchUniqueIdentifierReturn));
	Req.usReasonCode = WorkTran->TranGCFQual.usReasonCode;
	Req.usReturnType = (WorkTran->TranGCFQual.usReturnType & 0x00ff);  // remove any flags that may be set

	bResult = s_pfnBlInterface (FRAMEWORK_IOCTL_SIGNATURECAPTURE_READ, &Req, sizeof(Req));
	if (bResult)
	{
		// Successful read of the MSR device.  Put the data into a KEYMSG
		// and send it into the data stream.
		if (pSigCapData != 0) {
			memset (pSigCapData, 0, sizeof(BLFSIGCAPIF));
			pSigCapData->m_SigCapMaxX = Req.m_SigCapMaxX;
			pSigCapData->m_SigCapMaxY = Req.m_SigCapMaxY;
			pSigCapData->m_SigCapPointArray = Req.m_SigCapPointArray;
			pSigCapData->m_ArrayMaxSize = Req.m_ArrayMaxSize;
			pSigCapData->m_ArrayLength = Req.m_ArrayLength;
		}
	}
	else
	{
		sResult = UIE_SCALE_ERROR;
	}

	s_pfnBlInterface (FRAMEWORK_IOCTL_SIGNATURECAPTURE_BITMAP, &Req, sizeof(Req));

	memset (&Req, 0, sizeof(Req));   // clear memory per the PABP recommendations for track data security.
	return sResult;
}



VOID  UifEnableVirtualKeyDeviceRead (VOID)
{
	BOOL					bResult;
	FRAMEWORK_IO_VIRTUALKEY_DATA	Req = {0};

	/* call framework */
	Req.usOptions = FRAMEWORK_IO_VIRTUALKEY_READ_ENABLE;
	bResult = s_pfnBlInterface (FRAMEWORK_IOCTL_COM_VIRTUAL_KEYBOARD, &Req, sizeof(Req));
}


VOID  UifDisableVirtualKeyDeviceRead (VOID)
{
	BOOL					bResult;
	FRAMEWORK_IO_VIRTUALKEY_DATA	Req = {0};

	/* call framework */
	Req.usOptions = FRAMEWORK_IO_VIRTUALKEY_READ_DISABLE;
	bResult = s_pfnBlInterface (FRAMEWORK_IOCTL_COM_VIRTUAL_KEYBOARD, &Req, sizeof(Req));
}

/*
*===========================================================================
** Synopsis:    VOID BlFwIfWriteToLineDisplay(USHORT usDispId, USHORT usRow,
*											USHORT usColumn, TCHAR *puchString,
*											USHORT usChars)
*
*
*     Input:
*
*    Output:
*
** Return:
*===========================================================================
*/
VOID BlFwIfWriteToLineDisplay(USHORT usDispId, USHORT usRow, USHORT usColumn,
							  TCHAR *puchString, USHORT usChars)
{
	FRAMEWORK_IO_DISPLAY_DATA	Data = {0};

	// make up parameter
    Data.nRow         = (LONG)usRow;
    Data.nColumn      = (LONG)usColumn;
    Data.uchAttribute = 0;                  /* not use */
    Data.nChars       = (LONG)usChars;
    Data.lpuchBuffer  = puchString;

	/* call framework */
	s_pfnBlInterface(FRAMEWORK_IO_LINEDISPLAY_WRITE, &Data, sizeof(Data));
}
/*
*===========================================================================
** Synopsis:    VOID	BlFwIfGetLineDisplayInformation(TCHAR *puchServiceObject, 
																LONG *lColumns);
*
*
*     Input:
*
*    Output:
*
** Return:
*===========================================================================
*/
VOID BlFwIfGetLineDisplayInformation(TCHAR *puchServiceObject, LONG *lColumns, LONG *lRows)
{
	FRAMEWORK_IO_DISPLAY_DATA	Data = {0};

	/* call framework */
	s_pfnBlInterface(FRAMEWORK_IO_LINEDISPLAY_GETSO, &Data, sizeof(Data));

	_tcscpy(puchServiceObject, Data.tchDeviceDescription);
	*lColumns = Data.nColumn;
	*lRows = Data.nRow;
}

ULONG BlFwIfPrint(LONG lPrtType, TCHAR *pucBuff, USHORT usLen, BLFDEVERROR *pArgError)
{
	TRANITEMIZERS       *WorkTI;
	ULONG                ulResultCode;
	FRAMEWORK_IO_PRINTER ArgWrite = {0};

	TrnGetTI(&WorkTI);
	if (WorkTI->blDblShrdPrinting == TRUE)
		ArgWrite.lColumns = 1;

	ArgWrite.lPrintStation = lPrtType;
	ArgWrite.ulLength = usLen;
	ArgWrite.ptchBuffer = pucBuff;

	ulResultCode = s_pfnBlInterface(FRAMEWORK_IOCTL_PRINTER_WRITE, &ArgWrite, sizeof(ArgWrite));

	if (pArgError) {
		pArgError->lPrintStation = ArgWrite.lPrintStation;
		pArgError->m_ulPrinter1StatusMap = ArgWrite.m_ulPrinter1StatusMap;
		pArgError->dwErrorCode = ArgWrite.dwErrorCode;
		pArgError->resultCode = ArgWrite.resultCode;
		pArgError->extResultCode = ArgWrite.extResultCode;
		pArgError->fbStatus = ArgWrite.fbStatus;
	}
	return (ArgWrite.dwErrorCode == DEVICEIO_PRINTER_MSG_SUCCESS_ADR) ? 1 : 0;
}

ULONG BlFwIfInitLoadLogo(VOID)
{
	FRAMEWORK_IO_PRINTER ArgWrite = {0};

	ArgWrite.lPrintStation = 2;
#if 1
	return s_pfnBlInterface(FRAMEWORK_IOCTL_PRINTER_INIT_LOADLOGO, &ArgWrite, sizeof(ArgWrite));
#else
	return UifDeviceMessageSystemSendWait (FRAMEWORK_IOCTL_PRINTER_INIT_LOADLOGO, &ArgWrite, sizeof(ArgWrite));
#endif
}

USHORT BlFwGetPrinterStatus(LONG lPrtType, USHORT* pfbStatus)
{
	BOOL	bRet;
	FRAMEWORK_IO_PRINTER Arg = {0};

	Arg.lPrintStation = lPrtType;
#if 1
	bRet = s_pfnBlInterface(FRAMEWORK_IOCTL_PRINTER_STATUS, &Arg, sizeof(Arg));
#else
	bRet = UifDeviceMessageSystemSendWait (FRAMEWORK_IOCTL_PRINTER_STATUS, &Arg, sizeof(Arg));
#endif
	*pfbStatus = Arg.fbStatus;
	return bRet;
}

VOID BlFwGetPrinterColumnsStatus(LONG lPrtType, USHORT* pusColumn, USHORT* pfbStatus)
{
	BOOL	bRet;
	FRAMEWORK_IO_PRINTER Arg = {0};

	*pfbStatus = 0;
	Arg.lPrintStation = lPrtType;
#if 1
	bRet = s_pfnBlInterface(FRAMEWORK_IOCTL_PRINTER_COLUMN_STATUS, &Arg, sizeof(Arg));
#else
	bRet = UifDeviceMessageSystemSendWait (FRAMEWORK_IOCTL_PRINTER_COLUMN_STATUS, &Arg, sizeof(Arg));
#endif
	if(bRet){
		NHPOS_ASSERT(Arg.lColumns <= MAXWORD);
		*pusColumn = (USHORT)Arg.lColumns;
	}else{
		*pusColumn = 0;
	}

	if(Arg.bSlipStationExist){
		*pfbStatus = PMG_VALIDATION;
	}
}

VOID BlFwPmgWait(VOID)
{
	FRAMEWORK_IO_PRINTER Arg = {0};

	s_pfnBlInterface(FRAMEWORK_IOCTL_PRINTER_WAIT, &Arg, sizeof(Arg));
}

ULONG BlFwPmgGetLastError (VOID)
{
	return BlFwPmgLastError;
}

USHORT BlFwPmgCallBack(ULONG dwErrorCode, SHORT  usSilent)
{
	BlFwPmgLastError = dwErrorCode;
	return PmgCallBack(dwErrorCode, usSilent);
}


// Printer error handler used by print spooler allow for printer errors to 
// be reported through the Connection Engine Interface and allow a remote
// application to display an error dialog and send a response back to GenPOS.
USHORT BlFwPmgCallBackCheckOposReceipt(ULONG dwErrorCode)
{
	USHORT  fbPrtStatus1 = 0;                /* printer status */
	USHORT  fbPrtStatus2 = 0;                /* printer status */

	PmgGetStatusOnly((PMG_PRT_REPORT_ERROR | PMG_PRT_RECEIPT), &fbPrtStatus1);
	// bit PRT_STAT_ERROR is used to indicate NOT an error condition for some reason.
	fbPrtStatus1 &= ~(PRT_STAT_ERROR | PRT_STAT_INIT_END);

	if ((flDispRegDescrControl & PRT_OFFLINE_CNTRL) != 0 || (flDispRegKeepControl & PRT_OFFLINE_CNTRL) != 0) {
		if (fbPrtStatus1 == 0) {
			flDispRegDescrControl &= ~PRT_OFFLINE_CNTRL;
			flDispRegKeepControl &= ~PRT_OFFLINE_CNTRL;
			RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_0);
			PifSleep(50);
		}
		return fbPrtStatus1;
	}

	if (fbPrtStatus1) {
		CONNENGINE_PRINTSPOOLER_DIALOG  EventDialog = {0};
		STATECHANGE    StateChangeDefaults = {0};

		StateChangeDefaults.uleventid = EventDialog.ulEventId = 32767;
		EventDialog.iDialogId = 1;
		EventDialog.tcsTitle = L"Printer Error";
		EventDialog.tcsText = L"Please Check Printer";
		EventDialog.iNoOptions = 1;
		EventDialog.tcsOptions[0] = L"Ok";
		flDispRegDescrControl |= PRT_OFFLINE_CNTRL;
		flDispRegKeepControl |= PRT_OFFLINE_CNTRL;
		RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_0);
		PifSleep(50);
		UieMessageBoxNonModalPopUp (EventDialog.tcsText, EventDialog.tcsTitle, 1, CONNENGINE_SERVICE_PRINTSPOOLER, EventDialog.ulEventId);
		BlRegisterStateChangeEvent (CONNENGINE_SERVICE_PRINTSPOOLER, EventDialog.ulEventId);
		BlSetStateChangeEvent (CONNENGINE_SERVICE_PRINTSPOOLER, EventDialog.ulEventId, 0, 0, &StateChangeDefaults);
		ConnEngineSendStateChange (L"event", CONNENGINE_SERVICE_PRINTSPOOLER, EventDialog.ulEventId, &EventDialog);
	}

	fbPrtStatus2 = fbPrtStatus1;
	while (fbPrtStatus2) {
		if (BlCheckStateChangeEvent (CONNENGINE_SERVICE_PRINTSPOOLER, 32767, 0) > 0) {
			break;
		}
		PifSleep (100);
		PmgGetStatusOnly((PMG_PRT_REPORT_ERROR | PMG_PRT_RECEIPT), &fbPrtStatus2);
		fbPrtStatus2 &= ~(PRT_STAT_ERROR | PRT_STAT_INIT_END);
	}
	if (fbPrtStatus1) {
		dwErrorCode = 0;
		UieMessageBoxNonModalPopDown ();
		BlRegisterStateChangeEvent (CONNENGINE_SERVICE_PRINTSPOOLER, 0);
		ConnEngineSendStateChange (L"event-clear", CONNENGINE_SERVICE_PRINTSPOOLER, 32767, 0);
	}

	if (fbPrtStatus2 == 0) {
		flDispRegDescrControl &= ~PRT_OFFLINE_CNTRL;
		flDispRegKeepControl &= ~PRT_OFFLINE_CNTRL;
		RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_0);
		PifSleep(50);
	}

	return fbPrtStatus1;
}


VOID BlFwBeginInsertion(LONG lTimeout)
{
	FRAMEWORK_IO_PRINTER Arg = {0};

	Arg.lTimeout = lTimeout;
	s_pfnBlInterface(FRAMEWORK_IOCTL_PRINTER_BEG_INS, &Arg, sizeof(Arg));
}

VOID BlFwEndInsertion()
{
	FRAMEWORK_IO_PRINTER Arg = {0};

	s_pfnBlInterface(FRAMEWORK_IOCTL_PRINTER_END_INS, &Arg, sizeof(Arg));
}

VOID BlFwBeginRemoval(LONG lTimeout)
{
	FRAMEWORK_IO_PRINTER Arg = {0};

	Arg.lTimeout = lTimeout;
	s_pfnBlInterface(FRAMEWORK_IOCTL_PRINTER_BEG_REM, &Arg, sizeof(Arg));
}

VOID BlFwEndRemoval()
{
	FRAMEWORK_IO_PRINTER Arg = {0};

	s_pfnBlInterface(FRAMEWORK_IOCTL_PRINTER_END_REM, &Arg, sizeof(Arg));
}

/*
*===========================================================================
** Synopsis:    VOID BlFwIfWriteToLineDisplay()
*
*
*     Input:    NONE
*
*    Output:    NONE
*
** Return:
** Description:  This function calls to reload the mnemonic that was previously
**				 displayed on the terminal.
*===========================================================================
*/
VOID BlFwIfDisplayWrite()
{
	/*This Switch Disp Recovery Function recalls the saved information that was last
	displayed in the operator display.*/
	UieBuf2SwitchDispRecovery(UIE_OPER, UieRecovery.auchOperDisp, NULL);

	/*This recalls the discriptor on the screen, if any are to be shown*/
    UieDisplayEchoBackDesc(UieEcho.flDescriptor);

	memset (UieRecovery.flOperDesc, 0, sizeof(UieRecovery.flOperDesc));
	NbDescriptor(NB_REDISPLAY);
}

/*
*===========================================================================
** Synopsis:    SHORT BlFwIfDispenseCoin(LONG lDispenseAmount)
*
*
*     Input:    LONG lDispenseAmount
*
*    Output:

*
** Return:		UIE_SUCCESS                - successful
*               UIE_COIN_ERROR_NOT_PROVIDE - coin dispenser not provide
*               UIE_COIN_ERROR_RANGE       - range error
*               UIE_COIN_ERROR_COM         - communication error
*
** Description:  This function sends the amount of coins to be dispensed to
				the OPOS Control
*===========================================================================
*/
SHORT BlFwIfDispenseCoin(DCURRENCY lDispenseAmount)
{
	FRAMEWORK_IO_COIN_DATA dispenseData = {0};

	dispenseData.lDispenseAmount = (LONG) lDispenseAmount;
	s_pfnBlInterface(FRAMEWORK_IOCTL_COIN_DISPENSE, &dispenseData, sizeof(dispenseData));

	return dispenseData.sReturnCode;
}


/*
*===========================================================================
** Synopsis:    SHORT BlFwIfMDCCheck(SHORT usAddress, UCHAR uchMaskData)
*
*
*     InPut:    usAddress  - MDC address
*               uchMaskData - MDC bit mask
*
** Return:      Masked Bit Patern
*
** Description: This function checks MDC parameter on system RAM.
*===========================================================================
*/
SHORT BlFwIfMDCCheck(SHORT usAddress, UCHAR uchMaskData)
{
	return ParaMDCCheck(usAddress, uchMaskData);
}


/*
*===========================================================================
** Synopsis:    SHORT BlFwIfCheckMldStatus(VOID)
*
*
*     InPut:    usAddress  - MDC address
*               uchMaskData - MDC bit mask
*
** Return:      Masked Bit Patern
*
** Description: This function checks MDC parameter on system RAM.
*===========================================================================
*/
USHORT BlFwIfCheckMldSystemStatus()
{
	return uchMldSystem;
}


/*===========================================================================
** Synopsis:    VOID	CliParaRead(VOID *pDATA);
*     InOut:    pDATA   - Data Pointer
*
** Return:      none.       
*
** Description: This function reads parameter on system RAM.   
*===========================================================================
*/
VOID   BlFwIfParaRead(VOID *pDATA)
{
    ParaRead(pDATA);
}
/*
*===========================================================================
** Synopsis:    SHORT BlFwIfMDCCheck(SHORT usAddress, UCHAR uchMaskData)
*
*
*     InPut:    usAddress  - MDC address
*               uchMaskData - MDC bit mask
*
** Return:      Masked Bit Patern
*
** Description: This function sets the font for the selected line
*===========================================================================
*/
VOID	BlFwIfPrintSetFont(LONG lPrtType , USHORT usFont) //ESMITH EJ FONT
{
	FRAMEWORK_IO_PRINTER Arg = {0};

	Arg.lPrintStation = lPrtType;
	Arg.usFontType = usFont;
	s_pfnBlInterface(FRAMEWORK_IOCTL_PRINTER_FONT, &Arg, sizeof(Arg));
}

/*
*===========================================================================
** Synopsis:    SHORT BlFwIfPrintGetSetPrtCap(SHORT usAddress, UCHAR uchMaskData)
*
*
*     InPut:    usAddress  - MDC address
*               uchMaskData - MDC bit mask
*
** Return:      Masked Bit Patern
*
** Description: This function sets the font for the selected line
*===========================================================================
*/
LONG	BlFwIfPrintGetSetPrtCap(LONG lPrtType, USHORT usPrtCap, LONG lValue) //ESMITH EJ FONT
{
	FRAMEWORK_IO_PRINTER Arg = {0};

	Arg.lPrintStation = lPrtType;
	Arg.usFontType = usPrtCap;
	Arg.lColumns = lValue;
	s_pfnBlInterface(FRAMEWORK_IOCTL_PRINTER_GETSETCAP, &Arg, sizeof(Arg));

	return Arg.lTimeout;
}

/*
*===========================================================================
** Synopsis:    SHORT BlFwIfPrintPrintBarCode(LONG lPrtType, TCHAR *pucBuff, ULONG ulTextFlags, ULONG ulCodeFlags)
*
*
*     InPut:    usAddress  - MDC address
*               uchMaskData - MDC bit mask
*
** Return:      Masked Bit Patern
*
** Description: This function sets the font for the selected line
*===========================================================================
*/
ULONG  BlFwIfPrintPrintBarCode(LONG lPrtType, TCHAR *pucBuff, ULONG ulTextFlags, ULONG ulCodeFlags, BLFDEVERROR *pArgError)
{
	ULONG   ulResultCode = 0;
	FRAMEWORK_IO_PRINTER ArgWrite = {0};

	ArgWrite.lPrintStation = lPrtType;
	ArgWrite.ptchBuffer = pucBuff;
	ArgWrite.ulAlignment = ulTextFlags;
	ArgWrite.ulSymbology = ulCodeFlags;

	ulResultCode = s_pfnBlInterface(FRAMEWORK_IOCTL_PRINTER_PRINTBARCODE, &ArgWrite, sizeof(ArgWrite));
	if (pArgError) {
		pArgError->lPrintStation = ArgWrite.lPrintStation;
		pArgError->m_ulPrinter1StatusMap = ArgWrite.m_ulPrinter1StatusMap;
		pArgError->dwErrorCode = ArgWrite.dwErrorCode;
		pArgError->resultCode = ArgWrite.resultCode;
		pArgError->extResultCode = ArgWrite.extResultCode;
		pArgError->fbStatus = ArgWrite.fbStatus;
	}

	return 	ulResultCode;
}

//This section of Fucntions deal with security
//There are various ways to get the information you may need
//Read the Descriptions of the Functions
/*
*===========================================================================
** Synopsis:    int BlFwIfLicStatus(VOID)
*
*
*     Input:
*
** Return:
*
** Description: This function calls to security to notify of status.
				If you want the status and you use this function, make
				sure that you set the location to put the status by calling
				BlFwIfSecStatus or BlFwIfSecStatusProp functions prior
				to calling BlFwIfLicStatus. The serial number and expire value
				can be retrived after calling BlFwIfLicStatus by
				using BlFwIfReadSerial and BlFwIfReadExpStr
*===========================================================================
*/
int BlFwIfLicStatus(VOID)
{
	AXDATA Arg = {0};

#if 1
	// send message to DeviceEngine for license data rather than the
	// previous license control method.
	if(s_pfnBlInterface){
		s_pfnBlInterface(FRAMEWORK_IOCTL_LIC_DETAILS, &Arg, sizeof(Arg));
	}
#else
	if(s_pfnBlAXInterface){
		s_pfnBlAXInterface(FRAMEWORK_AXCTL_LIC_DETAILS, &Arg, sizeof(Arg));
	}
#endif

	return Arg.licStatus;    // return value of m_licStatus from LoadLicenseKeyFile()
}


/*
*===========================================================================
** Synopsis:    VOID BlFwIfReadSerial(TCHAR* szSerial)
*
*
*     Input:
*
** Return:
*
** Description: This function gets the serial number from a fired event.
*===========================================================================
*/
VOID BlFwIfReadSerial(TCHAR* szSerial)
{
	AXDATA Arg = {0};

	Arg.ptchUnlock = szSerial;
	if(s_pfnBlAXInterface)
	{
		s_pfnBlAXInterface(FRAMEWORK_AXCTL_LIC_SERIAL, &Arg, sizeof(Arg));
	}
}

/*
*===========================================================================
** Synopsis:    VOID BlFwIfLicReg(TCHAR* szUlCode)
*
*
*     Input:    Unlock Code
*
** Return:
*
** Description: This function registers the software with the unlock code.
*===========================================================================
*/
VOID BlFwIfLicReg(TCHAR* szUlCode, TCHAR* szStatus)
{
	AXDATA Arg = {0};

	Arg.ptchUnlock = szUlCode;
	Arg.ptchStatus = szStatus;
	if(s_pfnBlAXInterface){
		s_pfnBlAXInterface(FRAMEWORK_AXCTL_LIC_REG, &Arg, sizeof(Arg));
	}
}

/*
*===========================================================================
** Synopsis:    VOID BlFwIfReadExpStr(TCHAR* szExpireVal)
*
*
*     Input:
*
** Return:
*
** Description: This function gets the Expire Value(the number of days left)
*				string from a fired event.
*===========================================================================
*/
VOID BlFwIfReadExpStr(TCHAR* szExpireVal)
{
	AXDATA Arg = {0};

	Arg.ptchUnlock = szExpireVal;
	if(s_pfnBlAXInterface){
		s_pfnBlAXInterface(FRAMEWORK_AXCTL_LIC_EXPIRE, &Arg, sizeof(Arg));
	}
}


/*
*===========================================================================
** Synopsis:    VOID BlFwIfReadExpStrProp(TCHAR* szExpireVal)
*
*
*     Input:
*
** Return:
*
** Description: This function gets the Expire Value(number of days left)
				string by reading the property from the Control.
*===========================================================================
*/
LONG BlFwIfReadExpProp(VOID)
{
	LONG  ExpProp = -1;

	if(s_pfnBlAXExp)
	{
		ExpProp = s_pfnBlAXExp(NULL);
	}

	return ExpProp;
}

/*
*===========================================================================
** Synopsis:    VOID BlFwIfReadExpStr(TCHAR* szExpireVal)
*
*
*     Input:
*
** Return:
*
** Description: This function provides means to set the Location of the
				Status Bitmap.
				See Also BlFwIfSecStatusProp
*===========================================================================
*/
VOID BlFwIfSecStatus(ULONG* plStatus)
{
	AXDATA Arg = {0};

	Arg.pulong = plStatus;
	if(s_pfnBlAXInterface){
		s_pfnBlAXInterface(FRAMEWORK_AXCTL_LIC_PUTSTATUS, &Arg, sizeof(Arg));
	}
}

/*
*===========================================================================
** Synopsis:    VOID BlFwIfReadExpStrProp(TCHAR* szExpireVal)
*
*
*     Input:
*
** Return:
*
** Description: This function sets the Expire Value(number of days left)
				string by reading the property from the Control.
*===========================================================================
*/
VOID BlFwIfSecStatusProp(ULONG* plStatus)
{
	if(s_pfnBlAXSt){
		s_pfnBlAXSt(plStatus);
	}
}


/*
*===========================================================================
** Synopsis:    VOID BlFwIfNagScreen()
*
*
*     Input:
*
** Return:
*
** Description: This function calls the nag screen based on which security
				control is being used.
*===========================================================================
*/
VOID BlFwIfNagScreen()
{
	AXDATA Arg = {0};

	if(s_pfnBlNagScreen){
		s_pfnBlNagScreen();
	}
}


/*
*===========================================================================
** Synopsis:    VOID BlFwIfPrintFileWrite(LONG lStation,
*						                  TCHAR *lptData,
*									      USHORT usLen)
*
*
*     Input: szData - data string
*			 usLen	 - length of string
*			 szFileName - path/file to write data to
*
** Return: NONE
*
** Description: This function will take a set of data and write it to
*				a specified file. The data length can not exceed 512.
*
*               This supports the Display to File, indicated with @/For key,
*               in Supervisor Mode along with the Display to Windows, indicated
*               with #/Type key.  This function will write the printer output
*               to a file so that it can be viewed in a text editor or sent
*               to a laser printer.
*===========================================================================
*/ //ESMITH PRTFILE
VOID	BlFwIfPrintFileWrite(TCHAR *szData, USHORT usDataLen, TCHAR *szFileName)
{
	if(usDataLen) {
		HANDLE hHandle;

		hHandle = CreateFile(szFileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		if (hHandle) {
			DWORD dwReturn;

			dwReturn = SetFilePointer(hHandle, 0, NULL, FILE_END);

			if (dwReturn != 0xFFFFFFFF) {
				DWORD  dwNumberOfBytes;
				int    wszStart = 0;
				TCHAR  wszLog[MAX_PRINT_BUFFER + 1] = {0};

				memcpy(wszLog, szData, usDataLen * 2);
				do {
					int     j;
					TCHAR   *wszLogPtr = wszLog + wszStart;
					USHORT  usLen;
					CHAR    szLog[MAX_PRINT_BUFFER+1] = {0};

					// find the end of a substring for an entire print line.
					// an entire print line is a single line to be printed and the
					// provided text buffer may contain more than one print line.
					// end of print line is either a 0x0A (new line) or 0x0D followed by 0x0A
					// which is the Windows end of line.
					for (usLen = 0; usLen < usDataLen; usLen++) {
						if (wszLogPtr[usLen] == 0x0A || wszLogPtr[usLen] == 0) {
							break;
						}
					}
					if (usLen < 1) break;
					wszLogPtr[usLen] = 0;  // end of string
					wcstombs(szLog, wszLogPtr, sizeof(szLog));
					wszStart += usLen + 1;
					usLen = strlen (szLog);
					szLog[usLen] = 0x0A;
					usLen++;

					/*
						The buffer being sent in is expected to contain
						data that will be going to the printer. Among these
						characters are the following line feed and paper cut,
						since these values do not end in return characters or
						new line symbols we must add them prior to printing these
						values.
					*/
					for(j = 0; j < usLen; ) {
						if (szLog[j] == 0x0A) {
							szLog[j] = 0x0D;
							szLog[j+1] = 0x0A;
							j += 2;
							usLen++;
						} else if (szLog[j] == 0x0D && szLog[j+1] == 0x0A) {
							j += 2;
						} else if (szLog[j] == ESC && szLog[j+1] == _T('|')) {
							if (szLog[j+3] == _T('C')) {
								// PRT_SDOUBLE or PRT_DOUBLE character size
								szLog[j] = szLog[j+1] = szLog[j+2] = szLog[j+3] = _T(' ');
								j += 4;
							}
							else if (szLog[j+3] == _T('A')) {
								// PRT_CENTERED or Tab
								// default tab positions are every 8 characters so tab positions are
								// at character positions 9, 17, 25, ...
								// ESC|rA indicates to right justify and tab over
								if (szLog[j+2] == 'r') {
									int kMove = 48 - j;
									kMove -= (usLen - j);
									if (kMove > 0) {
										memmove (szLog + j + kMove, szLog + j + 4, usLen);
										memset (szLog + j, ' ', kMove);
										j += kMove;
										usLen += kMove - 4;
									} else {
										j++;
									}
								} else {
									if (((j + 1) % 8) == 1) {
										memmove (szLog + j, szLog + j + 4, usLen);
										usLen -= 4;
										j++;
									} else {
										int  kMove = 8 - (j + 1) % 8;
										if (kMove > 0) {
											memmove (szLog + j + kMove, szLog + j + 4, usLen);
											memset (szLog + j, ' ', kMove);
											j += kMove;
											usLen += kMove - 4;
										} else {
											j++;
										}
									}
								}
							}
							else if (szLog[j+4] == _T('F')) {
								szLog[j] = szLog[j+1] = szLog[j+2] = _T(' ');
								szLog[j+3] = 0x0D;
								szLog[j+4] = 0x0A;
								j += 5;
							}
							else if (szLog[j+5] == _T('P')) {
								// Paper Cut
								szLog[j] = szLog[j+1] = _T(' ');
								szLog[j+2] = 0x0D;
								szLog[j+3] = 0x0A;
								szLog[j+4] = 0x0D;
								szLog[j+5] = 0x0A;
								j += 6;
							} else {
								// handle unknown printer escape sequences that may be used.
								szLog[j] = szLog[j+1] = _T(' ');
								j += 2;
							}
						} else {
							j++;
						}
					}
					WriteFile(hHandle, szLog, usLen , &dwNumberOfBytes, NULL);
				} while (wszStart < usDataLen);
			}

			CloseHandle(hHandle);
		}
	}
}


TCHAR BlFwIfGetPrintType(VOID)
{
	FRAMEWORK_IO_PRINTER ArgWrite = {0};

	if (s_pfnBlInterface)
		s_pfnBlInterface(FRAMEWORK_IOCTL_PRINTER_TYPE, &ArgWrite, sizeof(ArgWrite));

	return ArgWrite.ptchBuffer[0];
}

/*
*===========================================================================
** Synopsis:    VOID BlFwIfDefaultUserScreen(VOID)
*
*
*     Input:	none
*	
** Return:		none
*
** Description: This function interfaces with Framework so that we can
				bring up the default window
*===========================================================================
*/
VOID BlFwIfDefaultUserScreen()
{
	DWORD   dwLastError = 0;
    BOOL    status;
	HANDLE  hWnd = PifGetWindowHandle(0);
		
	if (hWnd) {
		status = PostMessage(hWnd, (UINT)WU_EVS_DFLT_LOAD, (WPARAM)0, (LPARAM)0);
		dwLastError = GetLastError();
		PifSleep(0);
	}
}

/*
*===========================================================================
** Synopsis:    VOID BlFwIfGroupWindowShow(ULONG ulMaskGroupPermission)
*
*
*     Input:	ULONG ulMaskGroupPermission 
*	
** Return:		none
*
** Description: This function interfaces with Framework so that we can
*               indicate whether windows which have at least one matching
*               group in their group permissions mask should be shown or
*               displayed.
*               This function allows for a set of windows to be designated
*               as special purpose windows that should only be displayed if
*               the operator's group permissions has at least one permission
*               bit that is also specified in the window's group permissons.
*
*               The BlFwIfGroupWindowShow() function is to show those windows
*               and the BlFwIfGroupWindowHide() function is to hide those windows.
*
*               To hide all windows, pass a mask of all ones, 0xffffffff, with BlFwIfGroupWindowHide().
*               To show all windows, pass a mask of all ones, 0xffffffff, with BlFwIfGroupWindowShow().
*
*               Normally the mask used with these functions is the group permission mask from
*               the Operator Record created with AC20.
*
*               See class method CFrameworkWndDoc::OnShowHideGroupMsgRcvd()
*===========================================================================
*/
VOID BlFwIfGroupWindowShow(ULONG ulMaskGroupPermission)
{
	DWORD   dwLastError = 0;
    BOOL    status;
	HANDLE  hWnd = PifGetWindowHandle(0);
		
	if (hWnd) {
		status = PostMessage(hWnd, (UINT)WM_APP_SHOW_HIDE_GROUP, (WPARAM)ulMaskGroupPermission, (LPARAM)1);
		dwLastError = GetLastError();
		PifSleep(0);
	}
}

VOID BlFwIfGroupWindowHide(ULONG ulMaskGroupPermission)
{
	DWORD   dwLastError = 0;
    BOOL    status;
	HANDLE  hWnd = PifGetWindowHandle(0);
		
	if (hWnd) {
		status = PostMessage(hWnd, (UINT)WM_APP_SHOW_HIDE_GROUP, (WPARAM)ulMaskGroupPermission, (LPARAM)2);
		dwLastError = GetLastError();
		PifSleep(0);
	}
}

/*
*===========================================================================
** Synopsis:    VOID BlFwIfWindowTranslate(ULONG ulBoolTranslate)
*
*
*     Input:	ULONG ulBoolTranslate 
*	
** Return:		none
*
** Description: This function interfaces with Framework so that we can
*               indicate whether windows should be translated or moved so
*               that windows that are arranged for a right handed person
*               using their right hand for data entry can be automatically
*               rearranged so that the windows are displayed suitable for
*               a left handed person or vice versa.
*               The idea is that a left handed person can have the controls
*               arranged on the screen so that they can see the screen without
*               having their view blocked by their hand.
*
*               See class method CFrameworkWndDoc::OnShowHideGroupMsgRcvd()
*===========================================================================
*/
VOID BlFwIfWindowTranslate(ULONG ulBoolTranslate)
{
	DWORD   dwLastError = 0;
    BOOL    status;
	HANDLE  hWnd = PifGetWindowHandle(0);
		
	if (hWnd) {
		status = PostMessage(hWnd, (UINT)WM_APP_SHOW_HIDE_GROUP, (WPARAM)ulBoolTranslate, (LPARAM)3);
		dwLastError = GetLastError();
		PifSleep(0);
	}
}


/*
*===========================================================================
** Synopsis:    UCHAR BlFwIfSetPresetPLUNumber(UCHAR *ptchPLUNumber))
*
*
*     Input:	Keyboard Type
*	
** Return:		none
*
** Description: This function is used to create a temporary PLU preset in
*               what amounts to a slot in a circular buffer so that a message
*               with this preset location can be inserted into the UI ring
*               buffer allowing the Touchscreen button PLU to be processed
*               as if the button specified a preset PLU instead of a
*               PLU number.
*               For use with the CUSTOMS SCER for Release 2.X JHHJ 7-28-05
*===========================================================================
*/
UCHAR BlFwIfSetPresetPLUNumber(UCHAR *ptchPLUNumber)
{
	PARAPLUNOMENU myPLUNumber = {0};
	TCHAR		tchPLU[PLU_MAX_DIGIT + 1] = {0};
	size_t		i, size;

	size = strlen(ptchPLUNumber);
	if (size > PLU_MAX_DIGIT) size = PLU_MAX_DIGIT;
	for(i =0; i < size; i++, ptchPLUNumber++)
	{
		tchPLU[i] = (TCHAR)*ptchPLUNumber;
	}
	
	_tcsncpy(myPLUNumber.PluNo.aszPLUNumber, tchPLU, PLU_MAX_DIGIT);
	// ParaPLUNoMenuWriteTouch() should return values in the range of 1 to PLU_MAX_TOUCH_PRESET
	// This indicates the address in Para.ParaPLUNoMenu[] where the PLU number was placed.
	return ParaPLUNoMenuWriteTouch(&myPLUNumber);
}

/*
*===========================================================================
** Synopsis:    USHORT BlFwIfUifACChgKBType(USHORT usKBType)
*
*
*     Input:	Keyboard Type
*	
** Return:		Previous keyboard type that was set.
*
** Description: This functions changes the conversion table of NHPOS from
*				Keyboard data to Alphanumeric data.  This is for when users
*				need to Input character data, for use with the CUSTOMS SCER
*				for Release 2.X JHHJ 7-28-05
*===========================================================================
*/
USHORT BlFwIfUifACChgKBType(USHORT usKBType)
{
    return UifACChgKBType((UCHAR)usKBType);
}

/*
*===========================================================================
** Synopsis:    LONG BlFwIfGetCodePage(VOID)
*
*
*     Input:	Keyboard Type
*	
** Return:		none
*
** Description: We Get the code page so that we can assign the line display the correct
*				line display
*===========================================================================
*/
LONG BlFwIfGetCodePage(VOID)
{
    SYSCONFIG CONST *pSys = PifSysConfig();
	PARATERMINALINFO TermInfo = {0};

	TermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
	TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(pSys->usTerminalPositionFromName);
	ParaTerminalInfoParaRead (&TermInfo);

    return 	TermInfo.TerminalInfo.ulCodePage;
}

/*
*===========================================================================
** Synopsis:    LONG BlFwIfSetOEPGroups(VOID)
*
*
*     Input:	Keyboard Type
*	
** Return:		none
*
** Description: we need to send frameworkwndtext this information for the OEP
*               window group numbers from the OEP table provisioned.
*               in addition this is used for special purpose OEP group numbers
*               to pop up an OEP window for special purposes such as Preauth Capture
*               or Reason Code selection.
*
*               Special OEP pseudo group number assignments
*                 - 201 (\xc9) BL_OEPGROUP_STR_PREAUTH Preauth Capture display in function ItemDisplayPreauthChooseDialog()
*                 - 202 (\xca) BL_OEPGROUP_STR_REASONCODE Reason Code display in function ItemDisplayOepChooseDialog()
*                 - 203 (\xcb) BL_OEPGROUP_STR_AUTOCOUPON Auto Coupon display in function ItemAutoCouponCheck()
*
*               we must use BlFwIfSetOEPGroups() to set the group before calling
*               the function MldCreatePopUpRectWindowOnly() in order to prevent
*               OEP window close issues the first time the OEP window is used.
*               we have seen issues in which the first time a group specific OEP window
*               displays the OEP window does not disappear and remains displayed though
*               useless.
*===========================================================================
*/
LONG BlFwIfSetOEPGroups(UCHAR *pszGroups, UCHAR uchTableNo)    // see also BOOL CALLBACK BlFwSetOepGroup(UCHAR * pszGroups);
{
	LONG  lRet = 0;    // UIF_SUCCESS and other success codes use zero.

	if(s_pfnBlSetOepGroups)          // CMD_SET_OEP_GROUPS
	{
		s_pfnBlSetOepGroups(pszGroups, uchTableNo);     // normally call back is set to BlFwSetOepGroup().
	} else {
		NHPOS_ASSERT_TEXT((s_pfnBlSetOepGroups != 0), "==PROVISION: BlFwIfSetOEPGroups() called but no OEP windows exist in layout.");
		lRet = -1;    // -1 is a generic fail code 
	}
	return lRet;
}

LONG BlFwIfDisplayOEPImagery (UCHAR uchCommand, UCHAR uchDeptNo, UCHAR uchTableNo, UCHAR uchGroupNo)    // see also BlFwDisplayOEPImagery (UCHAR uchDeptNo, UCHAR uchTableNo, UCHAR uchGroupNo);
{
	LONG  lRet = 0;    // UIF_SUCCESS and other success codes use zero.

	if(s_pfnBlDisplayOEPImagery)    // CMD_DISPLAY_OEP_IMAGE
	{
		s_pfnBlDisplayOEPImagery(uchCommand, uchDeptNo, uchTableNo, uchGroupNo);     // normally call back is set to BlFwDisplayOEPImagery().
	} else {
		NHPOS_ASSERT_TEXT((s_pfnBlDisplayOEPImagery != 0), "==PROVISION: BlFwIfDisplayOEPImagery() called but no OEP windows exist in layout.");
		lRet = -1;    // -1 is a generic fail code 
	}
	return lRet;
}

/*
*===========================================================================
** Synopsis:    VOID BlFwIfGetMaxOEPButtons(UINT *pszMaxButtons)
*
*
*     Input:	Keyboard Type
*	
** Return:		none
*
** Description: we need to get this information from frameworkwndtxt
*===========================================================================
*/
int BlFwIfGetMaxOEPButtons(UCHAR *pszMaxButtons)
{
	int maxButtons = STD_OEP_MAX_NUM;

	if(s_pfnBlGetMaxOepButtons)
	{
		maxButtons = s_pfnBlGetMaxOepButtons(pszMaxButtons);
		if (maxButtons == 0)
		{
			maxButtons = STD_OEP_MAX_NUM;
		}
	}
	return maxButtons;
}



//WINDOWS PRINTER FUNCTIONS

VOID BlFwIfPrintWin(TCHAR *pucBuff, USHORT usLen)
{
	FRAMEWORK_IO_PRINTER ArgWrite = {0};

	//ArgWrite.lPrintStation = lPrtType;
	ArgWrite.ulLength = usLen;
	ArgWrite.ptchBuffer = pucBuff;

	s_pfnBlInterface(FRAMEWORK_IOCTL_WINPRINT_WRITE, &ArgWrite, sizeof(ArgWrite));
}

VOID BlFwIfPrintWinStart(VOID)
{
	FRAMEWORK_IO_PRINTER ArgWrite = {0};
	s_pfnBlInterface(FRAMEWORK_IOCTL_WINPRINT_START, &ArgWrite, sizeof(ArgWrite));
}

VOID BlFwIfPrintWinEnd(VOID)
{
	FRAMEWORK_IO_PRINTER ArgWrite = {0};
	s_pfnBlInterface(FRAMEWORK_IOCTL_WINPRINT_END, &ArgWrite, sizeof(ArgWrite));
}

VOID BlFwIfPrintTransStart(LONG lPrtType, ULONG ulTransNo)
{
	FRAMEWORK_IO_PRINTER ArgWrite = {0};

	ArgWrite.lPrintStation = lPrtType;
	ArgWrite.lColumns = ulTransNo;
	s_pfnBlInterface(FRAMEWORK_IOCTL_PRINTER_TRANS_START, &ArgWrite, sizeof(ArgWrite));
}

VOID BlFwIfPrintTransEnd(LONG lPrtType, ULONG ulTransNo)
{
	FRAMEWORK_IO_PRINTER ArgWrite = {0};

	ArgWrite.lPrintStation = lPrtType;
	ArgWrite.lColumns = ulTransNo;
	s_pfnBlInterface(FRAMEWORK_IOCTL_PRINTER_TRANS_END, &ArgWrite, sizeof(ArgWrite));
}

BOOL BlFwIfBlInterface (ULONG ulIoctlId, VOID *pvDataArea, ULONG ulDataAreaSize)
{
	return s_pfnBlInterface(ulIoctlId, pvDataArea, ulDataAreaSize);
}
/*
*===========================================================================
** Synopsis:    VOID BlFwIfGetMaxOEPButtons(UINT *pszMaxButtons)
*
*
*     Input:	Keyboard Type
*	
** Return:		none
*
** Description: 
*===========================================================================
*/
VOID BlFwIfSetAutoSignOutTime (ULONG time, SHORT mode)
{
	if (s_pfnBlSetAutoSignOutTime){
		s_pfnBlSetAutoSignOutTime ( time, mode);  
	}
}

VOID BlFwIfForceShutdown ()
{
	if (s_pfnBlForceShutdown){
		s_pfnBlForceShutdown ();  // invoke the CMD_FORCE_SHUTDOWN type of call back
	}
	
}

VOID BlFwIfProcessFrameworkMessage (FRAMEWORK  *pFrameworkObject)
{
	if (s_pfnBlInterfaceFrameworkMessage){
		s_pfnBlInterfaceFrameworkMessage (pFrameworkObject);  
	}
}

/**
 *	Method to get certain data about the currently
 *	signed in Cashier for use in TOUCHLIBRARY
 *
 *	RETURN: TRUE if successful
 *			FALSE if no cashier signed in
 */
BOOL BlFwIfGetCasData(CASDATA_RTRV *data)
{
	memset(data, 0, sizeof(CASDATA_RTRV));

	data->ulCashierNo = TrnInformationPtr->TranModeQual.ulCashierID;
	//leave one character as zero for a guaranteed terminator
	memcpy(data->wchCashierName, TrnInformationPtr->TranModeQual.aszCashierMnem, sizeof(data->wchCashierName)-sizeof(data->wchCashierName[0]));
	data->usStartupWindow = TrnInformationPtr->TranModeQual.usStartupWindow;
	data->ulGroupAssociations = TrnInformationPtr->TranModeQual.ulGroupAssociations;

	return (data->ulCashierNo != 0);
}

/* ======================================== */
/* ========== End of FrameworkIf.c ======== */
/* ======================================== */
